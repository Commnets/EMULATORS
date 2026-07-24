#include <COMMODORE/VIA/VIATimer.hpp>
#include <COMMODORE/VIA/VIAPort.hpp>

// ---
COMMODORE::VIATimer::VIATimer (int id)
	: MCHEmul::InfoClass ("VIATimer"),
	  _id (id),
	  _interruptRequested (false),
	  _P (nullptr)
{ 
	initialize (); /** To initialize the rest of the values. */ 
}

// ---
void COMMODORE::VIATimer::reset ()
{ 
	_currentValue = _initialValue; 
		
	_firstTimeReaches0 = false;
		
	_counting = false;
}

// ---
void COMMODORE::VIATimer::initialize ()
{
	// This mode is common to both timers...
	_runMode = RunMode::_ONESHOOT;
	_countMode = CountMode::_PROCESSORCYCLES;

	_initialValue = 0x0000;

	_interruptEnabled = false;

	// The implementation values...
	_counting = false;
	_currentValue = 0x0000;
	_reaches0 = _reaches0LSB = _firstTimeReaches0 = false;
	_interruptRequested = false;
}

// ---
COMMODORE::VIATimer::CycleResult COMMODORE::VIATimer::simulate (MCHEmul::CPU* cpu)
{
	assert (cpu != nullptr);

	CycleResult result;

	if (_counting && 
		countDown (cpu)) // Counting one at a time...
	{
		result._lowByteReached0 = _reaches0LSB;

		if (!_reaches0)
			return (result);

		result._timeout = true;

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
						{ 
							// just in case...
							// Timer 2 has nothing to do with this mode and when defined,
							// the timer 2 doesn't take a look of any timer, so this is just a protection...
							assert (_P != nullptr);

							_P -> setP7 (true);
						}
					}
				}

				break;

			case COMMODORE::VIATimer::RunMode::_CONTINUOUS:
			case COMMODORE::VIATimer::RunMode::_CONTINUOUSSIGNAL:
				{
					_interruptRequested = true;

					// Starts back...
					_currentValue = _initialValue;


					if (_runMode == COMMODORE::VIATimer::RunMode::_CONTINUOUSSIGNAL)
					{
						assert (_P != nullptr);

						_P -> changeP7 (); // Train of pulses!
					}
				}

				break;

			default:
				{ 
					// It shouldn't be here
					_LOG ("Via Timer Run mode not supported:" + 
						std::to_string ((int) _runMode));
					assert (false); // Just when compiling under DEBUG mode...
				}

				break;
		}
	}

	return (result);
}

// ---
MCHEmul::InfoStructure COMMODORE::VIATimer::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::InfoClass::getInfoStructure ();

	result.add ("RUN",			(unsigned int) _runMode);
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

		default:
			break;
	}

	// When the current value reaches 0....
	_reaches0 = (_currentValue == 0x0000);
	// ...and this one when the LSB part reaches 0...
	_reaches0LSB = ((_currentValue & 0x00ff) == 0x0000);
	return (true);
}

// ---
void COMMODORE::VIATimer1::setCountAndRunMode 
	(COMMODORE::VIATimer::CountMode cM, COMMODORE::VIATimer::RunMode rM)
{
	if (cM != COMMODORE::VIATimer::CountMode::_PROCESSORCYCLES)
	{
		_LOG ("VIATimer1: Only _PROCESSORCYCLES counting mode is allowed.");

		assert (false); // Just when compiling under DEBUG mode...
	}

	// In RELEASE version it will work but the systme will work bad...
	COMMODORE::VIATimer::setCountAndRunMode (cM, rM);
}

// ---
void COMMODORE::VIATimer1::reset ()
{ 
	COMMODORE::VIATimer::reset (); // Reset the common values...

	// Loading T1C-H drives PB7 low until the first time-out.
	// if the mode is the right one...
	if (_runMode == RunMode::_ONESHOOTSIGNAL ||
		_runMode == RunMode::_CONTINUOUSSIGNAL)
	{ 
		assert (_P != nullptr);

		_P -> setP7 (false);
	}
}

// ---
void COMMODORE::VIATimer1::initialize ()
{
	COMMODORE::VIATimer::initialize ();

	assert (_P != nullptr);

	_P -> setP7 (true);
}

// ---
void COMMODORE::VIATimer2::setCountAndRunMode 
	(COMMODORE::VIATimer::CountMode cM, COMMODORE::VIATimer::RunMode rM)
{
	if (rM != COMMODORE::VIATimer::RunMode::_ONESHOOT)
	{
		_LOG ("VIATimer2: Only _ONESHOOT running mode is allowed.");

		assert (false); // Just when compiling under DEBUG mode...
	}

	// In RELEASE version it will work but the systme will work bad...
	COMMODORE::VIATimer::setCountAndRunMode (cM, rM);
}

// ---
bool COMMODORE::VIATimer2::countDown (MCHEmul::CPU* cpu)
{
	bool count = false;

	switch (_countMode)
	{
		case COMMODORE::VIATimer::CountMode::_PULSERECEIVED:
			{
				assert (_P != nullptr);

				// The negative transition of the pulse in P6...
				// ...is what is counted...
				if (_P -> peekP6negativeEdge ())
				{
					--_currentValue;
					count = true;
				}
			}

			break;

		case COMMODORE::VIATimer::CountMode::_PROCESSORCYCLES:
			count = COMMODORE::VIATimer::countDown (cpu);
			break;

		default:
			break;
	}

	if (_countMode == COMMODORE::VIATimer::CountMode::_PULSERECEIVED && count)
	{
		_reaches0 = (_currentValue == 0x0000);
		_reaches0LSB = ((_currentValue & 0x00ff) == 0x0000);
	}
	else if (!count)
		_reaches0 = _reaches0LSB = false;

	return (count);
}
