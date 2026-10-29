#include <ZXSpectrum/CZ80.hpp>
#include <ZXSpectrum/ULA.hpp>
#include <FZ80/Instruction.hpp>

// ---
ZXSPECTRUM::CZ80::MemoryAccessTransaction::MemoryAccessTransaction ()
	: _operation (MemoryAccessOperation::_NONE),
	  _state (MemoryAccessState::_IDLE),
	  _pc (0),
	  _startClockCycle (0),
	  _mCyclesStructure (FZ80::Instruction::_NOMCYCLESSTRUCTURE),
	  _timingAvailable (false),
	  _contentionHandled (false),
	  _contentionClockCycles (0),
	  _includedContentionClockCycles (0),
	  _pendingContentionClockCycles (0),
	  _recording (false),
	  _accesses ()
{
	// Reserve once; clearing transactions preserves this storage.
	_accesses.reserve (8);
}

// ---
void ZXSPECTRUM::CZ80::MemoryAccessTransaction::reset ()
{
	_operation = MemoryAccessOperation::_NONE;
	_state = MemoryAccessState::_IDLE;
	_pc = 0;
	_startClockCycle = 0;
	_mCyclesStructure = FZ80::Instruction::_NOMCYCLESSTRUCTURE;
	_timingAvailable = false;
	_contentionHandled = false;
	_contentionClockCycles = 0;
	_includedContentionClockCycles = 0;
	_pendingContentionClockCycles = 0;
	_recording = false;

	// Initialization abandons the previous operation but retains capacity.
	_accesses.clear ();
}

// ---
void ZXSPECTRUM::CZ80::MemoryAccessTransaction::begin
	(MemoryAccessOperation o, unsigned short pc, unsigned int cC)
{
	// Replacing an operation must never discard unconsumed contention wait.
	assert (_pendingContentionClockCycles == 0);

	_operation = o;
	_state = MemoryAccessState::_IN_PROGRESS;
	_pc = pc;
	_startClockCycle = cC;
	_mCyclesStructure = FZ80::Instruction::_NOMCYCLESSTRUCTURE;
	_timingAvailable = false;
	_contentionHandled = false;
	_contentionClockCycles = 0;
	_includedContentionClockCycles = 0;

	// A new operation owns a new record, reusing its allocated storage.
	_accesses.clear ();
	_recording = true;
}

// ---
void ZXSPECTRUM::CZ80::MemoryAccessTransaction::finish
	(bool failed, bool pendingPortAccess)
{
	suspendRecording ();
	_mCyclesStructure = FZ80::Instruction::_NOMCYCLESSTRUCTURE;
	_timingAvailable = false;

	if (failed)
		_state = MemoryAccessState::_FAILED;
	else if (pendingPortAccess)
		_state = MemoryAccessState::_PENDING_PORT_COMPLETION;
	else
		_state = MemoryAccessState::_COMPLETED;
}

// ---
void ZXSPECTRUM::CZ80::MemoryAccessTransaction::setTimingAssociation (size_t n)
{
	_mCyclesStructure = n;
	_timingAvailable = true;
}

// ---
void ZXSPECTRUM::CZ80::MemoryAccessTransaction::setContentionDelay
	(unsigned int total, unsigned int included)
{
	assert (included <= total);

	// A calculated zero delay is also a handled operation.
	_contentionHandled = true;
	_contentionClockCycles = total;
	_includedContentionClockCycles = included;
	_pendingContentionClockCycles = total - included;
}

// ---
unsigned int
ZXSPECTRUM::CZ80::MemoryAccessTransaction::consumeContentionDelay (bool full)
{
	if (_pendingContentionClockCycles == 0)
		return (0);

	unsigned int cycles = full ? _pendingContentionClockCycles : 1;

	_pendingContentionClockCycles -= cycles;

	return (cycles);
}

// ---
ZXSPECTRUM::CZ80::CZ80
	(int id, const FZ80::Z80PortsMap& pts)
	: FZ80::CZ80 (id, pts),
	  _ula (nullptr),
	  _memoryAccessTransaction ()
{
	// Nothing else.
}

// ---
void ZXSPECTRUM::CZ80::recordMemoryAccess
	(const MCHEmul::Address& a, size_t nB, MemoryAccess::Type t)
{
	if (!_memoryAccessTransaction._recording)
		return;

	// Continues only when the relevant types of memory accesses are being recorded.
	MemoryAccess::Origin origin = MemoryAccess::Origin::_DATA;
	if (t == MemoryAccess::Type::_READ)
	{
		switch (memoryAccessContext ())
		{
			case MCHEmul::CPU::MemoryAccessContext::_NORMAL:
				{
					// Instruction bytes are represented by their classified loading.
					if (fetchingInstructionCode ())
						return; // Not relevant at all...
				}

				break;

			case MCHEmul::CPU::MemoryAccessContext::_INSPECTION:
			case MCHEmul::CPU::MemoryAccessContext::_INSTRUCTION_SELECTION:
					return; // Not relevant at all...

			case MCHEmul::CPU::MemoryAccessContext::_INSTRUCTION_BYTES:
				{
					// IM0 obtains its first opcode from the acknowledge bus.
					// Its generic parameter loading must not invent a memory fetch.
					if (_memoryAccessTransaction._operation == MemoryAccessOperation::_INTERRUPT)
						return; // Not relevant at all...

					origin = MemoryAccess::Origin::_INSTRUCTION_BYTE;
				}

				break;

			// It should not be possible to reach this point with an unknown context...
			default:
				return;
		}
	}

	// Retain contained bytes even when the requested range crosses a boundary.
	// Associating a partially retained data-transfer group can still be unsupported.
	for (size_t i = 0; i < nB; i++)
	{
		// The 48K CPU subset also contains RAM outside the contended range.
		unsigned short address = (unsigned short) (a.value () + i);
		if ((address & 0xc000) == 0x4000)
			_memoryAccessTransaction._accesses.push_back
				({ address, t, origin, FZ80::MCycle::_NOCYCLE,
				   FZ80::MCycle::_NOCYCLE, FZ80::MCycle::_NOCYCLE });
	}
}

// ---
FZ80::CZ80::IOAccessTiming ZXSPECTRUM::CZ80::calculateIOAccessTiming
	(const FZ80::Instruction& instruction,
	 const FZ80::Z80Port::Access& access, unsigned int cC)
{
	IOAccessTiming result =
		calculateMemoryAndIOAccessTiming (instruction, access, cC);

	// Report once during preparation, including port-only calculations.
	if (_ula != nullptr &&
		(result._memoryBefore != 0 || result._port != 0 || result._memoryAfter != 0))
		_ula -> notifyContention ();

	return (result);
}

// ---
void ZXSPECTRUM::CZ80::executeBufferedCommands (bool force)
{
	// A terminal contention wait must elapse before its buffered effects
	// become visible, including debugger-requested releases.
	if (_memoryAccessTransaction._pendingContentionClockCycles != 0)
		return;

	bool completing =
		_memoryAccessTransaction._state ==
			MemoryAccessState::_PENDING_PORT_COMPLETION &&
		_currentInstruction == nullptr &&
		_currentInterrupt == nullptr;

	// A block input can request its memory write during port completion...
	// Append that request to the original operation, without clearing its record.
	if (completing)
		_memoryAccessTransaction.resumeRecording ();

	FZ80::CZ80::executeBufferedCommands (force);

	_memoryAccessTransaction.suspendRecording ();

	if (completing)
	{
		if (_error != MCHEmul::_NOERROR)
			finishMemoryAccessOperation (true);
		else if (!hasPendingPortAccess ())
			finishMemoryAccessOperation (false);
	}
}

// ---
bool ZXSPECTRUM::CZ80::initialize ()
{
	// Initialization abandons any previous capture and contention wait.
	_memoryAccessTransaction.reset ();

	return (FZ80::CZ80::initialize ());
}

// ---
void ZXSPECTRUM::CZ80::aknowledgeInterrupt
	(const MCHEmul::CPUInterruptRequest& iR)
{
	// Both execution modes acknowledge only an accepted request.
	// Preserve the PC and clock before the response changes CPU state.
	beginMemoryAccessOperation (MemoryAccessOperation::_INTERRUPT);

	FZ80::CZ80::aknowledgeInterrupt (iR);
}

// ---
bool ZXSPECTRUM::CZ80::executeNextInterruptRequest_PerCycle
	(unsigned int& e)
{
	// A consumed wait step must not acknowledge an interrupt or fetch an opcode.
	if (consumeMemoryContentionDelay (false))
		return (true);

	// An accepted response can span several CPU calls.
	// Its first call starts recording through aknowledgeInterrupt.
	if (_currentInterrupt != nullptr)
		_memoryAccessTransaction.resumeRecording ();

	bool result = FZ80::CZ80::executeNextInterruptRequest_PerCycle (e);

	_memoryAccessTransaction.suspendRecording ();

	// A query without acceptance must leave the previous record unchanged.
	if (_memoryAccessTransaction._operation == MemoryAccessOperation::_INTERRUPT &&
		_memoryAccessTransaction._state == MemoryAccessState::_IN_PROGRESS)
	{
		if (e != MCHEmul::_NOERROR)
			finishMemoryAccessOperation (true);
		else if (result && _currentInterrupt == nullptr)
			finishMemoryAccessOperation (false);
	}

	return (result);
}

// ---
bool ZXSPECTRUM::CZ80::executeNextInterruptRequest_Full
	(unsigned int& e)
{
	if (consumeMemoryContentionDelay (true))
		return (true);

	// Recording starts only if the base implementation accepts the request.
	bool result = FZ80::CZ80::executeNextInterruptRequest_Full (e);

	_memoryAccessTransaction.suspendRecording ();

	if (_memoryAccessTransaction._operation == MemoryAccessOperation::_INTERRUPT &&
		_memoryAccessTransaction._state == MemoryAccessState::_IN_PROGRESS)
	{
		if (e != MCHEmul::_NOERROR)
			finishMemoryAccessOperation (true);
		else if (result && _currentInterrupt == nullptr)
			finishMemoryAccessOperation (false);
	}

	return (result);
}

// ---
bool ZXSPECTRUM::CZ80::executeNextInstruction_PerCycle
	(unsigned int& e)
{
	// Preserve the base behavior while an interrupt owns execution.
	if (_currentInterrupt != nullptr)
		return (FZ80::CZ80::executeNextInstruction_PerCycle (e));

	if (_currentInstruction == nullptr)
		beginMemoryAccessOperation (MemoryAccessOperation::_INSTRUCTION);
	else
		_memoryAccessTransaction.resumeRecording ();

	bool result = FZ80::CZ80::executeNextInstruction_PerCycle (e);

	_memoryAccessTransaction.suspendRecording ();

	if (e != MCHEmul::_NOERROR)
		finishMemoryAccessOperation (true);
	else if (result && _currentInstruction == nullptr)
		finishMemoryAccessOperation (false);

	return (result);
}

// ---
bool ZXSPECTRUM::CZ80::executeNextInstruction_Full
	(unsigned int& e)
{
	beginMemoryAccessOperation (MemoryAccessOperation::_INSTRUCTION);

	bool result = FZ80::CZ80::executeNextInstruction_Full (e);

	_memoryAccessTransaction.suspendRecording ();

	finishMemoryAccessOperation
		(!result || e != MCHEmul::_NOERROR);

	return (result);
}

// ---
FZ80::CZ80::IOAccessTiming ZXSPECTRUM::CZ80::calculateMemoryAndIOAccessTiming
	(const FZ80::Instruction& instruction,
	 const FZ80::Z80Port::Access& access, unsigned int cC)
{
	MemoryAccessTransaction& transaction = _memoryAccessTransaction;

	// Memory timing remains limited to supported single-operation contexts.
	// The generic calculation still applies the port's own contention.
	if (_ula == nullptr ||
		transaction._operation != MemoryAccessOperation::_INSTRUCTION ||
		transaction._state != MemoryAccessState::_IN_PROGRESS ||
		transaction._startClockCycle != cC ||
		(ticksCounter () != nullptr && ticksCounter () -> elapsedTicks () != 1))
		return (FZ80::CZ80::calculateIOAccessTiming (instruction, access, cC));

	const FZ80::MCyclesData& timing =
		instruction.mCyclesData (instruction.executedMCyclesStructure ());
	assert (timing.ioCycles ().size () == 1);

	size_t ioIndex = timing.ioCycles ()[0];
	const FZ80::MCycleData& io = timing.cycleData (ioIndex);
	const std::vector <size_t>& reads = timing.memoryReads ();
	const std::vector <size_t>& writes = timing.memoryWrites ();

	// Inputs have only instruction reads and at most one following write.
	// Outputs have at most one preceding data read and no memory writes.
	bool input = access._type == FZ80::Z80Port::Access::Type::_READ;
	if (reads.size () < instruction.memoryPositions () ||
		(input && (reads.size () != instruction.memoryPositions () || writes.size () > 1)) ||
		(!input && (!writes.empty () || reads.size () - instruction.memoryPositions () > 1)))
		return (FZ80::CZ80::calculateIOAccessTiming (instruction, access, cC));

	const FZ80::MCycleData* followingWrite = nullptr;
	if (!writes.empty ())
	{
		followingWrite = &timing.cycleData (writes [0]);
		if (writes [0] <= ioIndex || followingWrite -> _contentionCycle == FZ80::MCycle::_NOCYCLE)
			return (FZ80::CZ80::calculateIOAccessTiming (instruction, access, cC));
	}

	if (!associateMemoryAccessTiming (instruction, timing))
		return (FZ80::CZ80::calculateIOAccessTiming (instruction, access, cC));

	// The observed prefix must precede I/O; a deferred input write is not yet recorded.
	for (const auto& memoryAccess : transaction._accesses)
		if (memoryAccess._mCycle >= ioIndex)
			return (FZ80::CZ80::calculateIOAccessTiming (instruction, access, cC));

	IOAccessTiming result;
	for (const auto& memoryAccess : transaction._accesses)
		result._memoryBefore += _ula -> memoryContentionDelayAt
			(cC + (unsigned int) (memoryAccess._contentionCycle) + result._memoryBefore);

	// Query the port once, after preceding memory waits have shifted its phase.
	result._port = additionalClockCyclesForIO
		(access._address, (unsigned char) (access._address & 0x00ff),
		 cC + (unsigned int) (io._contentionCycle) + result._memoryBefore);

	if (followingWrite != nullptr && (access._readContext._memoryAddress & 0xc000) == 0x4000)
		// Timing and eventual completion use the same captured destination.
		result._memoryAfter = _ula -> memoryContentionDelayAt
			(cC + (unsigned int) (followingWrite -> _contentionCycle) +
				result._memoryBefore + result._port);

	unsigned int memoryDelay = result._memoryBefore + result._memoryAfter;
	transaction.setContentionDelay (memoryDelay, memoryDelay);

	return (result);
}

// ---
const FZ80::Instruction* ZXSPECTRUM::CZ80::executedInstruction () const
{
	const MCHEmul::Instruction* instruction = _lastInstruction;

	const MCHEmul::InstructionUndefined* undefined =
		dynamic_cast <const MCHEmul::InstructionUndefined*> (instruction);
	if (undefined != nullptr)
		instruction = undefined -> lastDefinedInstruction ();

	return (dynamic_cast <const FZ80::Instruction*> (instruction));
}

// ---
bool ZXSPECTRUM::CZ80::associateMemoryAccessTiming ()
{
	MemoryAccessTransaction& transaction = _memoryAccessTransaction;

	if (transaction._operation != MemoryAccessOperation::_INSTRUCTION ||
		transaction._state != MemoryAccessState::_COMPLETED)
		return (false);

	const FZ80::Instruction* defined = executedInstruction ();
	if (defined == nullptr || !defined -> hasMCycles () ||
		!defined -> hasExecutedMCyclesStructure ())
		return (false);

	const FZ80::MCyclesData& timing =
		defined -> mCyclesData (defined -> executedMCyclesStructure ());
	if (!associateMemoryAccessTiming (*defined, timing))
		return (false);

	transaction.setTimingAssociation (defined -> executedMCyclesStructure ());

	return (true);
}

// ---
bool ZXSPECTRUM::CZ80::associateMemoryAccessTiming
	(const FZ80::Instruction& instruction, const FZ80::MCyclesData& timing)
{
	MemoryAccessTransaction& transaction = _memoryAccessTransaction;

	const std::vector <size_t>& reads = timing.memoryReads ();
	const std::vector <size_t>& writes = timing.memoryWrites ();

	size_t instructionBytes = instruction.memoryPositions ();

	// Supported ordinary Z80 sequences obtain their instruction bytes
	// before performing the remaining memory reads.
	if (reads.size () < instructionBytes)
		return (false);

	// Calculate the dataReads and dataWrites observed during execution...
	size_t dataReads = 0, dataWrites = 0;
	for (const auto& access : transaction._accesses)
	{
		if (access._origin == MemoryAccess::Origin::_DATA)
		{
			if (access._type == MemoryAccess::Type::_READ) dataReads++;
			else dataWrites++;
		}
	}

	// A partially retained group of equal-direction data transfers is ambiguous:
	// its omitted accesses could precede or follow the retained ones.
	// Reject its timing association; the recorded bytes remain available without a fallback delay.
	if ((dataReads != 0 && dataReads != reads.size () - instructionBytes) ||
		(dataWrites != 0 && dataWrites != writes.size ()))
		return (false);

	// Calculates the M-cycle indices for the retained accesses, preserving their original order...
	size_t readIndex = instructionBytes, writeIndex = 0;
	size_t previousCycle = FZ80::MCycle::_NOCYCLE;
	for (auto& access : transaction._accesses)
	{
		size_t mCycle;

		if (access._origin == MemoryAccess::Origin::_INSTRUCTION_BYTE)
		{
			if (access._type != MemoryAccess::Type::_READ)
				return (false);

			// Preserve instruction-byte position even when earlier bytes
			// were outside the retained address range.
			size_t byteIndex =
				(unsigned short) (access._address - transaction._pc);

			if (byteIndex >= instructionBytes)
				return (false);

			mCycle = reads [byteIndex];
		}
		else if (access._type == MemoryAccess::Type::_READ)
			mCycle = reads [readIndex++];
		else
			mCycle = writes [writeIndex++];

		const FZ80::MCycleData& cycle = timing.cycleData (mCycle);

		if (cycle._contentionCycle == FZ80::MCycle::_NOCYCLE ||
			cycle._dataCycle == FZ80::MCycle::_NOCYCLE)
			return (false);

		// Do not publish a mapping whose transfer order disagrees
		// with the order observed during execution.
		if (previousCycle != FZ80::MCycle::_NOCYCLE &&
			cycle._dataCycle <= previousCycle)
			return (false);

		access._mCycle = mCycle;
		access._contentionCycle = cycle._contentionCycle;
		access._dataCycle = cycle._dataCycle;

		previousCycle = cycle._dataCycle;
	}

	return (true);
}

// ---
void ZXSPECTRUM::CZ80::prepareMemoryContentionDelay ()
{
	MemoryAccessTransaction& transaction = _memoryAccessTransaction;

	if (_ula == nullptr || !transaction._timingAvailable ||
		transaction._contentionHandled)
		return;

	// Multi-tick batches remain outside the supported memory-timing scope.
	if (ticksCounter () != nullptr && ticksCounter () -> elapsedTicks () != 1)
		return;

	const FZ80::Instruction* defined = executedInstruction ();

	if (defined == nullptr)
		return;

	const FZ80::MCyclesData& timing =
		defined -> mCyclesData (transaction._mCyclesStructure);
	if (!timing.ioCycles ().empty ())
		return;

	unsigned int delay = 0;

	for (const auto& access : transaction._accesses)
	{
		// Each earlier wait shifts the contention phase of subsequent accesses.
		delay += _ula -> memoryContentionDelayAt
			(transaction._startClockCycle +
				(unsigned int) (access._contentionCycle) + delay);
	}

	transaction.setContentionDelay (delay);

	if (delay != 0)
		_ula -> notifyContention ();
}

// ---
bool ZXSPECTRUM::CZ80::consumeMemoryContentionDelay (bool full)
{
	unsigned int cycles =
		_memoryAccessTransaction.consumeContentionDelay (full);

	if (cycles == 0)
		return (false);

	// The outer CPU loop performs the actual clock advancement once.
	_lastCPUClockCycles = cycles;

	return (true);
}

// ---
void ZXSPECTRUM::CZ80::beginMemoryAccessOperation
	(MemoryAccessOperation o)
{
	_memoryAccessTransaction.begin
		(o, (unsigned short) (programCounter ().asAddress ().value ()),
		 clockCycles ());
}

// ---
void ZXSPECTRUM::CZ80::finishMemoryAccessOperation (bool failed)
{
	_memoryAccessTransaction.finish (failed, hasPendingPortAccess ());

	if (_memoryAccessTransaction._state != MemoryAccessState::_COMPLETED)
		return;

	if (associateMemoryAccessTiming ())
		prepareMemoryContentionDelay ();
}
