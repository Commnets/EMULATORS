#include <COMMODORE/DatasettePeripherals.hpp>
#include <COMMODORE/FileIO.hpp>

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

	// A very basic datasette is added...
	_data._name = "COMMTYNETS";
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
	if (dynamic_cast <COMMODORE::T64FileData*> (dt) == nullptr &&
		dynamic_cast <COMMODORE::TAPFileData*> (dt) == nullptr &&
		dynamic_cast <COMMODORE::RawFileData*> (dt) == nullptr)
		return (false); // That type of info is not valid from the datasette...

	_data = std::move (dt -> asMemoryBlocks ());
	
	return (true); 
}

// ---
MCHEmul::FileData* COMMODORE::DatasettePeripheral::retrieveData () const
{
	// The format understood by this retrive methos is just raw...
	MCHEmul::FileData* result = new COMMODORE::RawFileData; 
	COMMODORE::RawFileData* tap = dynamic_cast <COMMODORE::RawFileData*> (result);

	// The signature is not changed... 

	unsigned int dS = 0;
	std::vector <MCHEmul::UByte> _bytes;
	for (const auto& i : _data._data)
	{
		dS += (unsigned int) i.size ();

		_bytes.insert (i.bytes ().end (), i.bytes ().begin (), i.bytes ().end ());
	}

	tap -> _dataSize = dS;
	tap -> _bytes = std::move (_bytes); // It is not longer used...

	return (result);
}

// ---
MCHEmul::InfoStructure COMMODORE::DatasettePeripheral::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::IOPeripheral::getInfoStructure ());

	result.add ("KEYS",		!_noKeyPressed);
	result.add ("MOTOR",	!_motorOff);
	result.add ("DATA",		_data._data.empty () ? std::string ("no data") : _data._name);
	result.add ("DATASIZE", _data._data.size ());

	return (result);
}
