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
	  _CA1 (0 /** id. */), _CA2 (1), _CB1 (2), _CB2 (3),
	  _T1 (0), _T2 (1),
	  _SR (0),
	  _PA (0), _PB (1),
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

	// Initialize the control lines...
	_CA1.linkToPort (&_PA); // When interrupt flag is set, value at _PA is latched...
	_CA1.initialize ();
	_CA2.linkToControlLine (&_CA1); // In control line mode 4, transition in _CA1, sets _CA2 back to 1...
	_CA2.initialize ();
	_CB1.linkToPort (&_PB); // @see _CA1
	_CB1.initialize ();
	_CB2.linkToControlLine (&_CB1); // @see _CA2
	_CB2.initialize ();

	// ...then the timers...
	/** In run modes = _ONESHOOTSIGNAL and _CONTINUOSSHOOTSIGNAL, 
		PB7 changes when reaches0 = true... */
	_T1.lookAtPort (&_PB); 
	_T1.initialize ();
	/** T2 has nothing to do with any port! */
	_T2.initialize (); 

	// ...now the shift register...
	/** And also the wires, to know the signal or to output a new one. */
	_SR.lookAtControlLines (&_CB1, &_CB2);
	/** The shift register uses the timer B in some running modes,
		for both output and input. */
	_SR.lookAtTimer (&_T2);
	_SR.initialize ();

	// ..now the ports...
	/** Read / write action might clear interrupt flag of the control lines linked (@see VIAControlLine)... */
	_PA.linkAtControlLines (&_CA1, &_CA2); 
	_PA.initialize ();
	/** PB7 will impact in port output 
		when T1 run modes = _ONESHOOTSIGNAL or _CONTINUOSSHOOTSIGNAL... */
	_PB.linkAtTimer (&_T1); 
	/** Read/write action might clear interrupt flag of the control lines linked (@see VIAControlLine)
		It will depend on the configuration of the control lines. */
	_PB.linkAtControlLines (&_CB1, &_CB2); 
	_PB.initialize ();

	// ...and finally all registers...
	/** The registers need to access control lines, timers, shift register and ports. */
	_VIARegisters -> lookAtControlLines (&_CA1, &_CA2, &_CB1, &_CB2);
	_VIARegisters -> lookAtTimers (&_T1, &_T2);
	_VIARegisters -> lookAtShiftRegister (&_SR);
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
		// "ControlLines"
		result &= _CA1.simulate (cpu);
		result &= _CA2.simulate (cpu);
		result &= _CB1.simulate (cpu);
		result &= _CB2.simulate (cpu);
		// "Timers"
		result &= _T1.simulate (cpu);
		result &= _T2.simulate (cpu);
		// "ShiftRegister"
		result &= _SR.simulate (cpu);
		// "Ports"
		result &= _PA.simulate (cpu);
		result &= _PB.simulate (cpu);

		// Clean up pulses control signals!
		_CA1.transition (); _CA2.transition (); 
		_CB1.transition (); _CB2.transition ();
		_PA.p6Pulse (); _PB.p6Pulse ();

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
	result.add ("VIATimer1",	std::move (_T1.getInfoStructure ()));
	result.add ("VIATimer2",	std::move (_T2.getInfoStructure ()));
	result.add ("VIAShiftReg",	std::move (_SR.getInfoStructure ()));
	result.add ("PortA",		std::move (_PA.getInfoStructure ()));
	result.add ("PortB",		std::move (_PB.getInfoStructure ()));
	result.add ("VIACtrlA1",	std::move (_CA1.getInfoStructure ()));
	result.add ("VIACtrlA2",	std::move (_CA1.getInfoStructure ()));
	result.add ("VIACtrlB1",	std::move (_CB1.getInfoStructure ()));
	result.add ("VIACtrlB2",	std::move (_CB1.getInfoStructure ()));

	return (result);
}
