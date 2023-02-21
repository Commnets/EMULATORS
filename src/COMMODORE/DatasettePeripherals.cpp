#include <COMMODORE/DatasettePeripherals.hpp>

// ---
COMMODORE::DatasettePeripheral::DatasettePeripheral (int id, const MCHEmul::Attributes& attrs)
	: MCHEmul::IOPeripheral (id, attrs),
	  _valueRead (true),
	  _valueToWrite (false),
	  _motorOff (true),
	  _noKeyPressed (true)
{
	setClassName ("Datasette");
}

// ---
bool COMMODORE::DatasettePeripheral::initialize ()
{
	_valueRead = true;
	_valueToWrite = false;
	_motorOff = true;
	_noKeyPressed = true;

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::DatasettePeripheral::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::IOPeripheral::getInfoStructure ();

	result.add ("KEYS", !_noKeyPressed);
	result.add ("MOTOR", !_motorOff);

	return (result);
}
