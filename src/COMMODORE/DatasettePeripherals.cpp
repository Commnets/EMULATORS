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

	clearData ();

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

	// Later, when saving if any, the size in the name will be limited...
	tap -> _signature = _data._name;
	// ..the number of blocks...
	tap -> _dataBlocks = (unsigned int) _data._data.size ();
	// ...and the data...
	for (const auto& i : _data._data)
	{
		COMMODORE::RawFileData::Block dB;
		dB._name = "";
		dB._dataSize = (unsigned int) i.size ();
		dB._bytes = i.bytes ();
		tap -> _blocks.emplace_back (std::move (dB));
	}

	return (result);
}

// ---
MCHEmul::InfoStructure COMMODORE::DatasettePeripheral::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::IOPeripheral::getInfoStructure ());

	result.add ("KEYS",		!_noKeyPressed);
	result.add ("MOTOR",	!_motorOff);
	result.add ("DATANAME",	_data._name);
	result.add ("DATASIZE", _data._data.size ());

	MCHEmul::InfoStructure dS;
	for (size_t i = 0; i < (size_t) _data._data.size (); i++)
	{
		MCHEmul::InfoStructure dSA;
		dSA.add ("ID",		i); // The id...
		dSA.add ("SIZE",	_data._data [i].size ());
		dSA.add ("BYTES",	_data._data [i].bytes () /** It has to be a copy. */);
		dS.add (std::to_string (i), std::move (dSA));
	}

	result.add ("DATA", dS);

	return (result);
}
