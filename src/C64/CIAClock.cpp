#include <C64/CIAClock.hpp>
#include <F6500/NMIInterrupt.hpp>

// ---
void C64::CIAClock::initialize ()
{
	_IRQEnabled = false;

	_time = C64::Time ();

	// The implementation values...
	_lastClockCycles = 0;
	_hours = _minutes = _seconds = _tenthsSecond = 0x00;
	_minutesL = _secondsL = _tenthsSecondL = 0x00;
	_timeLatched = false;

	_stopped = false;

	_reachesAlarm = false;
	_IRQRequested = false;
}

// ---
void C64::CIAClock::simulate (MCHEmul::CPU* cpu)
{
	assert (cpu != nullptr); 

	if (_stopped)
		return;

	_reachesAlarm = false;

	actualizeTime ();

	// The alamr is controlled versus the values latched
	// because the actualization of the official ones might be in progress...
	if (_hours == _alarmHours && _minutesL == _alarmMinutes && _secondsL == _alarmSeconds &&
		_tenthsSecondL >= _alarmTenthsSecond)
	{
		if (_IRQEnabled)
			cpu -> interrupt (F6500::NMIInterrupt::_ID) -> setActive (_IRQRequested = true);
		
		_reachesAlarm = true;
	}

	_lastClockCycles = cpu -> clockCycles ();
}

// ---
std::ostream& C64::operator << (std::ostream& o, const C64::CIAClock& cc)
{
	o << "IRQ:" << (cc._IRQEnabled ? "On" : "Off") << std::endl;
	o << "Value:" << cc._hours << "," << cc._minutes << "," << cc._seconds << "," << cc._tenthsSecond 
	  << "(" << cc._alarmHours << "," << cc._alarmMinutes << "," << cc._alarmSeconds << "," << cc._alarmTenthsSecond << ")" << std::endl;

	return (o);
}

// ---
void C64::CIAClock::actualizeTime () 
{
	_time += C64::_TENTHSSECONDPAST; // Actualize the timer...
	if (_time.time_since_epoch ().count () >= 864000 /** Max tenth of second in a day. */)
		_time = C64::Time ();

	// Now it is time to convert it into days, hours,...
	auto tds = _time.time_since_epoch ().count (); // In tenths of seconds...
	_hours = (unsigned char) (tds / 36000); tds -= (unsigned long) _hours * 36000; // tenths of second per hour = 60 * 60 * 10
	_minutes = (unsigned char) (tds / 600); tds -= (unsigned long) _minutes * 600; // tenths of second per minute = 60 * 10
	_seconds = (unsigned char) (tds / 10); tds -= (unsigned long) _seconds * 10; // tenths of second per second = 10;
	_tenthsSecond = (unsigned char) tds; // What remains are the tenths of second...

	if (!_timeLatched)
		{ _minutesL = _minutes; _secondsL = _seconds; _tenthsSecondL = _tenthsSecond; }
}
