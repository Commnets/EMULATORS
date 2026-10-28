#include <ZXSpectrum/CZ80.hpp>
#include <ZXSpectrum/ULA.hpp>
#include <FZ80/Instruction.hpp>

// ---
ZXSPECTRUM::CZ80::MemoryAccessTransaction::MemoryAccessTransaction ()
	: _operation (MemoryAccessOperation::_NONE),
	  _state (MemoryAccessState::_IDLE),
	  _pc (0),
	  _startClockCycle (0),
	  _timingAvailable (false),
	  _contentionHandled (false),
	  _contentionClockCycles (0),
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
	_timingAvailable = false;
	_contentionHandled = false;
	_contentionClockCycles = 0;
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
	_timingAvailable = false;
	_contentionHandled = false;
	_contentionClockCycles = 0;

	// A new operation owns a new record, reusing its allocated storage.
	_accesses.clear ();
	_recording = true;
}

// ---
void ZXSPECTRUM::CZ80::MemoryAccessTransaction::finish
	(bool failed, bool pendingPortAccess)
{
	suspendRecording ();
	_timingAvailable = false;

	if (failed)
		_state = MemoryAccessState::_FAILED;
	else if (pendingPortAccess)
		_state = MemoryAccessState::_PENDING_PORT_COMPLETION;
	else
		_state = MemoryAccessState::_COMPLETED;
}

// ---
void ZXSPECTRUM::CZ80::MemoryAccessTransaction::setContentionDelay
	(unsigned int cC)
{
	// A calculated zero delay must also suppress legacy reconstruction.
	_contentionHandled = true;
	_contentionClockCycles = cC;
	_pendingContentionClockCycles = cC;
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

	// When multiple bytes are accessed...
	// ...it might happen that one is contended and the other is not!
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
const FZ80::Instruction* ZXSPECTRUM::CZ80::executedInstruction () const
{
	// Resolve the already executed leaf without decoding memory again...
	const MCHEmul::Instruction* inst = _lastInstruction;
	while (inst != nullptr)
	{
		const MCHEmul::InstructionUndefined* undefined =
			dynamic_cast <const MCHEmul::InstructionUndefined*> (inst);
		if (undefined == nullptr)
			break; // Exit in normal situations, because it has been found!

		inst = undefined -> lastInstruction ();
	}

	return (dynamic_cast <const FZ80::Instruction*> (inst));
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
		defined -> numberMCyclesStructures () != 1)
		return (false);

	const FZ80::MCyclesData& timing = defined -> mCyclesData ();
	const std::vector <size_t>& reads = timing.memoryReads ();
	const std::vector <size_t>& writes = timing.memoryWrites ();

	size_t instructionBytes = defined -> memoryPositions ();

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

	if (_ula == nullptr || !transaction._timingAvailable)
		return;

	// Preserve the old path until it has resolved its earlier accesses.
	if (_ula -> legacyMemoryContentionPending ())
		return;

	// Mixed batches need a separate policy for combining old and new operations.
	if (ticksCounter () != nullptr && ticksCounter () -> elapsedTicks () != 1)
		return;

	const FZ80::Instruction* defined = executedInstruction ();

	if (defined == nullptr || !defined -> mCyclesData ().ioCycles ().empty ())
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

	_memoryAccessTransaction.setTimingAvailable
		(associateMemoryAccessTiming ());

	prepareMemoryContentionDelay ();
}
