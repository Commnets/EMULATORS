#include <COMMODORE/CIAClock.hpp>

// ---
void COMMODORE::CIAClock::initialize ()
{
	_interruptEnabled = false;

	_time = MCHEmul::ClockDurationType (0);

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
		_interruptRequested = true;
		
		_reachesAlarm = true;
	}

	_lastClockCycles = cpu -> clockCycles ();
}

// ---
MCHEmul::InfoStructure COMMODORE::CIAClock::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

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
	_time = MCHEmul::_NOW - MCHEmul::_STARTINGTIME; // Actualize the time spend...
	if (_time.count () >= 86400000 /** Max milliseconds in a day = 24 * 60 * 60 * 1000 */)
		_time -= MCHEmul::ClockDurationType (8640000);

	// Now it is time to convert it into days, hours,...
	long long tds = _time.count ();
	// milliseconds per hour = 60 * 60 * 1000
	_hours = (unsigned char) (tds / 3600000);
	tds -= (long long) _hours * 3600000; 
	// milliseconds per minute = 60 * 1000
	_minutes = (unsigned char) (tds / 60000);
	tds -= (long long) _minutes * 60000; 
	// milliseconds per second = 1000;
	_seconds = (unsigned char) (tds / 1000);
	tds -= (long long) _seconds * 1000;	
	// What remains are milliseconds converted into tenths of second...
	_tenthsSecond = (unsigned char) (tds / 100);

	if (!_timeLatched)
		{ _minutesL = _minutes; _secondsL = _seconds; _tenthsSecondL = _tenthsSecond; }
}
