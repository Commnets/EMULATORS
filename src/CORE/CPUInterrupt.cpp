#include <CORE/CPUInterrupt.hpp>
#include <CORE/CPU.hpp>

// ---
bool MCHEmul::CPUInterrupt::executeOver (MCHEmul::CPU* c, unsigned int& nC)
{
	assert (c != nullptr);

	bool result = false;

	if (active () && isTime (c))
	{
		setActive (false);

		_lastClockCyclesExecuted = c -> clockCycles ();

		result = executeOverImpl (c, nC);
	}

	return (result);
}
