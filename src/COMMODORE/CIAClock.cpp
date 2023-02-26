#include <COMMODORE/CIAClock.hpp>

// ---
void COMMODORE::CIAClock::initialize ()
{
	_interruptEnabled = false;

	_time = MCHEmul::Time ();

	// The implementation values...
	_lastClockCycles = 0;
	_hours = _minutes = _seconds = _tenthsSecond = 0x00;
	_minutesL = _secondsL = _tenthsSecondL = 0x00;
	_timeLatched = false;

	_stopped = false;

	_reachesAlarm = false;
	_interruptRequested = false;
}

// ---
void COMMODORE::CIAClock::simulate (MCHEmul::CPU* cpu)
{
	assert (cpu != nullptr); 

	if (_stopped)
		return;

	_reachesAlarm = false;

	actualizeTime ();

	// The alarm is controlled versus the values latched
	// because the actualization of the official ones might be in progress...
	if (_hours == _alarmHours && _minutesL == _alarmMinutes && _secondsL == _alarmSeconds &&
		_tenthsSecondL >= _alarmTenthsSecond)
	{
		if (_interruptEnabled)
			cpu -> interrupt (_interruptId) -> setActive (_interruptRequested = true);
		
		_reachesAlarm = true;
	}

	_lastClockCycles = cpu -> clockCycles ();
}

// ---
MCHEmul::InfoStructure COMMODORE::CIAClock::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::InfoClass::getInfoStructure ();

	result.add ("Interrupt",		_interruptEnabled);
	result.add ("HOURS",			_hours);
	result.add ("MINUTES",			_minutes);
	result.add ("SECONDS",			_seconds);
	result.add ("TENTHSECONDS",		_tenthsSecond);
	result.add ("AHOURS",			_alarmHours);
	result.add ("AMINUTES",			_alarmMinutes);
	result.add ("ASECONDS",			_alarmSeconds);
	result.add ("ATENTHSECONDS",	_alarmTenthsSecond);

	return (result);
}

// ---
void COMMODORE::CIAClock::actualizeTime () 
{
	_time += MCHEmul::_TENTHSSECONDPAST; // Actualize the timer...
	if (_time.time_since_epoch ().count () >= 864000 /** Max tenth of second in a day. */)
		_time = MCHEmul::Time ();

	// Now it is time to convert it into days, hours,...
	auto tds = _time.time_since_epoch ().count (); // In tenths of seconds...
	_hours = (unsigned char) (tds / 36000); tds -= (unsigned long) _hours * 36000; // tenths of second per hour = 60 * 60 * 10
	_minutes = (unsigned char) (tds / 600); tds -= (unsigned long) _minutes * 600; // tenths of second per minute = 60 * 10
	_seconds = (unsigned char) (tds / 10); tds -= (unsigned long) _seconds * 10; // tenths of second per second = 10;
	_tenthsSecond = (unsigned char) tds; // What remains are the tenths of second...

	if (!_timeLatched)
		{ _minutesL = _minutes; _secondsL = _seconds; _tenthsSecondL = _tenthsSecond; }
}
