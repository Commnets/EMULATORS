#include <COMMODORE/1530Datasette.hpp>

// ---
COMMODORE::Datasette1530::Datasette1530 ()
	: COMMODORE::DatasettePeripheral (_ID, 
		{ { "Name", "Commodore 1530 (CN2)" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Commands", "1:FORWARD, 2:REWIND, 4:STOP, 8:PLAY, 24:PLAY + RECORD, 32:EJECT(and clear data)" } }),
	  _status (),
	  _clock (300), // 300 baudios, bits per second...
	  _readWritePhase (0),
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
		// REWIND...
		case 2:
		// STOPPED...
		case 4:
			{
				// When FORWARD...
				if (id == 1)
				{ 
					// ...move the pointer to the next element in the list...
					if (++_dataCounter > _data._data.size ())
						_dataCounter = 0; // ...or to the first one if there is none else to point to...
				}
				// When REWIND...
				if (id == 2)
				{
					// ...move the pointer to the previous element in the list...
					if (--_dataCounter > _data._data.size ()) // it is an unsigned short...
						_dataCounter = _data._data.size (); //...or to after the last one if there is none else to point to...
				}

				// Any other curcunstance nothing to do...
				// But always stopped...
				_status = Status::_STOPPED;

				// No keys is supossed to be pressed...
				// if the user wanted to move to the next / previous element ç
				// the same command would have to be executed...

				setNoKeyPressed (true);
			}

			break;

		// PLAY...
		case 8:
		// RECORD...
		case 24:
			{
				// The change in the status is only possible when stopped previously...
				if (_status == Status::_STOPPED)
				{
					_status = (id == 8) ? Status::_READING : Status::_SAVING;

					_elementCounter = 0;

					setNoKeyPressed (false);

					// If saving... 
					if (id == 24)
					{
						// ... but the counter is pointing at the end, a null data element is added...
						if (_dataCounter >= _data._data.size ())
							_data._data.push_back (MCHEmul::DataMemoryBlock ());

						// ...and that element is fully clearead...
						_data._data [_dataCounter].clear ();
					}
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
			if (_clock.tooQuick ())
				_clock.countCycles (0);
			else
			{
				if (_status == Status::_READING) 
					setRead (getNextDataBit ());
				else 
					storeNextDataBit (_valueToWrite);

				_clock.countCycles (1);
			}
		}
	}

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::Datasette1530::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (COMMODORE::DatasettePeripheral::getInfoStructure ());

	result.add ("DATACOUNTER", _dataCounter);

	return (result);
}

// ---
bool COMMODORE::Datasette1530::getNextDataBit ()
{
	return (_data._data [_dataCounter].bytes ()[_elementCounter++] == MCHEmul::UByte::_1);
}

// ---
void COMMODORE::Datasette1530::storeNextDataBit (bool s)
{
	_data._data [_dataCounter].addByte (s ? MCHEmul::UByte::_1 : MCHEmul::UByte::_0);

	_elementCounter++;
}
