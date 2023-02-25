#include <COMMODORE/DatasettePeripherals.hpp>
#include <COMMODORE/FileReaders.hpp>

// ---
COMMODORE::DatasettePeripheral::DatasettePeripheral (int id, const MCHEmul::Attributes& attrs)
	: MCHEmul::IOPeripheral (id, attrs),
	  _valueRead (true),
	  _valueToWrite (false),
	  _motorOff (true),
	  _noKeyPressed (true),
	  _data (),
	  _motorChangeStatusRequest (false),
	  _readChangeValueRequest (false),
	  _writeChangeValueRequest (false),
	  _keysChangedStatusRequest (false)
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

	// Notice that the data is not initialized...
	// After being loaded it could be also usefull to keep it here...

	_motorChangeStatusRequest = false;
	_readChangeValueRequest = false;
	_writeChangeValueRequest = false;
	_keysChangedStatusRequest = false;

	return (true);
}

// ---
bool COMMODORE::DatasettePeripheral::connectData (MCHEmul::FileData* dt)
{ 
	if (dynamic_cast <COMMODORE::T64FileData*> (dt) == nullptr)
		return (false); // That type of info is not valid from the datasette...

	_data = dt -> asMemoryBlocks (); 
	
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
