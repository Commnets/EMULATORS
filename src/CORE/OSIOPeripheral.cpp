#include <CORE/OSIOPeripheral.hpp>
#include <CORE/Computer.hpp>
#include <CORE/OSIO.hpp>
#include <CORE/LogChannel.hpp>

// ---
MCHEmul::InputOSSystemPeripheral::InputOSSystemPeripheral (int id,
		unsigned int wt, MCHEmul::Computer* c, const MCHEmul::Attributes& attrs)
	: MCHEmul::IOPeripheral (id, attrs),
	  _events { }, // Empty at the beginning...
	  _originalWaitTime (wt), _waitTime (wt), // Number of 1/25 s-length. 25 will mean 1 second...
	  _OSIO (nullptr), // This is set up later...
	  // Implementation variables...
	  _running (false), 
	  _eventReady (false), _eventToInject { }, _eventNumber (0), _waitTimeCounter (0)
{
	assert (c != nullptr);

	_OSIO = c -> inputOSSystem ();

	// It can be nullptr...
	assert (_OSIO != nullptr);
}

// ---
bool MCHEmul::InputOSSystemPeripheral::initialize ()
{
	_running = false; 

	_eventReady = false;
	
	_eventToInject = { };
	_eventNumber = 0; // The counter at the beginning...

	_waitTimeCounter = 0;

	return (true);
}

// ---
bool MCHEmul::InputOSSystemPeripheral::executeCommand (int id, const MCHEmul::Strings& prms)
{
	bool result = true;

	// Only the allowed possibilities...
	// Most of them are possibe only when the data is loaded
	switch (id)
	{
		// FORWARD...
		case _FOWARD:
			{
				if (_eventNumber < _events.size ())
					_eventNumber++;
				else
				{
					_LOG ("No more events beyond the current position.");

					result = false;
				}
			}

			break;

		// REWIND...
		case _REWIND:
			{
				if (_eventNumber > 0)
					_eventNumber--;
				else
				{
					_LOG ("No more events before the current position.");

					result = false;
				}
			}

			break;

		// STOPPED...
		case _STOP:
			{
				_running = false;
			}

			break;

		// PLAY...
		case _PLAY:
			{
				_running = true;
			}

			break;

		// BEGINNING...
		case _BEGINNING:
			{
				_running = false;

				_eventNumber= 0;
			}

		// EJECT...
		case _EJECT:
			{
				_events = { };

				_running = false;

				_eventNumber = 0;
			}

			break;

		// Command not valid, but the status doesn't change...
		default:
			{
				result = false; 
			}

			break;
	}

	return (result);
}

// ---
bool MCHEmul::InputOSSystemPeripheral::simulate (MCHEmul::CPU* cpu)
{
	if (!_running)
		return (true);

	// Only when the peripheral is running...
	if (++_waitTimeCounter >= _waitTime)
	{
		_waitTimeCounter = 0;

		// When there is more events to read...
		// The peripheral stops and the pointer goes back to the beginning...
		if (_eventNumber < _events.size ())
		{
			_eventReady = true;

			_eventToInject = _events [_eventNumber++];

			// The waiting time can change if there is a specific event that requieres...
			std::map <size_t, unsigned int>::const_iterator i =
				_specificWaitingTimes.find (_eventNumber);
			_waitTime = (i == _specificWaitingTimes.end () 
				? _originalWaitTime : (*i).second);
		}
		else
		{
			_running = false;

			_eventNumber = 0;
		}
	}

	return (true);
}

// ---
MCHEmul::InfoStructure MCHEmul::InputOSSystemPeripheral::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::IOPeripheral::getInfoStructure ());

	result.add ("RUNNINF",	_running);
	result.add ("POSITION", _eventNumber);

	return (result);
}

// ---
void MCHEmul::InputOSSystemPeripheral::loadEvents (const std::vector <SDL_Event>& evnts)
{
	_events = evnts;

	_running = false;

	_eventNumber = 0;
}
