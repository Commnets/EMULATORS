#include <SINCLAIR/FileIO.hpp>

// ---
std::string SINCLAIR::TZXFileData::Header::asString () const
{
	return (_name + " Version:" + 
		std::to_string ((int) _majorVersion) + "." + std::to_string ((int) _minorVersion)); 
}

// ---
std::string SINCLAIR::TZXFileData::DataBlock::asString () const 
{ 
	return ("Type:" + std::to_string ((int) _type) + 
		", Bytes:" + std::to_string (_data.size ())); 
}

// ---
MCHEmul::DataMemoryBlock SINCLAIR::TZXFileData::DataBlock::asDataMemoryBlock () const
{ 
	MCHEmul::DataMemoryBlock result (MCHEmul::Address (), _data);

	result.setAttribute ("TYPE", std::to_string ((unsigned int) _type));

	return (result);
}

// ---
MCHEmul::DataMemoryBlock SINCLAIR::TZXFileData::DataBlock::asSampledDataMemoryBlock () const
{
	MCHEmul::DataMemoryBlock result = std::move (asDataMemoryBlock ());

	// The data is initially no longer needed...
	result.clear ();
	if (canBeSampled ())
	{
		result.addBytes (dataAsSampledBytes ());

		result.setAttribute ("SAMPLED", "1");
	}

	return (result);
}

// ---
SINCLAIR::TZXFileData::StandardSpeedDataBlock::StandardSpeedDataBlock (const MCHEmul::DataMemoryBlock& dtblk)
	: SINCLAIR::TZXFileData::DataBlock (_ID, dtblk.bytes ()),
	  _blockLapse			((unsigned short) MCHEmul::getAttributeAsInt ("LAPSE", dtblk.attributes ()))
{
	// Nothing else...
}

// ---
std::string SINCLAIR::TZXFileData::StandardSpeedDataBlock::asString () const
{
	std::string result = 
		std::move (SINCLAIR::TZXFileData::DataBlock::asString ());

	result += ", Lapse:" + std::to_string (_blockLapse) + " ms";

	return (result);
}

// ---
MCHEmul::DataMemoryBlock SINCLAIR::TZXFileData::StandardSpeedDataBlock::asDataMemoryBlock () const
{
	MCHEmul::DataMemoryBlock result = 
		std::move (SINCLAIR::TZXFileData::DataBlock::asDataMemoryBlock ());

	// Information of the block!
	result.setAttribute		("LAPSE", std::to_string (_blockLapse));

	return (result);
}

// ---
SINCLAIR::TZXFileData::TurboSpeedDataBlock::TurboSpeedDataBlock (const MCHEmul::DataMemoryBlock& dtblk)
	: SINCLAIR::TZXFileData::DataBlock (_ID, dtblk.bytes ()),
	  _pilotPulse			((unsigned short) MCHEmul::getAttributeAsInt ("PILOTPULSE", dtblk.attributes ())),
	  _pilotNumberPulses	((unsigned short) MCHEmul::getAttributeAsInt ("PILOTNUMBERPULSES", dtblk.attributes ())),
	  _syncFirstPulse		((unsigned short) MCHEmul::getAttributeAsInt ("SYNCFIRSTPULSE", dtblk.attributes ())),
	  _syncSecondPulse		((unsigned short) MCHEmul::getAttributeAsInt ("SYNCSECONDPULSE", dtblk.attributes ())),
	  _zeroPulse			((unsigned short) MCHEmul::getAttributeAsInt ("ZEROPULSE", dtblk.attributes ())),
	  _onePulse				((unsigned short) MCHEmul::getAttributeAsInt ("ONEPULSE", dtblk.attributes ())),
	  _bitsUsedLast			((unsigned char) MCHEmul::getAttributeAsInt ("BITSUSEDLAST", dtblk.attributes ())),
	  _blockLapse			((unsigned short) MCHEmul::getAttributeAsInt ("LAPSE", dtblk.attributes ()))
{
	// Nothing else...
}

// ---
std::string SINCLAIR::TZXFileData::TurboSpeedDataBlock::asString () const
{
	std::string result = 
		std::move (SINCLAIR::TZXFileData::DataBlock::asString ());

	result += ", Pilot Pulse:" + std::to_string (_pilotPulse) + " Ts";
	result += ", Pilot Number Pulses:" + std::to_string (_pilotNumberPulses);
	result += ", Sync First Pulse:" + std::to_string (_syncFirstPulse) + " Ts";
	result += ", Sync Second Pulse:" + std::to_string (_syncSecondPulse) + " Ts";
	result += ", Zero Pulse:" + std::to_string (_zeroPulse) + " Ts";
	result += ", One Pulse:" + std::to_string (_onePulse) + " Ts";
	result += ", Bits Used Last:" + std::to_string (_bitsUsedLast);
	result += ", Lapse:" + std::to_string (_blockLapse) + " ms.";

	return (result);
}

// ---
MCHEmul::DataMemoryBlock SINCLAIR::TZXFileData::TurboSpeedDataBlock::asDataMemoryBlock () const
{
	MCHEmul::DataMemoryBlock result = 
		std::move (SINCLAIR::TZXFileData::DataBlock::asDataMemoryBlock ());

	// Information of the block!
	result.setAttribute		("PILOTPULSE", std::to_string (_pilotPulse));
	result.setAttribute		("PILOTNUMBERPULSES", std::to_string (_pilotNumberPulses));
	result.setAttribute		("SYNCFIRSTPULSE", std::to_string (_syncFirstPulse));
	result.setAttribute		("SYNCSECONDPULSE", std::to_string (_syncSecondPulse));
	result.setAttribute		("ZEROPULSE", std::to_string (_zeroPulse));
	result.setAttribute		("ONEPULSE", std::to_string (_onePulse));
	result.setAttribute		("BITSUSEDLAST", std::to_string (_bitsUsedLast));
	result.setAttribute		("LAPSE", std::to_string (_blockLapse));

	return (result);
}

// ---
std::vector <MCHEmul::UByte> SINCLAIR::TZXFileData::TurboSpeedDataBlock::dataAsSampledBytes () const
{
	std::vector <MCHEmul::UByte> result;

	// Manage the partial data...
	std::vector <bool> pData;
	auto addToResult = [&result, &pData](std::vector <bool>& p) -> void
		{
			// TODO
		};

	for (const auto& i : _data)
	{
		// TODO
	}

	return (result);
}

// ---
SINCLAIR::TZXFileData::DirectRecordingDataBlock::DirectRecordingDataBlock 
		(const MCHEmul::DataMemoryBlock& dtblk)
	: SINCLAIR::TZXFileData::DataBlock (_ID, dtblk.bytes ()),
	  _tstates				((unsigned short) MCHEmul::getAttributeAsInt ("TSTATES", dtblk.attributes ())),
	  _bitsUsedLast			((unsigned short) MCHEmul::getAttributeAsInt ("BITSUSEDLAST", dtblk.attributes ())),
	  _blockLapse			((unsigned short) MCHEmul::getAttributeAsInt ("LAPSE", dtblk.attributes ()))
{
	// Mothing else...
}

// ---
std::string SINCLAIR::TZXFileData::DirectRecordingDataBlock::asString () const
{
	std::string result = 
		std::move (SINCLAIR::TZXFileData::DataBlock::asString ());

	result += ", T-States:" + std::to_string (_tstates);
	result += ", Bits Used Last:" + std::to_string (_bitsUsedLast);
	result += ", Lapse:" + std::to_string (_blockLapse) + " ms";

	return (result);
}

// ---
MCHEmul::DataMemoryBlock SINCLAIR::TZXFileData::DirectRecordingDataBlock::asDataMemoryBlock () const
{
	MCHEmul::DataMemoryBlock result = 
		std::move (SINCLAIR::TZXFileData::DataBlock::asDataMemoryBlock ());

	result.setAttribute		("TSTATES", std::to_string (_tstates));
	result.setAttribute		("BITSUSEDLAST", std::to_string (_bitsUsedLast));
	result.setAttribute		("LAPSE", std::to_string (_blockLapse));

	return (result);
}

// ---
SINCLAIR::TZXFileData::InformationDataBlock::InformationDataBlock (const MCHEmul::DataMemoryBlock& dtblk)
	: SINCLAIR::TZXFileData::DataBlock (_ID, dtblk.bytes ()),
	  _title				(MCHEmul::getAttribute ("TITLE", dtblk.attributes ())),
	  _house				(MCHEmul::getAttribute ("HOUSE", dtblk.attributes ())),
	  _author				(MCHEmul::getAttribute ("AUTHOR", dtblk.attributes ())),
	  _year					(MCHEmul::getAttribute ("YEAR", dtblk.attributes ())),
	  _language				(MCHEmul::getAttribute ("LANGUAGE", dtblk.attributes ())),
	  _game					(MCHEmul::getAttribute ("GAME", dtblk.attributes ())),
	  _price				(MCHEmul::getAttribute ("PRICE", dtblk.attributes ())),
	  _protection			(MCHEmul::getAttribute ("PROTECTION", dtblk.attributes ())),
	  _origin				(MCHEmul::getAttribute ("ORIGIN", dtblk.attributes ())),
	  _comments { } // The comments are assigned in the body...
{
	size_t i = 0;
	bool f = true;
	while (!f)
	{
		std::string attrn = 
			"COMMNETS" + MCHEmul::fixLenStr (std::to_string (i++), 3, true, MCHEmul::_CEROS);
		if ((f = MCHEmul::isThereAttribute (attrn, dtblk.attributes ())))
			_comments.push_back (MCHEmul::getAttribute (attrn, dtblk.attributes ()));
	}
}

// ---
std::string SINCLAIR::TZXFileData::InformationDataBlock::asString () const
{ 
	std::string result = 
		std::move (SINCLAIR::TZXFileData::DataBlock::asString ());

	// The information is not sampling info, so it is not included in the string...
	result += ", Title:" + _title;
	result += ", House:" + _house;
	result += ", Author:" + _author;
	result += ", Year:" + _year;
	result += ", Language:" + _language;
	result += ", Game:" + _game;
	result += ", Price:" + _price;
	result += ", Protection:" + _protection;
	result += ", Origin:" + _origin;

	return (result);
}

// ---
MCHEmul::DataMemoryBlock SINCLAIR::TZXFileData::InformationDataBlock::asDataMemoryBlock () const
{
	MCHEmul::DataMemoryBlock result = 
		std::move (SINCLAIR::TZXFileData::DataBlock::asDataMemoryBlock ());

	// In this block there is no data...

	result.setAttribute		("TITLE",		_title);
	result.setAttribute		("HOUSE",		_house);
	result.setAttribute		("AUTHOR",		_author);
	result.setAttribute		("YEAR",		_year);
	result.setAttribute		("LANGUAGE",	_language);
	result.setAttribute		("GAME",		_game);
	result.setAttribute		("PRICE",		_price);
	result.setAttribute		("PROTECTION",	_protection);
	result.setAttribute		("ORIGIN",		_origin);

	// The commnents:: COMMENT00x...
	size_t i = 0;
	for (const auto& j : _comments)
		result.setAttribute ("COMMENTS" + 
			MCHEmul::fixLenStr (std::to_string (i++), 3, true, MCHEmul::_CEROS), j);

	return (result);
}

// ---
std::string SINCLAIR::TZXFileData::HardwareDataBlock::asString () const
{ 
	std::string result = 
		std::move (SINCLAIR::TZXFileData::DataBlock::asString ());

	size_t ct = 0;
	bool first = false;
	for (const auto& i : _machines)
	{
		// The first one is not preceded by a comma...
		result += (first ? "," : "");
		result += "Machine " + std::to_string (ct++) + ":" + i.asString (); 
	}

	return (result);
}

// ---
SINCLAIR::TZXFileData::HardwareDataBlock::HardwareDataBlock (const MCHEmul::DataMemoryBlock& dtblk)
	: SINCLAIR::TZXFileData::DataBlock (_ID, { }),
	  _machines { } // The machines are added in the body...
{
	bool end = false;
	size_t i = 0;
	while (!end)
	{
		std::string pN = "MACHINE" + 
			MCHEmul::fixLenStr (std::to_string (i++), 3, true, MCHEmul::_SPACES);
		if (end = !dtblk.existAttribute (pN)) continue;
		
		// If the attribute exists, it is added to the list of machines...
		MCHEmul::Strings mA = MCHEmul::getElementsFrom (MCHEmul::getAttribute (pN, dtblk.attributes ()), ',');
		if (mA.size () < 3)
		{
			_LOG ("Hardware Data Block: Machine " + std::to_string (i) + 
				  " has not enough attributes, ignoring it");
			continue; // Not enough attributes to create a machine...
		}

		_machines.emplace_back (SINCLAIR::TZXFileData::HardwareDataBlock::MachineInfo {
			(unsigned char) std::atoi (mA [0].c_str ()),	// Type,
			(unsigned char) std::atoi (mA [1].c_str ()),	// Id,
			(unsigned char) std::atoi (mA [2].c_str ()) });	// Info
	}
}

// ---
MCHEmul::DataMemoryBlock SINCLAIR::TZXFileData::HardwareDataBlock::asDataMemoryBlock () const
{
	MCHEmul::DataMemoryBlock result = 
		std::move (SINCLAIR::TZXFileData::DataBlock::asDataMemoryBlock ());

	size_t i = 0;
	for (const auto& j : _machines)
		result.setAttribute ("MACHINE" + 
			MCHEmul::fixLenStr (std::to_string (i++), 3, true, MCHEmul::_SPACES), j.justAttributes ());

	return (result);
}

// ---
SINCLAIR::TZXFileData::~TZXFileData ()
{
	for (const auto& i : _dataBlocks)
		delete (i);
}

// ---
MCHEmul::ExtendedDataMemoryBlocks SINCLAIR::TZXFileData::asMemoryBlocks () const
{
	MCHEmul::ExtendedDataMemoryBlocks result;

	// The commong things...
	result._name = _header._name;
	MCHEmul::Attributes attrs;
	attrs ["MAJORVERSION"]	= std::to_string ((unsigned int) _header._majorVersion);
	attrs ["MINORVERSION"]	= std::to_string ((unsigned int) _header._minorVersion);
	for (const auto& i : _attributes)
		attrs [i.first] = i.second; // Copy the attributes (none can have the same name than the ones above)...
	result._attributes	= std::move (attrs); // It is not longer valid...

	// Adds the different data blocks...
	for (const auto& i : _dataBlocks)
		result._data.emplace_back (std::move (i -> asDataMemoryBlock ()));

	return (result);
}

// ---
MCHEmul::ExtendedDataMemoryBlocks SINCLAIR::TZXFileData::asSampledMemoryBlocks () const
{
	// Invokes to the previous one, just to avoid code duplication...
	MCHEmul::ExtendedDataMemoryBlocks result = std::move (asMemoryBlocks ());
	// The data is not valid, so it is emptied (it has to be sampled)...
	result._data = { }; 

	// Adds the different data blocks, but sampling the information...if posssible!
	for (const auto& i : _dataBlocks)
		result._data.emplace_back (std::move (i -> asSampledDataMemoryBlock ()));

	return (result);
}

// ---
bool SINCLAIR::TZXFileData::canBeSampled () const
{
	bool result = true;
	for (const auto& i : _dataBlocks)
		if (!i -> isJustInfo ())
			result &= i -> canBeSampled (); 

	return (result);
}

// ---
bool SINCLAIR::TZXFileData::isOnlyLikeTAPInfo () const
{
	bool result = true;
	for (const auto& i : _dataBlocks)
		if (!i -> isJustInfo ())
			result &= i -> isLikeTAPFormat ();

	return (result);
}

// ---
void SINCLAIR::TZXFileData::addHeaderFromExtendeDataMemoryBlock 
	(const MCHEmul::ExtendedDataMemoryBlocks& dMB)
{
	_header._name = dMB._name;
	_header._majorVersion = (unsigned char) MCHEmul::getAttributeAsInt ("MAJORVERSION", dMB._attributes);
	_header._minorVersion = (unsigned char) MCHEmul::getAttributeAsInt ("MINORVERSION", dMB._attributes);
}

// ---
void SINCLAIR::TZXFileData::addDataBlockFromDataMemoryBlock (const MCHEmul::DataMemoryBlock& dtblk)
{
	// With all blocks...
	SINCLAIR::TZXFileData::DataBlock* dB = nullptr;

	// Depending on the type of memorty block is created...
	std::string tp = MCHEmul::trim (MCHEmul::getAttribute ("TYPE", dtblk.attributes ()));
	switch (std::atoi (tp.c_str ()))
	{
		case SINCLAIR::TZXFileData::StandardSpeedDataBlock::_ID:
			{
				dB = new SINCLAIR::TZXFileData::StandardSpeedDataBlock (dtblk);
			}

			break;

		case SINCLAIR::TZXFileData::TurboSpeedDataBlock::_ID:
			{
				dB = new SINCLAIR::TZXFileData::TurboSpeedDataBlock (dtblk);
			}

			break;

		case SINCLAIR::TZXFileData::DirectRecordingDataBlock::_ID:
			{
				dB = new SINCLAIR::TZXFileData::DirectRecordingDataBlock (dtblk);
			}

			break;

		case SINCLAIR::TZXFileData::InformationDataBlock::_ID:
			{
				dB = new SINCLAIR::TZXFileData::InformationDataBlock (dtblk);
			}

			break;

		case SINCLAIR::TZXFileData::HardwareDataBlock::_ID:
			{
				dB = new SINCLAIR::TZXFileData::HardwareDataBlock (dtblk);
			}

			break;

		// Any other type of bloxk is not supported so far!
		default:
			break;
	}

	// ...and added to the list of data blocks if possible...
	if (dB) // If there were a mistake, adding it is not possible!
		_dataBlocks.push_back (dB);
	// ...otherwise a log entry is created
	else
		_LOG ("Data Memory Block type:" + ((tp == "") ? "-" : tp) + " doesn't exist");
}

// ---
std::string SINCLAIR::TZXFileData::asString () const
{
	std::string result = "";

	// The header...
	result += _header.asString ();
	// ...and the data...
	size_t ct = 0;
	for (const auto& i : _dataBlocks)
		// ...depending on the type it will be printed out one thing or another...
		result += ". Block " + std::to_string (ct++) + 
			" (" + i -> asString () + ")";

	return (result);
}

// ---
bool SINCLAIR::TZXFileTypeIO::canRead (const std::string& fN) const
{
	// Extension?
	size_t pp = fN.find_last_of ('.');
	if (pp == std::string::npos || pp == fN.length ())
		return (false); // ...no extension provided, so no valid file...

	// The right extension?
	std::string ext = MCHEmul::upper (fN.substr (pp + 1));
	if (ext != "TZX")
		return (false); // ...no

	// Possible to be opened?
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (false); // ...no

	// The right size of the file?
	f.seekg (0, std::ios::end);
	std::streamoff s = f.tellg ();
	if (s < (std::streamoff) 10 /** @see header, with no blocks */)
		return (false);

	// Those first 10 bytes must be the header
	// The header has a very specific structure
	f.seekg (0, std::ios::beg);
	char chr [255] = { };
	f.read (chr, 7);
	chr [7] = 0; // End of string...
	if (std::string (chr) != "ZXTape!")
		return (false); // Must have this name...
	f.read (chr, 3); // Including the major and the minor version...
	if (chr [0] != 0x1a)
		return (false); // And after the name must have this specific attribute

	// The otjher two are the version and the subversion 
	// and they are not important to verify whether the format is right!

	f.close ();

	return (true);
}

// ---
MCHEmul::FileData* SINCLAIR::TZXFileTypeIO::readFile (const std::string& fN, bool bE) const
{
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (nullptr); // Impossible to be open... 
						  // At this point it shouldn't happen but just in case...

	char chr [256] = { }; // To read the info from the file...
	MCHEmul::FileData* result = new SINCLAIR::TZXFileData;
	SINCLAIR::TZXFileData* tzx = 
		static_cast <SINCLAIR::TZXFileData*> (result); // To better manipulation...

	// First of all the number of bytes within the file is given...
	f.seekg (0, std::ios::end);
	std::streamoff s = f.tellg ();
	f.seekg (0, std::ios::beg); // back to the beginning...

	// The name of the file...
	tzx -> _attributes ["FNAME"] = fN;

	// Read the header first...
	if (f.read (chr, 7).eof ())
		{ delete result; return (nullptr); }
	chr [7] = 0; // End of string...
	tzx -> _header._name = std::string (chr);
	// The character 0 is not taken into account = 0x1a (usually)
	if (f.read (chr, 3).eof ())
		{ delete result; return (nullptr); } 
	tzx -> _header._majorVersion = (unsigned char) chr [1];
	tzx -> _header._minorVersion = (unsigned char) chr [2];

	// And then the different bloks until the end of the file (or error)...
	bool e = false; // No errror...so far...
	while (!f.eof () && !e)
	{
		// Reads the type of the block...
		if (f.read (chr, 1).eof ()) continue; // No more types...
		SINCLAIR::TZXFileData::DataBlock* dB = nullptr;
		switch (chr [0])
		{
			// Standard type...
			// This is the format used by the load/save standard basic commands...
			case SINCLAIR::TZXFileData::StandardSpeedDataBlock::_ID:
				{
					dB = readBlockType0x10 (f);
				}

				break;

			// This format is like the previous one, but it is created using TURBO loader!
			case SINCLAIR::TZXFileData::TurboSpeedDataBlock::_ID:
				{
					dB = readBlockType0x11 (f);
				}

				break;

			// This is the type with information about a pure recording!...
			case SINCLAIR::TZXFileData::DirectRecordingDataBlock::_ID:
				{
					dB = readBlockType0x15 (f);
				}

				break;

			// The block type 0x32 is for the comments...
			case SINCLAIR::TZXFileData::InformationDataBlock::_ID:
				{
					dB = readBlockType0x32 (f);
				}

				break;

			// The block type 0x33 is for the hardware info...
			case SINCLAIR::TZXFileData::HardwareDataBlock::_ID:
				{
					dB = readBlockType0x33 (f);
				}

				break;

			// It shouldn't be here, meaning this type of block is not supported...
			default:
				{
					e = false;

					_LOG ("Type of TZX block not supported:" + std::to_string ((int) chr [0]));
				}
		}

		if (!(e = (dB == nullptr)))
			tzx -> _dataBlocks.emplace_back (dB);
	}

	f.close ();

	// If there were any error...
	if (e)
	{ 
		delete tzx;
		result = tzx = new SINCLAIR::TZXFileData;
		// ...and empty object will be returned
		// This is something to be managed later as an error...
	}

	return (result);
}

// ---
bool SINCLAIR::TZXFileTypeIO::writeFile (MCHEmul::FileData* fD, const std::string& fN, bool bE) const
{
	SINCLAIR::TZXFileData* tzx = 
		dynamic_cast <SINCLAIR::TZXFileData*> (fD); // To better manipulation...
	if (tzx == nullptr)
		return (false); // it is not really a tap structure!

	// There might not be name of the file,
	// It it were the case, the name defined in the data file should be taken instead
	// and if there were not event that, a mistake is generated!
	std::string lFN = fN;
	if (lFN == "")
	{
		MCHEmul::Attributes::const_iterator p = tzx -> _attributes.find ("FNAME");
		if (p == tzx -> _attributes.end ())
			return (false);
		else
			lFN = (*p).second; // The name of the file to write...
	}

	// Open the file, but keeps its name, just for the next time if any!
	std::ofstream f (tzx -> _attributes ["FNAME"] = lFN, 
		std::ios::out | std::ios::binary);
	if (!f)
		return (false); // Impossible to be opened...

	char data [256] = { };

	// The header...
	// The name...
	size_t i = 0;
	for (; i < tzx -> _header._name.size () && i < 7; i++)
		data [i] = tzx -> _header._name [i];
	// The symbol...
	data [7] = 0x1a;
	f.write (data, 8); // Including the symbol 0x1a..
	// The version and the subversion...
	data [0] = (char) tzx -> _header._majorVersion;
	data [1] = (char) tzx -> _header._minorVersion;
	f.write (data, 2);

	// The different elements...
	bool e = false;
	for (std::vector <SINCLAIR::TZXFileData::DataBlock*>::const_iterator i = tzx -> _dataBlocks.begin ();
			i != tzx -> _dataBlocks.end () && !e; i++)
	{
		// First of all, it saves the type...
		data [0] = (*i) -> _type;
		f.write (data, 1);
		// ...and depending on it, the resto of the information...
		switch (data [0])
		{
			// The type 10...
			case SINCLAIR::TZXFileData::StandardSpeedDataBlock::_ID:
				{
					writeBlockType0x10 
						(f, static_cast <SINCLAIR::TZXFileData::StandardSpeedDataBlock*> (*i));
				}

				break;

			// The type 10...
			case SINCLAIR::TZXFileData::TurboSpeedDataBlock::_ID:
				{
					writeBlockType0x11
						(f, static_cast <SINCLAIR::TZXFileData::TurboSpeedDataBlock*> (*i));
				}

				break;

			// The Direct Recording...
			case SINCLAIR::TZXFileData::DirectRecordingDataBlock::_ID:
				{
					writeBlockType0x15
						(f, static_cast <SINCLAIR::TZXFileData::DirectRecordingDataBlock*> (*i));
				}

				break;

			// The type 0x32 is the one with the comments...
			case SINCLAIR::TZXFileData::InformationDataBlock::_ID:
				{
					writeBlockType0x32
						(f, static_cast <SINCLAIR::TZXFileData::InformationDataBlock*> (*i));
				}

				break;

			// The type 0x33 is the one with the hardware info...
			case SINCLAIR::TZXFileData::HardwareDataBlock::_ID:
				{
					writeBlockType0x33
						(f, static_cast <SINCLAIR::TZXFileData::HardwareDataBlock*> (*i));
				}

				break;

			// It shouldn't be here, meaning this type of block is not supported...
			default:
				{
					e = false;

					_LOG ("Type of TZX block not supported:" + std::to_string ((int) (*i) -> _type));
				}
		}
	}

	f.close ();

	return (true);
}

// ---
SINCLAIR::TZXFileData::StandardSpeedDataBlock* SINCLAIR::TZXFileTypeIO::readBlockType0x10 (std::ifstream& f) const
{
	SINCLAIR::TZXFileData::StandardSpeedDataBlock* result = 
		new SINCLAIR::TZXFileData::StandardSpeedDataBlock;

	char chr [256] = { };
	if (f.read (chr, 2).eof ())
		{ delete result; return (nullptr); }
	result -> _blockLapse = (unsigned short) ((((unsigned char) chr [1]) << 8) + ((unsigned char) chr [0]));
	if (f.read (chr, 2).eof ())
		{ delete result; return (nullptr); }
	unsigned short dl = (unsigned short) ((((unsigned char) chr [1]) << 8) + ((unsigned char) chr [0]));

	MCHEmul::UByte ck = MCHEmul::UByte::_0;
	char* dt = new char [dl];
	if (f.read (dt, dl).eof ()) 
		{ delete result; return (nullptr); }
	for (unsigned short i = 0; i < dl; i++)
	{
		MCHEmul::UByte b ((unsigned char) dt [i]);
		ck = ck ^ b; // Verify the checksum..
		// Inserts all except the last one because it is the checksum...
		if (i != (dl - 1))
			result -> _data.emplace_back (b);
	}

	delete [] dt;
	if (ck != MCHEmul::UByte::_0) // ? Checksum...
		{ delete result; return (nullptr); }

	return (result);
}

// ---
void SINCLAIR::TZXFileTypeIO::writeBlockType0x10 (std::ofstream& f,
	SINCLAIR::TZXFileData::StandardSpeedDataBlock* b) const
{
	// Just in case...
	assert (b != nullptr);

	char chr [256] = { };

	// Lapse...
	chr [0] = (char) (b -> _blockLapse & 0x00ff);
	chr [1] = (char) ((b -> _blockLapse & 0xff00) >> 8);
	f.write (chr, 2);
	// Data length...
	chr [0] = (char) ((unsigned short) (b -> _data.size ()) & 0x00ff);
	chr [1] = (char) ((unsigned short) ((b -> _data.size ()) & 0xff00) >> 8);
	f.write (chr, 2);

	// The values...
	char* dt = new char [b -> _data.size ()];
	for (size_t i = 0; i < b -> _data.size (); i++)
		dt [i] = b -> _data [i].value ();
	f.write (dt, (std::streamsize) b -> _data.size ());

	// Notice that the file is not closed...
}

// ---
SINCLAIR::TZXFileData::TurboSpeedDataBlock* SINCLAIR::TZXFileTypeIO::readBlockType0x11 (std::ifstream& f) const
{
	SINCLAIR::TZXFileData::TurboSpeedDataBlock* result = 
		new SINCLAIR::TZXFileData::TurboSpeedDataBlock;

	char chr [256] = { };

	// Reads the definition bytes of the block...
	if (f.read (chr, 2).eof ())
		{ delete result; return (nullptr); }
	result -> _pilotPulse = (unsigned short) ((((unsigned char) chr [1]) << 8) + ((unsigned char) chr [0]));
	if (f.read (chr, 2).eof ())
		{ delete result; return (nullptr); }
	result -> _syncFirstPulse = (unsigned short) ((((unsigned char) chr [1]) << 8) + ((unsigned char) chr [0]));
	if (f.read (chr, 2).eof ())
		{ delete result; return (nullptr); }
	result -> _syncSecondPulse = (unsigned short) ((((unsigned char) chr [1]) << 8) + ((unsigned char) chr [0]));
	if (f.read (chr, 2).eof ())
		{ delete result; return (nullptr); }
	result -> _zeroPulse = (unsigned short) ((((unsigned char) chr [1]) << 8) + ((unsigned char) chr [0]));
	if (f.read (chr, 2).eof ())
		{ delete result; return (nullptr); }
	result -> _onePulse = (unsigned short) ((((unsigned char) chr [1]) << 8) + ((unsigned char) chr [0]));
	if (f.read (chr, 2).eof ())
		{ delete result; return (nullptr); }
	result -> _pilotNumberPulses = (unsigned short) ((((unsigned char) chr [1]) << 8) + ((unsigned char) chr [0]));
	if (f.read (chr, 1).eof ())
		{ delete result; return (nullptr); }
	result -> _bitsUsedLast = (unsigned char) chr [0];
	if (f.read (chr, 2).eof ())
		{ delete result; return (nullptr); }
	result -> _blockLapse = (unsigned short) ((((unsigned char) chr [1]) << 8) + ((unsigned char) chr [0]));

	// The length of the data block that is about to follow...
	if (f.read (chr, 3).eof ())
		{ delete result; return (nullptr); }
	unsigned int dl = (unsigned int) ((((unsigned char) chr [2]) << 16) + 
		(((unsigned char) chr [1]) << 8) + ((unsigned char) chr [0]));

	MCHEmul::UByte ck = MCHEmul::UByte::_0;
	char* dt = new char [dl];
	if (f.read (dt, dl).eof ()) 
		{ delete result; return (nullptr); }
	for (unsigned short i = 0; i < dl; i++)
	{
		MCHEmul::UByte b ((unsigned char) dt [i]);
		ck = ck ^ b; // Verify the checksum..
		// Inserts all except the last one because it is the checksum...
		if (i != (dl - 1))
			result -> _data.emplace_back (b);
	}

	delete [] dt;
	if (ck != MCHEmul::UByte::_0) // ? Checksum...
		{ delete result; return (nullptr); }

	return (result);
}

// ---
void SINCLAIR::TZXFileTypeIO::writeBlockType0x11 (std::ofstream& f,
	SINCLAIR::TZXFileData::TurboSpeedDataBlock* b) const
{
	// Just in case...
	assert (b != nullptr);

	char chr [256] = { };

	// Pilot pulse...
	chr [0] = (char)  (b -> _pilotPulse & 0x00ff);
	chr [1] = (char) ((b -> _pilotPulse & 0xff00) >> 8);
	f.write (chr, 2);
	// Pilot (first) pulse...
	chr [0] = (char)  (b -> _syncFirstPulse & 0x00ff);
	chr [1] = (char) ((b -> _syncFirstPulse & 0xff00) >> 8);
	f.write (chr, 2);
	// Pilot (second) pulse...
	chr [0] = (char)  (b -> _syncSecondPulse & 0x00ff);
	chr [1] = (char) ((b -> _syncSecondPulse & 0xff00) >> 8);
	f.write (chr, 2);
	// Zero pulse...
	chr [0] = (char)  (b -> _zeroPulse & 0x00ff);
	chr [1] = (char) ((b -> _zeroPulse & 0xff00) >> 8);
	f.write (chr, 2);
	// One pulse...
	chr [0] = (char)  (b -> _onePulse & 0x00ff);
	chr [1] = (char) ((b -> _onePulse & 0xff00) >> 8);
	f.write (chr, 2);
	// Pilot number pulses...
	chr [0] = (char)  (b -> _pilotNumberPulses & 0x00ff);
	chr [1] = (char) ((b -> _pilotNumberPulses & 0xff00) >> 8);
	f.write (chr, 2);
	// Bits in the last byte...
	chr [0] = (unsigned char) b -> _bitsUsedLast;
	f.write (chr, 1);
	// Lapse...
	chr [0] = (char)  (b -> _blockLapse & 0x00ff);
	chr [1] = (char) ((b -> _blockLapse & 0xff00) >> 8);
	f.write (chr, 2);
	// Data length...
	chr [0] = (char) ((unsigned int)  (b -> _data.size ()) & 0x000000ff);
	chr [1] = (char) ((unsigned int) ((b -> _data.size ()) & 0x0000ff00) >> 8);
	chr [1] = (char) ((unsigned int) ((b -> _data.size ()) & 0x00ff0000) >> 16);
	f.write (chr, 3);

	// The values...
	char* dt = new char [b -> _data.size ()];
	for (size_t i = 0; i < b -> _data.size (); i++)
		dt [i] = b -> _data [i].value ();
	f.write (dt, (std::streamsize) b -> _data.size ());

	// Notice that the file is not closed...
}

// ---
SINCLAIR::TZXFileData::DirectRecordingDataBlock* SINCLAIR::TZXFileTypeIO::readBlockType0x15 (std::ifstream& f) const
{
	SINCLAIR::TZXFileData::DirectRecordingDataBlock* result = 
		new SINCLAIR::TZXFileData::DirectRecordingDataBlock;

	char chr [256] = { };

	// Reads the definition bytes of the block...
	if (f.read (chr, 2).eof ())
		{ delete result; return (nullptr); }
	result -> _tstates = (unsigned short) ((((unsigned char) chr [1]) << 8) + ((unsigned char) chr [0]));
	if (f.read (chr, 2).eof ())
		{ delete result; return (nullptr); }
	result -> _blockLapse = (unsigned short) ((((unsigned char) chr [1]) << 8) + ((unsigned char) chr [0]));
	if (f.read (chr, 1).eof ())
		{ delete result; return (nullptr); }
	result -> _bitsUsedLast = (unsigned char) chr [0];

	// The length of the data block that is about to follow...
	if (f.read (chr, 3).eof ())
		{ delete result; return (nullptr); }
	unsigned int dl = (unsigned int) ((((unsigned char) chr [2]) << 16) + 
		(((unsigned char) chr [1]) << 8) + ((unsigned char) chr [0]));

	MCHEmul::UByte ck = MCHEmul::UByte::_0;
	char* dt = new char [dl];
	if (f.read (dt, dl).eof ()) 
		{ delete result; return (nullptr); }
	for (unsigned short i = 0; i < dl; i++)
	{
		MCHEmul::UByte b ((unsigned char) dt [i]);
		ck = ck ^ b; // Verify the checksum..
		// Inserts all except the last one because it is the checksum...
		if (i != (dl - 1))
			result -> _data.emplace_back (b);
	}

	delete [] dt;
	if (ck != MCHEmul::UByte::_0) // ? Checksum...
		{ delete result; return (nullptr); }

	return (result);
}

// ---
void SINCLAIR::TZXFileTypeIO::writeBlockType0x15 
	(std::ofstream& f, SINCLAIR::TZXFileData::DirectRecordingDataBlock* b) const
{
	// Just in case...
	assert (b != nullptr);

	char chr [256] = { };

	// Numer of tstates per sample...
	chr [0] = (char)  (b -> _tstates & 0x00ff);
	chr [1] = (char) ((b -> _tstates & 0xff00) >> 8);
	f.write (chr, 2);
	// Lapse...
	chr [0] = (char)  (b -> _blockLapse & 0x00ff);
	chr [1] = (char) ((b -> _blockLapse & 0xff00) >> 8);
	f.write (chr, 2);
	// Bits in the last byte...
	chr [0] = (unsigned char) b -> _bitsUsedLast;
	f.write (chr, 1);
	// Data length...
	chr [0] = (char) ((unsigned int)  (b -> _data.size ()) & 0x000000ff);
	chr [1] = (char) ((unsigned int) ((b -> _data.size ()) & 0x0000ff00) >> 8);
	chr [2] = (char) ((unsigned int) ((b -> _data.size ()) & 0x00ff0000) >> 16);
	f.write (chr, 3);

	// The values...
	char* dt = new char [b -> _data.size ()];
	for (size_t i = 0; i < b -> _data.size (); i++)
		dt [i] = b -> _data [i].value ();
	f.write (dt, (std::streamsize) b -> _data.size ());

	// Notice that the file is not closed...
}

// ---
SINCLAIR::TZXFileData::InformationDataBlock* SINCLAIR::TZXFileTypeIO::readBlockType0x32 (std::ifstream& f) const
{
	SINCLAIR::TZXFileData::InformationDataBlock* result = 
		new SINCLAIR::TZXFileData::InformationDataBlock;

	char chr [20];
	if (f.read (chr, 2).eof ())
		{ delete result; return (nullptr); }
	// The first two bytes is the length of the block...
	unsigned short l = (unsigned short) ((((unsigned char) chr [1]) << 8) + ((unsigned char) chr [0]));
	char* dt = new char [l];
	if (f.read (dt, l).eof ())
		{ delete result; return (nullptr); }

	// Let's manage text by text...
	// The pointer to the texts starts at the byte 1 of the loaded info...
	// At the position 0, the byte points the number of texts to read...
	size_t p = 0x01;
	while (p < l) 
	{
		std::string txt;
		for (size_t i = p + 2; 
			i < (p + 2) + dt [(p + 1)]; i++) txt += dt [i];
		switch (dt [p])
		{
			case 0: result -> _title		= txt; break;
			case 1: result -> _house		= txt; break;
			case 2: result -> _author		= txt; break;
			case 3: result -> _year			= txt; break;
			case 4: result -> _language		= txt; break;
			case 5: result -> _game			= txt; break;
			case 6: result -> _price		= txt; break;
			case 7: result -> _protection	= txt; break;
			case 8: result -> _origin		= txt; break;
			case 0xff: result -> _comments.emplace_back (txt); break;
			default: break; // Nothing else supported...
		}

		p += 2 + dt [p + 1];
	}

	return (result);
}

// ---
void SINCLAIR::TZXFileTypeIO::writeBlockType0x32 
	(std::ofstream& f, SINCLAIR::TZXFileData::InformationDataBlock* b) const
{
	// Just in case...
	assert (b != nullptr);

	char chr [256] = { };

	// Temporal Structure to save the texts...
	struct TextData
	{
		unsigned char _type;	// Type of text
		std::string _text;		// The text itself
	};

	// Saves all texts...
	std::vector <TextData> texts;
	if (b -> _title != "")		texts.emplace_back (TextData { 0, b -> _title });
	if (b -> _house != "")		texts.emplace_back (TextData { 1, b -> _house });
	if (b -> _author != "")		texts.emplace_back (TextData { 2, b -> _author });
	if (b -> _year != "")		texts.emplace_back (TextData { 3, b -> _year });
	if (b -> _language != "")	texts.emplace_back (TextData { 4, b -> _language });
	if (b -> _game != "")		texts.emplace_back (TextData { 5, b -> _game });
	if (b -> _price != "")		texts.emplace_back (TextData { 6, b -> _price });
	if (b -> _protection != "")	texts.emplace_back (TextData { 7, b -> _protection });
	if (b -> _origin != "")		texts.emplace_back (TextData { 8, b -> _origin });
	for (const auto& i : b -> _comments)
		texts.emplace_back (TextData { 0xff, i });

	// Calculates the total length of the block...
	// The length includes 1 byte with the number of texts, and 2 bytes plus the length of the text itself per text...
	size_t l = 1;
	for (const auto& i : texts)
		l += 2 + i._text.length (); // 2 for the type and the length of the text

	// Saves the length of the block...
	chr [0] = (char) (l & 0x00ff);
	chr [1] = (char) ((l & 0xff00) >> 8);
	f.write (chr, 2);
	// ...then the number of texts...
	chr [0] = (char) (texts.size () & 0xff);
	f.write (chr, 1);
	// ...and then the texts...
	for (size_t i = 0; i < texts.size (); i++)
	{
		const std::string& txt = texts [i]._text;
		// The type of the text...
		chr [0] = (char) texts [i]._type;
		f.write (chr, 1);
		// The length of the text...
		chr [0] = (char) (txt.length () & 0xff);
		f.write (chr, 1);
		// The text itself...
		f.write (txt.c_str (), (std::streamsize) txt.length ());
	}
}

// ---
SINCLAIR::TZXFileData::HardwareDataBlock* SINCLAIR::TZXFileTypeIO::readBlockType0x33 (std::ifstream& f) const
{
	SINCLAIR::TZXFileData::HardwareDataBlock* result = 
		new SINCLAIR::TZXFileData::HardwareDataBlock;

	// The first byte defines the number of machines this code is for,---
	char chr [20];
	if (f.read (chr, 1).eof ())
		{ delete result; return (nullptr); }

	// Iterates over all 
	// And every machines is defined with three bytes...
	for (size_t i = (size_t) chr [0]; i > 0; i--)
	{
		if (f.read (chr, 3).eof ())
			{ delete result; return (nullptr); }
		result -> _machines.emplace_back 
			(SINCLAIR::TZXFileData::HardwareDataBlock::MachineInfo {
				(unsigned char) chr [0], // Type
				(unsigned char) chr [1], // Id
				(unsigned char) chr [2]  // Info
			});
	}

	return (result);
}

// ---
void SINCLAIR::TZXFileTypeIO::writeBlockType0x33 (std::ofstream& f,
	SINCLAIR::TZXFileData::HardwareDataBlock* b) const
{
	// Just in case...
	assert (b != nullptr);

	char chr [256] = { };

	// First of, saves the number of machines...
	chr [0] = (char) b -> _machines.size ();
	f.write (chr, 1);
	// Then iterates over all machines...
	for (const auto& i : b -> _machines)
	{
		// The type of the machine...
		chr [0] = (char) i._type;
		// The id of the machine...
		chr [1] = (char) i._id;
		// The info of the machine...
		chr [2] = (char) i._info;
		f.write (chr, 3);
	}
}

// ---
MCHEmul::Strings SINCLAIR::KeystrokeTypeIO::generateTokensFor (const std::string& str) const
{
	struct TokenPosition 
		{ size_t _position; std::string _token; };
	using TokenPositions = std::vector <TokenPosition>;

	// Converts the line into uppercase to find better the token if any...
	std::string uStr = MCHEmul::trim (str);

	// The tokens...
	MCHEmul::Strings result; 
	size_t lTP = 0;
	std::string c;
	for (size_t i = 0; i < uStr.size(); i++) 
	{
		// Over the current piece under construction...
		// ...collects a new character...
		char l = uStr [i];
		c += l;
		std::string uC = MCHEmul::upper (c);

		// ...and verifies whether a new token (of several) has been reached!
		// Iterating over all and checking them!
		bool tPF = false;
		TokenPosition tP { };
		for (MCHEmul::Strings::const_iterator j = _TOKENS.begin (); 
			j != _TOKENS.end (); j++) // We have to iterate over all...
		{
			// Position where the token has been found...
			size_t tPP = 0;
			// If a reference to a token has been found...(using uppercase)
			if ((tPP = uC.find ((*j))) != std::string::npos)
			{
				// Sometimes the token is very simple (one character)
				// and could be confused with other made up with the next char...
				// So it is needed to ensure that can be inserted... 
				// These types of tokens depends on the computer. Are not the same in a ZXSpectrum than in a ZX80 or ZX81 even...
				bool ins = ((*j).length () == 1)
					? isARealToken (l /* Last letter used. */, uStr /** The line. */, i /** The position. */)
					: true;

				// The insertion is only possible...
				// ...when requested but there is nothing before or something with less length...
				// If there were two tokens possible with the same length, only the first will be taken into account...
				if (ins && 
					(!tPF || (tPF && tP._token.length () < (*j).length ())))
				{
					tPF = true;

					tP = TokenPosition { lTP + tPP, (*j) };
				}
			}
		}

		// If a token were found...
		if (tPF)
		{
			// The previous one could be a token...
			if (lTP < tP._position)
				result.emplace_back (MCHEmul::trim (uStr.substr (lTP, tP._position - lTP)));
			// ...and for sure the token found...
			result.emplace_back (MCHEmul::trim 
				(MCHEmul::upper (uStr.substr (tP._position, tP._token.length ())))); // The insertion in uppercase...
			// The new position...
			lTP = tP._position + tP._token.length (); 
			// Starts again...
			c = "";

			// Everything without spaces...
		}
	}

	// The last part not treated has also to be addedm if any...
	if (c != "")
	{
		std::string tC = MCHEmul::trim (c);
		std::string uTC = MCHEmul::upper (tC);
		result.emplace_back (((std::find (_TOKENS.begin (), _TOKENS.end (), uTC)) == _TOKENS.end ()) 
			? tC : uTC); // It is not the same whether it a token or it is not...
	}

	// ...and always the end of line is added...
	result.push_back ("\n");
	
	return (result);
}

// ---
MCHEmul::Strings SINCLAIR::KeystrokeTypeIO::generateKeystrokeForToken (const std::string& t) const
{
	// If the keystroke is not found, an enrror is kept into the log and nothing is returned...
	MCHEmul::Strings result;
	std::map <std::string, MCHEmul::Strings>::const_iterator i = _KEYSTROKES.find (t);
	if (i == _KEYSTROKES.end ())
	{
		// There is no a specific keystroke associated, so the keystrokes are just the keys...
		// The letter could be in uppercase...
		// ...and a SHIFT could be needed before...
		for (const auto& i : t)
			result.push_back ((i == ' ') 
				// A space is not considering as a token (so it is not in the transaltion list), 
				// but requeries special key for typewriter...
				? "SPACE" 
				: (std::isupper (i) ? "LSHIFT+" : "") + std::string (1, i));
	}
	else
		result = (*i).second;

	return (result);
}
