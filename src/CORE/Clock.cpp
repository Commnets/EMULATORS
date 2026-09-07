#include <CORE/Clock.hpp>

// ---
MCHEmul::Clock::Clock (unsigned int cS, double f)
	: _cyclesPerSecond (cS),
	  _realCyclesPerSecond (0),
	  _ticks (),
	  _factor (f),
	  _tooQuick (false),
	  _realCyclesPerSecondTmp (0),
	  _cyclesPerAdjustment (1),
	  _cyclesSinceLastAdjustment (0),
	  _requestsSinceLastTimeCheck (0),
	  _iClock (),
	  _adjustmentClock (),
	  _resumeClock ()
{
	assert (_cyclesPerSecond > 0);
	assert (_factor > 0.0f);

	actualizeCyclesPerAdjustment ();

	start ();
}

// ---
void MCHEmul::Clock::setFactor (double f)
{
	assert (f > 0.0f);

	std::chrono::time_point <std::chrono::steady_clock> n =
		std::chrono::steady_clock::now ();

	_factor = f;

	actualizeCyclesPerAdjustment ();

	// A speed debt calculated with the previous factor is no longer valid.
	_tooQuick = false;
	_cyclesSinceLastAdjustment = 0;
	_requestsSinceLastTimeCheck = 0;
	_adjustmentClock = _resumeClock = n;
}

// ---
void MCHEmul::Clock::start ()
{
	std::chrono::time_point <std::chrono::steady_clock> n =
		std::chrono::steady_clock::now ();

	_tooQuick = false;

	_realCyclesPerSecond = 0;
	_realCyclesPerSecondTmp = 0;

	_cyclesSinceLastAdjustment = 0;
	_requestsSinceLastTimeCheck = 0;

	_iClock = _adjustmentClock = _resumeClock = n;

	for (auto& i : _ticks)
		i.reset ();
}

// ---
void MCHEmul::Clock::countCycles (unsigned int cC)
{
	// Emulated cycles and derived ticks never depend on a real-time boundary.
	_realCyclesPerSecondTmp += cC;

	for (auto& i : _ticks)
		i.count (cC);

	if (cC == 0)
	{
		// Skipping checks can only delay resumption. It can never make
		// the clock resume before the calculated deadline.
		if (++_requestsSinceLastTimeCheck <
			MCHEmul::Clock::_REQUESTSBETWEENTIMECHECKS)
			return;

		_requestsSinceLastTimeCheck = 0;

		std::chrono::time_point <std::chrono::steady_clock> n =
			std::chrono::steady_clock::now ();

		actualizeMeasurement (n);

		if (_tooQuick && n >= _resumeClock)
			finishAdjustment (n);

		return;
	}

	_requestsSinceLastTimeCheck = 0;

	_cyclesSinceLastAdjustment += cC;
	if (_cyclesSinceLastAdjustment < _cyclesPerAdjustment)
		return;

	std::chrono::time_point <std::chrono::steady_clock> n =
		std::chrono::steady_clock::now ();

	actualizeMeasurement (n);

	_resumeClock = _adjustmentClock + std::chrono::nanoseconds
		((long long) (((double) _cyclesSinceLastAdjustment *
			(double) MCHEmul::Clock::_NANOSECOND) /
			((double) _cyclesPerSecond * _factor)));

	_tooQuick = n < _resumeClock;

	if (!_tooQuick)
		finishAdjustment (n);
}

// ---
void MCHEmul::Clock::actualizeCyclesPerAdjustment ()
{
	_cyclesPerAdjustment = (unsigned int)
		(((double) _cyclesPerSecond * _factor) /
			(double) MCHEmul::Clock::_ADJUSTMENTSPERSECOND);

	if (_cyclesPerAdjustment == 0)
		_cyclesPerAdjustment = 1;
}

// ---
void MCHEmul::Clock::actualizeMeasurement
	(const std::chrono::time_point <std::chrono::steady_clock>& n)
{
	long long elapsed = std::chrono::duration_cast <std::chrono::nanoseconds>
		(n - _iClock).count ();

	if (elapsed < MCHEmul::Clock::_NANOSECOND)
		return;

	_realCyclesPerSecond = (unsigned int)
		((_realCyclesPerSecondTmp * MCHEmul::Clock::_NANOSECOND) /
			(unsigned long long) elapsed);

	_realCyclesPerSecondTmp = 0;
	_iClock = n;
}

// ---
void MCHEmul::Clock::finishAdjustment
	(const std::chrono::time_point <std::chrono::steady_clock>& n)
{
	_tooQuick = false;
	_cyclesSinceLastAdjustment = 0;
	_requestsSinceLastTimeCheck = 0;

	// Preserve the scheduled phase after ordinary host-side latency, but discard
	// it after a delay of at least one complete adjustment period. This avoids
	// both cumulative clock drift and bursts intended to recover a long pause.
	const std::chrono::steady_clock::duration adjustmentDuration =
		_resumeClock - _adjustmentClock;

	_adjustmentClock =
		(n < (_resumeClock + adjustmentDuration))
			? _resumeClock
			: n;
	_resumeClock = _adjustmentClock;
}

// ---
std::string MCHEmul::Clock::asString () const
{
	std::string result;

	bool f = true;
	for (const auto& i : _ticks)
	{
		result += ((!f) ? "\n" : "") + i.asString ();

		f = false;
	}

	return (result);
}
