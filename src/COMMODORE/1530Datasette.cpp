#include <COMMODORE/1530Datasette.hpp>

// ---
COMMODORE::Datasette1530::Datasette1530 ()
	: COMMODORE::DatasettePeripheral (_ID, 
		{ { "Name", "Commodore 1530 (CN2)" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Commands", "4:STOP, 8:PLAY, 24:RECORD, 32:EJECT(clear data)" } }),
	  _status (),
	  _clock (400), // 400 baudios, bits per second...
	  _readWritePhase (0),
	  _dataCounter (0), _bitCounter (0), _byteCounter (0)
{
	// Nothing else to do...
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
	setNoKeyPressed ((id == _KEYSTOP) ? true : false);

	// Only keys play and record has effect...
	_status = ((id & _KEYPLAY) == _KEYPLAY) 
		? (((id & _KEYRECORD) == _KEYRECORD) ? Status::_SAVING : Status::_READING)
		: Status::_STOPPED;

	if (_status != Status::_STOPPED)
	{
		_readWritePhase = 0;

		_dataCounter = 0;
		_bitCounter = 0;
		_byteCounter = 0;
	}

	if (id & _KEYEJECT)
		clearData (); // No longer data inside...

	return (true);
}

// ---
bool COMMODORE::Datasette1530::simulate (MCHEmul::CPU* cpu)
{
	if (!motorOff ())
	{
		if (_clock.tooQuick ())
			_clock.countCycles (0);
		else
		{
			if (_status == Status::_SAVING)
				storeNextDataBit (_valueToWrite); 
			else
			if (_status == Status::_READING)
				setRead (getNextDataBit ());

			_clock.countCycles (1);
		}
	}

	return (true);
}

// ---
bool COMMODORE::Datasette1530::getNextDataBit ()
{
	// TODO

	return (true);
}

// ---
void COMMODORE::Datasette1530::storeNextDataBit (bool s)
{
	// TODO
}
