#include <C64/CIATimer.hpp>

// ---
void C64::CIATimer::initialize ()
{
	_runMode = RunMode::_RESTART;
	_countMode = CountMode::_PROCESSORCYCLES;

	_enabled = false;
	_IRQEnabled = false;

	_initialValue = 0x0000;
	_time = MCHEmul::Time ();

	// The implementation values...
	_currentValue = 0x0000;
	_lastClockCycles = 0;

	_reaches0 = false;
	_IRQRequested = false;
}

// ---
void C64::CIATimer::simulate (MCHEmul::CPU* cpu, C64::CIATimer* t)
{
	assert (cpu != nullptr);

	if (!_enabled)
		return;

	if (countDown (cpu, t))
	{
		if (_IRQEnabled)
			cpu -> interrupt (_interruptId) -> setActive (_IRQRequested = true);
		
		switch (_runMode)
		{
			case C64::CIATimer::RunMode::_RESTART:
				_currentValue = _initialValue;
				break;

			case C64::CIATimer::RunMode::_ONETIME:
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
MCHEmul::InfoStructure C64::CIATimer::getInfoStructure () const
{
	MCHEmul::InfoStructure result;

	result.add ("RUN",			(unsigned int)_runMode);
	result.add ("COUNT",		(unsigned int) _countMode);
	result.add ("STATUS",		_enabled);
	result.add ("IRQ",			_IRQEnabled);
	result.add ("VALUE",		_currentValue);
	result.add ("INITIALVALUE", _initialValue);

	return (result);
}

// ---
bool C64::CIATimer::countDown (MCHEmul::CPU* cpu, C64::CIATimer* t)
{
	_reaches0 = false;

	bool result = false;
	switch (_countMode)
	{
		case C64::CIATimer::CountMode::_PROCESSORCYCLES:
			{
				unsigned int c = cpu -> clockCycles () - _lastClockCycles;
				result = ((_currentValue -= (c > (unsigned int) _currentValue) ? _currentValue : (unsigned short) c) == 0x0000);
			}

			break;

		case C64::CIATimer::CountMode::_SIGNALSONCNTLINE:
			// TODO: Linked to the serial port...
			break;

		case C64::CIATimer::CountMode::_TIMERCOUNTSDOWNTO0:
			if (t != nullptr && t -> reaches0 ())
				result = (--_currentValue == 0x00);
			break;

		case C64::CIATimer::CountMode::_0ONCNTPULSES:
			// TODO: Linked to the serial port...
			break;

		default:
			break;
	}

	return (result);
}
