#include <CORE/Clock.hpp>

// ---
void MCHEmul::Clock::start ()
{
	_tooQuick = false;

	_realCyclesPerSecond = 0;

	_realCyclesPerSecondTmp = 0;

	_realCyclesPerSecondCalculated = false;

	_iClock = std::chrono::steady_clock ().now ();
}

// ---
void MCHEmul::Clock::countCycles (unsigned int cC)
{
	// The time lasted from the last calculation (in nanoseconds) of the speed.
	// The variable iClock keeps always the last moment when the speed was calculated...
	long long elapsed = (std::chrono::steady_clock::now () - _iClock).count ();

	// If the real speed has already been calculated
	// Then it is latched in a variable accesible from outside
	// and starts back again the counter.
	// Otherwise keep counting cycles until one second has lasted.
	if (_realCyclesPerSecondCalculated)
	{
		_realCyclesPerSecond = _realCyclesPerSecondTmp;

		_iClock = std::chrono::steady_clock::now ();

		_realCyclesPerSecondTmp = 0;

		_realCyclesPerSecondCalculated = false;
	}
	else
	{
		if (elapsed < _NANOSECOND) 
			_realCyclesPerSecondTmp += cC;
		else 
			_realCyclesPerSecondCalculated = true;
	}

	// To stimate whether the clock goes or not too quick attending the last data...
	// Some tolerance is taking into account (2,5%)
	_tooQuick = (_realCyclesPerSecondTmp * _NANOSECOND) > (_cyclesPerSecond * (elapsed * 1.025));
}
