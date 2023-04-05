#include <COMMODORE/CIA.hpp>
#include <F6500/IRQInterrupt.hpp>

COMMODORE::CIA::CIA (int id, int rId, unsigned int intId)
	: MCHEmul::Chip (id,
		{ { "Name", "CIA" },
		  { "Code", "6526/6526A/8521" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1980" } }),
	  _CIARegisters (nullptr),
	  _registersId (rId),
	  _timerA (0, intId /** they have to know the interruption id. */), _timerB (1, intId), 
	  _clock (0, intId),
	  _lastClockCycles (0),
	  _timerAValueAtPortB (0), _pulseTimerASentToPortB (false),
	  _timerBValueAtPortB (0), _pulseTimerBSentToPortB (false)
{ 
	setClassName ("CIA"); 
}

// ---
bool COMMODORE::CIA::initialize ()
{
	assert (memoryRef () != nullptr);

	// Gets the memory block dedicated to the CIA
	if (!(_CIARegisters = 
		dynamic_cast <COMMODORE::CIARegisters*> (memoryRef () -> subset (_registersId))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_CIARegisters -> initializeInternalValues ();

	_timerA.initialize ();

	_timerB.initialize ();

	_clock.initialize ();

	_CIARegisters -> lookAtTimers (&_timerA, &_timerB);

	_CIARegisters -> lookAtClock (&_clock);

	_CIARegisters -> initialize ();

	_pulseTimerASentToPortB = _pulseTimerBSentToPortB;

	_timerAValueAtPortB = _timerBValueAtPortB = 0; // Do not anything...

	return (true);
}

// ---
bool COMMODORE::CIA::simulate (MCHEmul::CPU* cpu)
{
	// Simulate the Timer...
	// After that the timer can reach 0
	// If so (apart of launching a interrupt if configured) the result can be reflected
	// in the port B of the Chip...
	_timerA.simulate (cpu);
	if (_timerA.affectPortDataB ())
	{
		if (_timerA.reaches0 ())
			_timerAValueAtPortB = (_pulseTimerASentToPortB = _timerA.pulseAtPortDataB ()) 
				? 1 : (_timerAValueAtPortB == 1) ? 2 : 1;
		else
		if (_pulseTimerASentToPortB)
		{
			_pulseTimerASentToPortB = false;

			_timerAValueAtPortB = 2; // Swith it off...
		}
		else
			_timerAValueAtPortB = 0; // Do nothing...
	}
	else
		_timerAValueAtPortB = 0; // Do nothing...

	// Finally the value reflected is set into the registers...
	_CIARegisters -> setReflectTimerAAtPortDataB (_timerAValueAtPortB /** 0 means do nothing. */);

	// Same but with timer B
	// The timer B has to take into account the timer A...
	_timerB.simulate (cpu, &_timerA);
	if (_timerB.affectPortDataB ())
	{
		if (_timerB.reaches0 ())
			_timerBValueAtPortB = (_pulseTimerBSentToPortB = _timerB.pulseAtPortDataB ()) 
				? 1 : (_timerBValueAtPortB == 1) ? 2 : 1;
		else
		if (_pulseTimerBSentToPortB)
		{
			_pulseTimerBSentToPortB = false;

			_timerBValueAtPortB = 2;
		}
		else
			_timerBValueAtPortB = 0;
	}
	else
		_timerBValueAtPortB = 0;

	_CIARegisters -> setReflectTimerAAtPortDataB (_timerBValueAtPortB /** 0 means do nothing. */);

	_clock.simulate (cpu);

	_lastClockCycles = cpu -> clockCycles ();

	if (_CIARegisters -> flagLineInterruptRequested ())
		cpu -> interrupt (F6500::IRQInterrupt::_ID) -> setActive (true);

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::CIA::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::Chip::getInfoStructure ();

	result.remove ("Memory"); // This is not neccesary...
	result.add ("Registers",	_CIARegisters -> getInfoStructure ());
	result.add ("TimerA",		_timerA.getInfoStructure ());
	result.add ("TimerB",		_timerB.getInfoStructure ());
	result.add ("Clock",		_clock.getInfoStructure ());

	return (result);
}
