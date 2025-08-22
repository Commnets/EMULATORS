#include <SINCLAIR/Datasette.hpp>

// ---
std::tuple <bool, bool, bool> SINCLAIR::Datasette::TZXDirectRecordingImplementation::timeToReadValue (unsigned int cC) 
{
	// Time to read?
	bool tr = false;
	// Which value is read?
	bool vr = false;
	// Get a new value from the file?
	bool nVR  = false;

	// Is is time for a new value?
	if (tr = (clockValue () >= _cyclesToAction))
	{
		// The value to read is codified into the _byteInfo. From MSB to LSB
		vr = _byteInfo.bit (_bitManaged);
		// ...but all bits have been processing,
		// and is time to read a neew value from the file?
		nVR = (_bitManaged == 0);
		// The bit to managed for the next iteration...
		_bitManaged = nVR ? 7 : _bitManaged - 1;

		// Time to restart...
		resetClock ();
	}

	return { tr, vr, nVR };
}

// ---
void SINCLAIR::Datasette::TZXDirectRecordingImplementation::whenReadingNewBlock (const MCHEmul::DataMemoryBlock& dB)
{
	// New parameters to sample...
	_cyclesToAction = (unsigned int) 
		MCHEmul::getAttributeAsInt ("TSTATES", dB.attributes ());
	// New information about the last bit in this block...
	_bitsLastByte = (unsigned int) 
		MCHEmul::getAttributeAsInt ("BITSUSEDLAST", dB.attributes ());

	// Restart the clock...
	resetClock ();
}

// ---
std::tuple <bool, MCHEmul::UByte> SINCLAIR::Datasette::TZXDirectRecordingImplementation::timeToWriteValue 
	(unsigned int cC, bool vc, unsigned int ccvc, bool vs)
{
	// Write a new value to the file?
	bool tw = false;
	// Which value must be written in that case?
	MCHEmul::UByte vw = MCHEmul::UByte::_0;

	// Is time to action?
	if (clockValue () >= _cyclesToAction)
	{
		// Shift left 1 and set the value of the bit 0 witjh the value vs...
		// As you notice, it doesn't mather whether the value in the EAR was or changed...
		// It saves at a specific pace!
		_byteInfo.shiftLeft (1).setBit (0, vs);
		// If the number of iterations (bits managed) is already 8 (or bigger)
		// it is time to write the value...
		// ...and the value is the one that has been managed for a while...
		if (tw = (++_bitManaged >= 8))
			vw = _byteInfo;

		// Restart the clock!
		resetClock ();
	}

	return { tw, vw };
}

// ---
void SINCLAIR::Datasette::TZXDirectRecordingImplementation::whenCreatingNewBlock 
	(const MCHEmul::DataMemoryBlock& dB)
{
	// The numberof Tstates to wait is read from the header...
	_cyclesToAction = (unsigned int) 
		MCHEmul::getAttributeAsInt ("TSTATES", dB.attributes ());
}

// ---
void SINCLAIR::Datasette::TZXDirectRecordingImplementation::initialize 
	(MCHEmul::StandardDatasette::Status st)
{
	MCHEmul::StandardDatasette::Implementation::initialize (st);

	_byteInfo = MCHEmul::UByte::_0;

	_bitManaged = (st == MCHEmul::StandardDatasette::Status::_READING) 
		? 7 /** The MSB when reading. */ : 0; // The LSB in other cases...
}

// ---
SINCLAIR::Datasette::Datasette (unsigned int rS)
	: MCHEmul::StandardDatasette (_ID, 
		new SINCLAIR::Datasette::TZXDirectRecordingImplementation (rS, 8), false /** Controlled externally. */,
		{ { "Name", "Datasette SINCLAIR" },
		  { "Manufacturer", "Almost anyone, No special manufacturer needed" } }),
	  _originalData () // Assigned when connectdata is executed...
{
	setClassName ("SINCLAIRDatasette");
}

// ---
bool SINCLAIR::Datasette::connectData (MCHEmul::FileData* dt)
{
	// The parent class moves RAW type, 
	// that is not going to be valid for this datasette...

	// Only TZX is admitted...
	// ...and everything is transformed into DirectRecording type of records...
	if (dynamic_cast <SINCLAIR::TZXFileData*> (dt) == nullptr)
		return (false); // That type of info is not valid from the datasette...
	// But TZX is only admitted when all blocks talks about sampling info...
	if (!static_cast <SINCLAIR::TZXFileData*> (dt) -> canBeSampled ())
		return (false);

	// Saves the data that cames from the file...
	_originalData = std::move (dt -> asMemoryBlocks ());
	// ...but moves only the one to be sampled...
	_data = std::move (static_cast <SINCLAIR::TZXFileData*> (dt) -> asSampledMemoryBlocks ());

	// And sets the counter to start...
	// In the first block with data if any...
	_dataCounter = std::numeric_limits <size_t>::max ();
	for (size_t i = 0; 
			i < _data._data.size () && 
			_dataCounter == std::numeric_limits <size_t>::max (); i++)
		if (_data._data [i].bytes ().size () > 0) _dataCounter = i;
	
	return (true); 
}

// ---
MCHEmul::FileData* SINCLAIR::Datasette::emptyData () const
{ 
	SINCLAIR::TZXFileData* result = new SINCLAIR::TZXFileData;

	// The header is always the same, so it is created here...
	result -> _header = SINCLAIR::TZXFileData::Header (1, 2 /** The minimum version. */);
	SINCLAIR::TZXFileData::InformationDataBlock* infoBlock;
	result -> _dataBlocks.emplace_back (infoBlock = new SINCLAIR::TZXFileData::InformationDataBlock);
	infoBlock -> _title			= "My Own Datasette";
	infoBlock -> _house			= "Community Networks";
	infoBlock -> _author		= "Ignacio Cea";
	infoBlock -> _year			= "2025";
	infoBlock -> _language		= "Spanish";
	infoBlock -> _game			= "Game";
	infoBlock -> _price			= "Free";
	infoBlock -> _protection	= "None";
	infoBlock -> _origin		= "Original";
	// No hardware block is created...
	// The method will requiere to be overloaded depending on the simulation!

	return (result);
}

// ---
MCHEmul::FileData* SINCLAIR::Datasette::retrieveData () const
{
	// The data that is get is the one managed!
	// If the original one had sampled data and it were changed....
	// the mesh could be great!, but like n a real datasette!
	SINCLAIR::TZXFileData* result = 
		SINCLAIR::TZXFileData::createFromMemoryBlock (_data);

	// Adds the name of the file...
	result -> _attributes ["FNAME"] = 
		MCHEmul::getAttribute ("FNAME", _originalData._attributes);

	// ...and returns it!
	return (result);
}

// ---
MCHEmul::DataMemoryBlock SINCLAIR::Datasette::createNewDataBlock ()
{
	MCHEmul::DataMemoryBlock result;

	// When using this method, the only type of data block is the standard one...
	SINCLAIR::TZXFileData::DirectRecordingDataBlock* drDB = 
		new SINCLAIR::TZXFileData::DirectRecordingDataBlock;
	drDB -> _tstates = static_cast 
		<SINCLAIR::Datasette::TZXDirectRecordingImplementation*> (implementation ()) -> cyclesToAction ();
	// The number of bits used in the last is not modified here...

	result = std::move (drDB -> asDataMemoryBlock ()); 
	// To avoid inclusing definitions here that are part of the TZXFileData
	delete drDB;

	return (result);
}
