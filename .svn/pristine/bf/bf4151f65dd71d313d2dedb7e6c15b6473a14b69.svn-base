#include <CORE/CPUInterrupt.hpp>
#include <CORE/CPU.hpp>

// ---
bool MCHEmul::CPUInterrupt::executeOver (MCHEmul::CPU* c, unsigned int& nC)
{
	assert (c != nullptr);

	bool result = false;

	if (active () && isTime (c))
	{
		_lastClockCyclesExecuted = c -> clockCycles ();

		executeOverImpl (c, nC);

		result = true;
	}

	return (result);
}
