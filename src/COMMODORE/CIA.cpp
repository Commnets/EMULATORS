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
	  _lastClockCycles (0)
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

	_timerA.initialize ();

	_timerB.initialize ();

	_clock.initialize ();

	_CIARegisters -> lookAtTimers (&_timerA, &_timerB);

	_CIARegisters -> lookAtClock (&_clock);

	_CIARegisters -> initialize ();

	return (true);
}

// ---
bool COMMODORE::CIA::simulate (MCHEmul::CPU* cpu)
{
	_timerA.simulate (cpu);

	_timerB.simulate (cpu, &_timerA);

	_clock.simulate (cpu);

	_lastClockCycles = cpu -> clockCycles ();

	if (_CIARegisters -> flagLineInterruptRequested ())
		cpu -> interrupt (F6500::IRQInterrupt::_ID) -> setActive (true);
	// The machine code will set this flag to 0 back...

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
