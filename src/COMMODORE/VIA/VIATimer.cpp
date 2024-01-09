#include <COMMODORE/VIA/VIATimer.hpp>

// ---
void COMMODORE::VIATimer::initialize ()
{
	_runMode = RunMode::_ONESHOOT;

	_countMode = CountMode::_PROCESSORCYCLES;

	_interruptEnabled = false;

	_initialValue = 0x0000;

	// The implementation values...
	_counting = false;
	_currentValue = 0x0000;
	_lastClockCycles = 0;
	_reaches0 = _reaches0LSB = _firstTimeReaches0 = _reachesHalf = _alreadyReachedHalf = false;
	_PB7Situation = false;
	_PB6Situation = false;
	_interruptRequested = false;
}

// ---
bool COMMODORE::VIATimer::simulate (MCHEmul::CPU* cpu)
{
	assert (cpu != nullptr);

	if (_lastClockCycles == 0)
	{
		_lastClockCycles = cpu -> clockCycles (); // Just to set up in the first cycle...

		return (true);
	}

	for (unsigned int i = cpu -> lastCPUClockCycles () - _lastClockCycles; i > 0; i--)
	{
		if (_counting && 
			countDown (cpu)) // Counting one at a time...
		{
			switch (_runMode)
			{
				case COMMODORE::VIATimer::RunMode::_ONESHOOT:
				case COMMODORE::VIATimer::RunMode::_ONESHOOTSIGNAL:
					{
						if (!_firstTimeReaches0)
						{ 
							_firstTimeReaches0 = true;

							_interruptRequested = true;

							if (_runMode == COMMODORE::VIATimer::RunMode::_ONESHOOTSIGNAL)
								_PB7Situation = true;
						}
						else
							_PB7Situation = false;
					}

					break;

				case COMMODORE::VIATimer::RunMode::_CONTINUOUS:
				case COMMODORE::VIATimer::RunMode::_CONTINUOUSSIGNAL:
					{
						_interruptRequested = true;

						// Starts back...
						_currentValue = _initialValue;

						_alreadyReachedHalf = false;

						if (_runMode == COMMODORE::VIATimer::RunMode::_CONTINUOUSSIGNAL)
							_PB7Situation = !_PB7Situation;
					}

					break;

				default:
					// It shouldn't be here...
					assert (false);
					break;
			}
		}
	}

	_lastClockCycles = cpu -> clockCycles ();

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::VIATimer::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::InfoClass::getInfoStructure ();

	result.add ("RUN",			(unsigned int)_runMode);
	result.add ("COUNT",		(unsigned int) _countMode);
	result.add ("IRQ",			_interruptEnabled);
	result.add ("VALUE",		_currentValue);
	result.add ("INITIALVALUE", _initialValue);

	return (result);
}

// ---
bool COMMODORE::VIATimer::countDown (MCHEmul::CPU* cpu)
{
	switch (_countMode)
	{
		case COMMODORE::VIATimer::CountMode::_PROCESSORCYCLES:
			{
				--_currentValue;
			}

			break;

		case COMMODORE::VIATimer::CountMode::_PULSERECEIVED:
			{
				if (_PB6Situation) // This is an OBool...becomes false always after this instruction...
					--_currentValue;
			}

			break;

		default:
			break;
	}

	// When the current value reaches 0....
	_reaches0 = (_currentValue == 0x0000);
	// ...and this one when the LSB part reaches 0...
	_reaches0LSB = ((_currentValue & 0x00ff) == 0x0000);
	// This variable is set first time the value is below the half...
	if ((_reachesHalf = ((_currentValue >> 1) < _initialValue) && !_alreadyReachedHalf)) 
		_alreadyReachedHalf = true; // Latches, but not follow the value...

	return (_reaches0);
}
