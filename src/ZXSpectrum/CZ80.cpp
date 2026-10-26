#include <ZXSpectrum/CZ80.hpp>

// ---
ZXSPECTRUM::CZ80::CZ80
	(int id, const FZ80::Z80PortsMap& pts)
	: FZ80::CZ80 (id, pts),
	  _memoryAccessOperation (MemoryAccessOperation::_NONE),
	  _memoryAccessState (MemoryAccessState::_IDLE),
	  _memoryAccessPC (0),
	  _memoryAccessStartClockCycle (0),
	  _recordingMemoryAccesses (false),
	  _memoryAccesses ()
{
	// Reserve a small initial capacity, without limiting the number of accesses.
	_memoryAccesses.reserve (8);
}

// ---
void ZXSPECTRUM::CZ80::recordMemoryAccess
	(const MCHEmul::Address& a, size_t nB, MemoryAccess::Type t)
{
	if (!_recordingMemoryAccesses)
		return;

	for (size_t i = 0; i < nB; i++)
	{
		unsigned short address = (unsigned short) (a.value () + i);

		// The 48K CPU subset also contains RAM outside the contended range.
		if ((address & 0xc000) == 0x4000)
			_memoryAccesses.push_back ({ address, t });
	}
}

// ---
void ZXSPECTRUM::CZ80::executeBufferedCommands (bool force)
{
	bool completing =
		_memoryAccessState == MemoryAccessState::_PENDING_PORT_COMPLETION &&
		_currentInstruction == nullptr &&
		_currentInterrupt == nullptr;

	// A block input can request its memory write during port completion.
	// Append that request to the original operation, without clearing its record.
	if (completing)
		_recordingMemoryAccesses = true;

	FZ80::CZ80::executeBufferedCommands (force);

	endMemoryAccessRecording ();

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
	// Initialization must not append accesses to a previous operation.
	_recordingMemoryAccesses = false;
	_memoryAccessOperation = MemoryAccessOperation::_NONE;
	_memoryAccessState = MemoryAccessState::_IDLE;
	_memoryAccessPC = 0;
	_memoryAccessStartClockCycle = 0;
	_memoryAccesses.clear ();

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
	// An accepted response can span several CPU calls.
	// Its first call starts recording through aknowledgeInterrupt.
	if (_currentInterrupt != nullptr)
		_recordingMemoryAccesses = true;

	bool result = FZ80::CZ80::executeNextInterruptRequest_PerCycle (e);

	endMemoryAccessRecording ();

	// A query without acceptance must leave the previous record unchanged.
	if (_memoryAccessOperation == MemoryAccessOperation::_INTERRUPT &&
		_memoryAccessState == MemoryAccessState::_IN_PROGRESS)
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
	// Recording starts only if the base implementation accepts the request.
	bool result = FZ80::CZ80::executeNextInterruptRequest_Full (e);

	endMemoryAccessRecording ();

	if (_memoryAccessOperation == MemoryAccessOperation::_INTERRUPT &&
		_memoryAccessState == MemoryAccessState::_IN_PROGRESS)
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
		_recordingMemoryAccesses = true;

	bool result = FZ80::CZ80::executeNextInstruction_PerCycle (e);

	endMemoryAccessRecording ();

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

	endMemoryAccessRecording ();

	finishMemoryAccessOperation
		(!result || e != MCHEmul::_NOERROR);

	return (result);
}

// ---
void ZXSPECTRUM::CZ80::beginMemoryAccessOperation
	(MemoryAccessOperation o)
{
	_memoryAccessOperation = o;
	_memoryAccessState = MemoryAccessState::_IN_PROGRESS;
	_memoryAccessPC =
		(unsigned short) (programCounter ().asAddress ().value ());
	_memoryAccessStartClockCycle = clockCycles ();

	beginMemoryAccessRecording ();
}

// ---
void ZXSPECTRUM::CZ80::finishMemoryAccessOperation (bool failed)
{
	endMemoryAccessRecording ();

	if (failed)
		_memoryAccessState = MemoryAccessState::_FAILED;
	else if (hasPendingPortAccess ())
		_memoryAccessState = MemoryAccessState::_PENDING_PORT_COMPLETION;
	else
		_memoryAccessState = MemoryAccessState::_COMPLETED;
}

// ---
void ZXSPECTRUM::CZ80::beginMemoryAccessRecording ()
{
	// Reuse allocated storage for the next operation.
	_memoryAccesses.clear ();
	_recordingMemoryAccesses = true;
}
