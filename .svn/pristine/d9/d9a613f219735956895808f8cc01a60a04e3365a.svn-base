#include <core/CPUInterrupt.hpp>

// ---
bool MCHEmul::CPUInterrupt::isTime (unsigned cc) const
{
	if (!_active)
		return (false);

	bool result = false;
	if ((cc - _lastClockCyclesExecuted++) >= _everyClockCycles)
	{
		result = true;

		_lastClockCyclesExecuted = cc;
	}

	return (result);
}
