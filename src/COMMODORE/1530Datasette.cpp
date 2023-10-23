#include <COMMODORE/1530Datasette.hpp>

// ---
COMMODORE::Datasette1530::Datasette1530 (unsigned int cps)
	: COMMODORE::DatasettePeripheral (_ID, 
		{ { "Name", "Commodore 1530 (CN2)" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Commands", "1:FORWARD, 2:REWIND, 4:STOP, 8:PLAY, 24:PLAY + RECORD, 32:EJECT(and clear data)" } }),
	  _cyclesPerSecond (cps),
	  _status (Status::_STOPPED),
	  _clock ((unsigned int) ((float) _cyclesPerSecond / 3284.0f) /** 300 bauds in PAL, 311 in NTSC */),
	  _dataCounter (0), 
	  _elementCounter (0)
{
	setClassName ("C2N1530");
}

// ---
bool COMMODORE::Datasette1530::initialize ()
{
	_status = Status::_STOPPED;

	_clock.start ();

	return (COMMODORE::DatasettePeripheral::initialize ());
}

// ---
bool COMMODORE::Datasette1530::executeCommand (int id, const MCHEmul::Strings& prms)
{
	bool result = true;

	// Only the allowed possibilities...
	switch (id)
	{
		// FORWARD...
		case 1:
			{
				// ...move the pointer to the next element in the list...
				if (++_dataCounter > _data._data.size ())
					_dataCounter = 0; // ...or to the first one if there is none else to point to...

				// Always stopped...
				_status = Status::_STOPPED;

				// No keys is supossed to be pressed...
				// if the user wanted to move to the next / previous element
				// the same command would have to be executed...
				setNoKeyPressed (true);
			}

			break;

		// REWIND...
		case 2:
			{
				// ...move the pointer to the previous element in the list...
				if (--_dataCounter > _data._data.size ()) // it is an unsigned short...
					_dataCounter = _data._data.size (); //...or to after the last one if there is none else to point to...

				_status = Status::_STOPPED;

				setNoKeyPressed (true);
			}

			break;

		// STOPPED...
		case 4:
			{
				if (_status == Status::_SAVING)
					_dataCounter++;

				_status = Status::_STOPPED;

				setNoKeyPressed (true);
			}

			break;

		// PLAY...
		case 8:
			{
				if (_status == Status::_STOPPED &&
					_dataCounter < _data._data.size () /** Only possible when the data is not at the end. */)
				{
					_elementCounter = 0;

					_status = Status::_READING;

					setNoKeyPressed (false);
				}
			}

			break;

		// RECORD...
		case 24:
			{
				// The change in the status is only possible when stopped previously...
				if (_status == Status::_STOPPED)
				{
					_elementCounter = 0;

					_status = Status::_SAVING;

					_clock = MCHEmul::Clock (300 /** bauds. */);

					// If the counter is pointing at the end, a null data element is added...
					if (_dataCounter >= _data._data.size ())
						_data._data.push_back (MCHEmul::DataMemoryBlock ());
					// ...and, any case, the element affected is fully clearead...
					_data._data [_dataCounter].clear ();

					setNoKeyPressed (false);
				}
			}

			break;

		// EJECT...
		case 32:
			{
				// Data is forgetted...
				clearData ();

				// ...always stopped...
				_status = Status::_STOPPED;

				// and starts back from the beginning...
				_dataCounter = 0;

				_elementCounter = 0;

				setNoKeyPressed (true);
			}

			break;

		// Command not valid, but the status doesn't change...
		default:
			result = false; 
			break;
	}

	return (result);
}

// ---
bool COMMODORE::Datasette1530::simulate (MCHEmul::CPU* cpu)
{
	if (!motorOff ())
	{
		if (_status == Status::_READING || 
			_status == Status::_SAVING) // It shouldn't happen in other circunstance, but just to be sure...
		{
			// Only works at the speed of the tap...
			if (_clock.tooQuick ())
				_clock.countCycles (0);
			else
			{
				// When reading is allowed...
				// makes only sense when there is still data to read in the loaded file...
				if (_status == Status::_READING)
				{ 
					bool r = false;
					if (_dataCounter < _data._data.size ())
					{
						r = true;
						if (_elementCounter >= _data._data [_dataCounter].bytes ().size ())
						{
							_elementCounter = 0;

							if (++_dataCounter >= _data._data.size ())
								r = false;
						}
					}

					if (r)
						setRead (getNextDataBit ());
				}
				else 
					storeNextDataBit (_valueToWrite);

				_clock.countCycles (1);
			}
		}
	}

	return (true);
}

// ---
bool COMMODORE::Datasette1530::connectData (MCHEmul::FileData* dt)
{
	bool result = COMMODORE::DatasettePeripheral::connectData (dt);

	if (result)
		_dataCounter = 0; // at the beginning...

	return (result);
}

// ---
MCHEmul::InfoStructure COMMODORE::Datasette1530::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (COMMODORE::DatasettePeripheral::getInfoStructure ());

	result.add ("DATACOUNTER", _dataCounter);

	return (result);
}

