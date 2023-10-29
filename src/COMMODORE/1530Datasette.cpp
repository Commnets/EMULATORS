#include <COMMODORE/1530Datasette.hpp>

// ---
COMMODORE::Datasette1530::Datasette1530 ()
	: COMMODORE::DatasettePeripheral (_ID, 
		{ { "Name", "Commodore 1530 (CN2)" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Commands", "1:FORWARD, 2:REWIND, 4:STOP, 8:PLAY, 24:PLAY + RECORD, 32:EJECT(and clear data)" } }),
	  _status (Status::_STOPPED),
	  _dataCounter (0), 
	  _elementCounter (0),
	  _firstCycleSimulation (false), _lastCPUCycles (0), _clockCycles (0)
{
	setClassName ("C2N1530");
}

// ---
bool COMMODORE::Datasette1530::initialize ()
{
	_status = Status::_STOPPED;

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

					// Starting to count...
					_clockCycles = 0;

					_firstCycleSimulation = true;

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

					// If the counter is pointing at the end, a null data element is added...
					if (_dataCounter >= _data._data.size ())
						_data._data.push_back (MCHEmul::DataMemoryBlock ());
					// ...and, any case, the element affected is fully clearead...
					_data._data [_dataCounter].clear ();

					// Starting to count...
					_clockCycles = 0;

					_firstCycleSimulation = true;

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
	if (motorOff ())
		return (true);

	if (_status != Status::_READING &&
		_status != Status::_SAVING)
		return (true);

	if (_firstCycleSimulation)
	{
		_firstCycleSimulation = false;

		_lastCPUCycles = cpu -> clockCycles () - cpu -> lastCPUClockCycles ();
	}

	for (unsigned int i = (cpu -> clockCycles  () - _lastCPUCycles); i > 0; i--)
	{
		if (++_clockCycles <= 3284 /** = 300 baudios when CPU speed = 985.248 c/s, = 310 baudios when CPU speed = 1.023.000 c/s. */)
			continue; // Not the time...
	
		// Start back...
		_clockCycles = 0; 
		
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
	}

	_lastCPUCycles = cpu -> clockCycles ();

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

