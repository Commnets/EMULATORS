#include <COMMODORE/1530Datasette.hpp>

// ---
COMMODORE::Datasette1530::Datasette1530 ()
	: COMMODORE::DatasettePeripheral (_ID, 
		{ { "Name", "Commodore 1530 (CN2)" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Commands", "4:STOP, 8:PLAY, 24:RECORD" } }),
	  _status (),
	  _clock (400) // 400 baudios, bits per second...
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
			{
				// TODO

				std::cout << valueToWrite ();
			}
			else
			if (_status == Status::_READING)
			{
				// TODO
			}

			_clock.countCycles (1);
		}
	}

	return (true);
}
