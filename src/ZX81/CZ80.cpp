#include <ZX81/CZ80.hpp>
#include <ZX81/ULA.hpp>
#include <FZ80/INTInterrupt.hpp>
#include <FZ80/NMIInterrupt.hpp>

// ---
bool ZX81::CZ80::initialize ()
{
	_instructionCompleted = false;
	_interruptCompleted = false;
	_nmiWaitCycles = 0;

	return (FZ80::CZ80::initialize ());
}

// ---
void ZX81::CZ80::aknowledgeInterrupt
	(const MCHEmul::CPUInterruptRequest& iR)
{
	_nmiWaitCycles = 0;
	// Capture HALT before executing NMI: the generic response releases HALT
	// and adjusts PC itself. Do not duplicate those functional side effects.
	if (iR.type () == FZ80::NMIInterrupt::_ID &&
		_ula != nullptr && iR.from () == _ula && iR.reason () == 1)
	{
		bool halted = haltActive ();
		if (halted)
			_nmiWaitCycles = _ula -> haltNMIWaitCycles (iR.cycles (), clockCycles ());

		_IFDEBUG debugNMISynchronization (iR, halted, interrupt (iR.type ()) -> cyclesToLaunch ());
	}

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

	// The base call initializes the nominal response and consumes its first T.
	// Extend its remaining duration exactly once, retaining one-T execution.
	if (result && e == MCHEmul::_NOERROR &&
		_currentInterrupt != nullptr && _nmiWaitCycles != 0)
	{
		_cyclesPendingExecution += _nmiWaitCycles;
		_nmiWaitCycles = 0;
	}

	if (e != MCHEmul::_NOERROR)
		_nmiWaitCycles = 0;

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

	// The outer CPU loop adds this duration to its clock; the ULA subsequently
	// processes those elapsed cycles. Never advance either clock here directly.
	// Full execution retains atomic register/stack effects, not pin-level WAIT.
	if (result && e == MCHEmul::_NOERROR)
		_lastCPUClockCycles += _nmiWaitCycles;
	_nmiWaitCycles = 0;

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

//-----
// ---
void ZX81::CZ80::debugNMISynchronization
	(const MCHEmul::CPUInterruptRequest& iR, bool halted, unsigned int nominalCycles) const
{
	assert (_deepDebugFile != nullptr);
	unsigned int elapsed = clockCycles () - iR.cycles ();
	_deepDebugFile -> writeCompleteLine (className (), clockCycles (), "NMI Synchronization",
		{ { "Request",
			"RequestClock=" + std::to_string (iR.cycles ()) + "," +
			"AcceptanceClock=" + std::to_string (clockCycles ()) },
		  { "Synchronization",
			"HALTBefore=" + std::to_string (halted) + "," +
			"Latency=" + std::to_string (elapsed) + "," +
			"AdditionalCycles=" + std::to_string (_nmiWaitCycles) },
		  { "Model",
			"Reference=HALT4T,Applied=" + std::to_string (_nmiWaitCycles != 0) + "," +
			"Reason=" + std::string (!halted ? "NotHalted" :
				(elapsed > 4 ? "LatencyOutsideModel" :
				(_nmiWaitCycles == 0 ? "ModelNotSupported" : "PhaseCompensation"))) },
		  { "Timing",
			"NominalCycles=" + std::to_string (nominalCycles) + "," +
			"TotalCycles=" + std::to_string (nominalCycles + _nmiWaitCycles) + "," +
			"State=BeforeResponseExecution" } });
}
//-----
