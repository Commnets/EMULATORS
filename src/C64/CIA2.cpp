#include <C64/CIA2.hpp>
#include <C64/Memory.hpp>
#include <C64/VICII.hpp>
#include <F6500/NMIInterrupt.hpp>

// ---
bool C64::CIA2::initialize ()
{
	assert (memoryRef () != nullptr);

	// Gets the memory block dedicated to the CIA2
	if (!(_CIA2Registers = 
		dynamic_cast <C64::CIA2Registers*> (memoryRef () -> subset (C64::Memory::_CIA2_SUBSET))))
	{
		_lastError = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_CIA2Registers -> lookAtTimers (&_timerA, &_timerB);

	_CIA2Registers -> lookAtClock (&_clock);

	return (true);
}

// ---
bool C64::CIA2::simulate (MCHEmul::CPU* cpu)
{
	_VICIIRef -> setBank (_CIA2Registers -> VICIIBank ());

	_timerA.simulate (cpu);

	_timerB.simulate (cpu, &_timerA);

	_clock.simulate (cpu);

	_lastClockCycles = cpu -> clockCycles ();

	return (true);
}

// ---
std::ostream& C64::operator << (std::ostream& o, const C64::CIA2& c)
{
	o << *c._CIA2Registers << std::endl;
	o << c._timerA << std::endl;
	o << c._timerB;

	return (o);
}
