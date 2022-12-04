#include <C64/CIA1.hpp>
#include <C64/Memory.hpp>

// ---
bool C64::CIA1::initialize ()
{
	assert (memoryRef () != nullptr);

	// Gets the memory block dedicated to the CIA1
	if (!(_CIA1Registers = 
		dynamic_cast <C64::CIA1Registers*> (memoryRef () -> subset (C64::Memory::_CIA1_SUBSET))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_timerA.initialize ();

	_timerB.initialize ();

	_clock.initialize ();

	_CIA1Registers -> lookAtTimers (&_timerA, &_timerB);

	_CIA1Registers -> lookAtClock (&_clock);

	_CIA1Registers -> initialize ();

	return (true);
}

// ---
bool C64::CIA1::simulate (MCHEmul::CPU* cpu)
{
	_timerA.simulate (cpu);

	_timerB.simulate (cpu, &_timerA);

	_clock.simulate (cpu);

	return (true);
}

// ---
MCHEmul::InfoStructure C64::CIA1::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::Chip::getInfoStructure ();

	result.add ("Registers",	_CIA1Registers -> getInfoStructure ());
	result.add ("TimerA",		_timerA.getInfoStructure ());
	result.add ("TimerB",		_timerB.getInfoStructure ());
	result.add ("Clock",		_clock.getInfoStructure ());

	return (result);
}
