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
		_lastError = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_CIA1Registers -> lookAtTimers (&_timerA, &_timerB);

	_CIA1Registers -> lookAtClock (&_clock);

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
std::ostream& C64::operator << (std::ostream& o, const C64::CIA1& c)
{
	o << *c._CIA1Registers << std::endl;
	o << c._timerA << std::endl;
	o << c._timerB;

	return (o);
}
