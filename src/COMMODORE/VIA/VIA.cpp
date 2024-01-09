#include <COMMODORE/VIA/VIA.hpp>
#include <F6500/IRQInterrupt.hpp>

COMMODORE::VIA::VIA (int id, int rId, unsigned int intId)
	: MCHEmul::Chip (id,
		{ { "Name", "VIA" },
		  { "Code", "6526/6526A/8521" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1980" } }),
	  _registersId (rId),
	  _VIARegisters (nullptr),
	  _interruptId (intId),
	  _timer1 (0, intId /** they have to know the interruption id. */), _timer2 (1, intId),
	  _shiftRegister (0, intId),
	  _CA1 (10 /** id. */), _CA2 (11), _CB1 (20), _CB2 (21),
	  _lastClockCycles (0)
{ 
	setClassName ("VIA"); 
}

// ---
bool COMMODORE::VIA::initialize ()
{
	assert (memoryRef () != nullptr);

	// Gets the memory block dedicated to the CIA
	if (!(_VIARegisters = 
		dynamic_cast <COMMODORE::VIARegisters*> (memoryRef () -> subset (_registersId))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	// Initialize the control line...
	_CA1.initialize ();
	_CA2.initialize ();
	_CB1.initialize ();
	_CB2.initialize ();

	// ...then the timers...
	_timer1.initialize ();
	_timer2.initialize ();

	// ...now the shift register...
	/** The shift register uses the timer B in some running modes,
		for both output and input. */
	_shiftRegister.lookAtTimer (&_timer2);
	/** And also the wires, to know the signal or to output a new one. */
	_shiftRegister.lookAtControlLines (&_CB1, &_CB2);
	_shiftRegister.initialize ();

	// ...and finally all register...
	/** The registers need to access then timers, shift registers, and the control lines. */
	_VIARegisters -> lookAtControlLines (&_CA1, &_CA2, &_CB1, &_CB2);
	_VIARegisters -> lookAtTimers (&_timer1, &_timer2);
	_VIARegisters -> lookAtShiftRegister (&_shiftRegister);
	_VIARegisters -> initialize ();

	_lastClockCycles = 0;

	return (true);
}

// ---
bool COMMODORE::VIA::simulate (MCHEmul::CPU* cpu)
{
	// First time?
	if (_lastClockCycles == 0)
	{ 
		_lastClockCycles = cpu -> clockCycles (); // Nothing to do...

		return (true);
	}

	bool result = true;
	for (unsigned int i = cpu -> clockCycles () - _lastClockCycles; 
			i > 0 && result == true; i--)
	{
		// Simulations...
		// The control lines first...
		result &= _CA1.simulate (cpu);
		result &= _CA2.simulate (cpu);
		result &= _CB1.simulate (cpu);
		result &= _CB2.simulate (cpu);

		// Then simulate the timer 1...
		result &= _timer1.simulate (cpu);
		// ...and the timer 2...
		// Considering the changes (depending the execution mode) of the PB6...
		_timer2.setPB6Situation (_VIARegisters -> lastPB6 ());
		result &= _timer2.simulate (cpu);

		// And, finally simulates the shift register...
		result &= _shiftRegister.simulate (cpu);

		// After all that much, 
		// any reason to launch an interruption?...
		int cI = -1;
		if ((cI = (int) _VIARegisters -> reasonIntCode ()) != 0)
			cpu -> requestInterrupt (
				_interruptId, 
				cpu -> clockCycles  () - i, 
				this,
				cI);
	}

	_lastClockCycles = cpu -> clockCycles ();

	return (result);
}

// ---
MCHEmul::InfoStructure COMMODORE::VIA::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::Chip::getInfoStructure ());

	result.remove ("Memory"); // This is not neccesary...
	result.add ("Registers",	std::move (_VIARegisters -> getInfoStructure ()));
	result.add ("VIATimer1",	std::move (_timer1.getInfoStructure ()));
	result.add ("VIATimer2",	std::move (_timer2.getInfoStructure ()));
	result.add ("VIAShiftReg",	std::move (_shiftRegister.getInfoStructure ()));
	result.add ("PortA",		_VIARegisters -> portA ());
	result.add ("PortB",		_VIARegisters -> portB ()); // This is already a Ubyte...
	result.add ("VIACtrlA1",	std::move (_CA1.getInfoStructure ()));
	result.add ("VIACtrlA2",	std::move (_CA1.getInfoStructure ()));
	result.add ("VIACtrlB1",	std::move (_CB1.getInfoStructure ()));
	result.add ("VIACtrlB2",	std::move (_CB1.getInfoStructure ()));
	result.add ("VIADDRA",		MCHEmul::UByte (_VIARegisters -> dataPortADir ())); // To write doen it in hexadecimal form...
	result.add ("VIADDRB",		MCHEmul::UByte (_VIARegisters -> dataPortBDir ()));

	return (result);
}
