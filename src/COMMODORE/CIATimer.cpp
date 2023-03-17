#include <COMMODORE/CIATimer.hpp>

// ---
void COMMODORE::CIATimer::initialize ()
{
	_runMode = RunMode::_RESTART;
	_countMode = CountMode::_PROCESSORCYCLES;

	_affectPortDataB = false;
	_pulseAtPortDataB = false;

	_enabled = false;
	_interruptEnabled = false;

	_initialValue = 0x0000;
	_time = MCHEmul::Time ();

	// The implementation values...
	_firstCycle = false;

	_currentValue = 0x0000;
	_lastClockCycles = 0;

	_reaches0 = false;
	_interruptRequested = false;
}

// ---
void COMMODORE::CIATimer::simulate (MCHEmul::CPU* cpu, COMMODORE::CIATimer* t)
{
	assert (cpu != nullptr);

	if (!_enabled)
		return;

	if (_firstCycle)
	{
		_firstCycle = false;

		_lastClockCycles = cpu -> clockCycles (); // Just to set up in the first cycle...
	}

	if (countDown (cpu, t))
	{
		if (_interruptEnabled)
			cpu -> interrupt (_interruptId) -> setActive (_interruptRequested = true);
		
		switch (_runMode)
		{
			case COMMODORE::CIATimer::RunMode::_RESTART:
				_currentValue = _initialValue;
				break;

			case COMMODORE::CIATimer::RunMode::_ONETIME:
				_enabled = false;
				break;

			default:
				break;
		}

		_reaches0 = true;
	}
	else
		_reaches0 = false;

	_lastClockCycles = cpu -> clockCycles ();
}

// ---
MCHEmul::InfoStructure COMMODORE::CIATimer::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::InfoClass::getInfoStructure ();

	result.add ("RUN",			(unsigned int)_runMode);
	result.add ("COUNT",		(unsigned int) _countMode);
	result.add ("STATUS",		_enabled);
	result.add ("IRQ",			_interruptEnabled);
	result.add ("VALUE",		_currentValue);
	result.add ("INITIALVALUE", _initialValue);

	return (result);
}

// ---
bool COMMODORE::CIATimer::countDown (MCHEmul::CPU* cpu, COMMODORE::CIATimer* t)
{
	_reaches0 = false;

	bool result = false;
	switch (_countMode)
	{
		case COMMODORE::CIATimer::CountMode::_PROCESSORCYCLES:
			{
				unsigned int c = cpu -> clockCycles () - _lastClockCycles;
				result = ((_currentValue -= (c > (unsigned int) _currentValue) ? _currentValue : (unsigned short) c) == 0x0000);
			}

			break;

		case COMMODORE::CIATimer::CountMode::_SIGNALSONCNTLINE:
			// TODO

			break;

		case COMMODORE::CIATimer::CountMode::_TIMERCOUNTSDOWNTO0:
			if (t != nullptr && t -> reaches0 ())
				result = (--_currentValue == 0x00);
			break;

		case COMMODORE::CIATimer::CountMode::_0ONCNTPULSES:
			// TODO

			break;

		default:
			break;
	}

	return (result);
}
