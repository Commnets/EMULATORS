#include <ZX81/CZ80.hpp>
#include <ZX81/ULA.hpp>
#include <FZ80/INTInterrupt.hpp>

// ---
bool ZX81::CZ80::initialize ()
{
	_instructionCompleted = false;
	_interruptCompleted = false;

	return (FZ80::CZ80::initialize ());
}

// ---
void ZX81::CZ80::aknowledgeInterrupt
	(const MCHEmul::CPUInterruptRequest& iR)
{
	// Preserve the generic Z80 handling of the acknowledge data bus.
	FZ80::CZ80::aknowledgeInterrupt (iR);

	// A new interrupt response supersedes any earlier completion indication.
	// In per-cycle execution its request must remain untouched until completion.
	_instructionCompleted = false;
	_interruptCompleted = false;

	if (iR.type () == FZ80::INTInterrupt::_ID)
	{
		assert (_ula != nullptr);

		// INT has been accepted. This clock identifies the start of its response,
		// rather than the time when the request was originally queued.
		_ula -> setINTack (clockCycles ());
	}
}

// ---
bool ZX81::CZ80::executeNextInterruptRequest_PerCycle (unsigned int& e)
{
	bool result = FZ80::CZ80::executeNextInterruptRequest_PerCycle (e);

	// Intermediate cycles retain the current interrupt...
	// The base method clears it after completing and removing its request.
	if (result && 
		e == MCHEmul::_NOERROR && 
		_currentInterrupt == nullptr)
	{
		_instructionCompleted = false;
		_interruptCompleted = true;
	}

	return (result);
}

// ---
bool ZX81::CZ80::executeNextInterruptRequest_Full (unsigned int& e)
{
	bool result = FZ80::CZ80::executeNextInterruptRequest_Full (e);

	// A successful full response has already removed the accepted request...
	if (result && 
		e == MCHEmul::_NOERROR && 
		_currentInterrupt == nullptr)
	{
		_instructionCompleted = false;
		_interruptCompleted = true;
	}

	return (result);
}

// ---
bool ZX81::CZ80::executeNextInstruction_PerCycle (unsigned int& e)
{
	bool result = FZ80::CZ80::executeNextInstruction_PerCycle (e);

	// Returning true does not imply completion: intermediate cycles keep
	// the current instruction, which is cleared only after successful execution...
	if (result && 
		e == MCHEmul::_NOERROR && 
		_currentInstruction == nullptr)
	{
		_instructionCompleted = true;
		_interruptCompleted = false;
	}

	return (result);
}

// ---
bool ZX81::CZ80::executeNextInstruction_Full (unsigned int& e)
{
	bool result = FZ80::CZ80::executeNextInstruction_Full (e);

	// The base method can return true while reporting an instruction error.
	if (result && 
		e == MCHEmul::_NOERROR)
	{
		_instructionCompleted = true;
		_interruptCompleted = false;
	}

	return (result);
}
