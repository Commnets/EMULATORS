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
void COMMODORE::VIATimer::initialize ()
{
	_runMode = RunMode::_ONESHOOT;

	_countMode = CountMode::_PROCESSORCYCLES;

	_initialValue = 0x0000;

	_interruptEnabled = false;

	// The implementation values...
	_counting = false;
	_currentValue = 0x0000;
	_reaches0 = _reaches0LSB = _firstTimeReaches0 = _reachesHalf = _alreadyReachedHalf = false;
	_interruptRequested = false;
}

// ---
void COMMODORE::VIATimer::reset ()
{ 
	_currentValue = _initialValue; 
		
	_alreadyReachedHalf = false; 
							  
	_firstTimeReaches0 = false;
		
	_counting = false;

	// When reset the PB7 is push down 
	// if the mode is the right one...
	if (_runMode == RunMode::_ONESHOOTSIGNAL ||
		_runMode == RunMode::_CONTINUOUSSIGNAL)
	{ 
		assert (_P != nullptr);

		_P -> setP7 (false);
	}
}

// ---
bool COMMODORE::VIATimer::simulate (MCHEmul::CPU* cpu)
{
	assert (cpu != nullptr);

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
						{ 
							// just in case...
							// Timer 2 has nothing to do with this mode and when defined,
							// the timer 2 doesn't take a look of any timer, so this is just a protection...
							assert (_P != nullptr);

							_P -> setP7 (true); // Pulse generated!
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

					_alreadyReachedHalf = false;

					if (_runMode == COMMODORE::VIATimer::RunMode::_CONTINUOUSSIGNAL)
					{
						assert (_P != nullptr);

						_P -> changeP7 (); // Train of pulses!
					}
				}

				break;

			default:
				// It shouldn't be here...
				assert (false);
				break;
		}
	}

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
				assert (_P != nullptr);

				if (_P -> peekP6Pulse ())
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
