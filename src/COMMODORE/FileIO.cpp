#include <COMMODORE/FileIO.hpp>

// ---
// How the special keys in the keyboard of the C64 are generated using a standard PC UK keyboard
// Really the keys assigned are the equivalents to the ones used in COMMODORE::InputIOSystem,
// that reflects the location of the keys in the UK keyboard.
// Typewriter class interprets the keys in the opposite way and assign a the SDL_KEYCode 
const std::map <char, MCHEmul::Strings> COMMODORE::KeystrokeTypeIO::_DEFAULTSPECIALKEYS = {
		{ '\n',	{ "RETURN" } },
		{ ' ',	{ "SPACE" } },
		{ '-',	{ "-" } },
		{ '|',	{ "LSHIFT+-" } },
		{ '=',	{ "=" } },
		{ '+',	{ "[" } },
		{ '*',	{ "]" } },
		{ ';',	{ ";" } },
		{ ']',	{ "LSHIFT+;" } },
		{ ':',	{ "´" } },
		{ '[',	{ "LSHIFT+´" } },
		{ '@',	{ "\\" } },
		{ ',',	{ "," } },
		{ '<',	{ "LSHIFT+," } },
		{ '.',	{ "." } },
		{ '>',	{ "LSHIFT+." } },
		{ '/',	{ "/" } },
		{ '?',	{ "LSHIFT+/" } },
		{ '£',	{ "`" } },	
		{ '!',	{ "LSHIFT+1" } },
		{ '"',	{ "LSHIFT+2" } },
		{ '#',	{ "LSHIFT+3" } },
		{ '$',	{ "LSHIFT+4" } },
		{ '%',	{ "LSHIFT+5" } },
		{ '&',	{ "LSHIFT+6" } },
		{ '´',	{ "LSHIFT+7" } },
		{ '(',	{ "LSHIFT+8" } },
		{ ')',	{ "LSHIFT+9" } }
	};

// ---
MCHEmul::ExtendedDataMemoryBlocks COMMODORE::TAPFileData::asMemoryBlocks() const
{
	MCHEmul::ExtendedDataMemoryBlocks result;

	// The common part of the result...
	result._name = _signature;
	MCHEmul::Attributes attrs;
	attrs ["VERSION"]	= std::to_string ((unsigned int) _version);
	attrs ["CVERSION"]	= std::to_string ((unsigned int) _computerVersion);
	attrs ["VVERSION"]	= std::to_string ((unsigned int) _videoVersion);
	for (const auto& i : _attributes)
		attrs [i.first] = i.second; // Copy the attributes (none can have the same name than the ones above)...
	result._attributes	= std::move (attrs); // It is not longer valid...

	// The data that is behind...
	static const std::vector <MCHEmul::UByte> _HEADERPATTERN { 0x2e, 0x2f, 0x30 };
	auto isInHeaderPattern = 
		[&](const MCHEmul::UByte& b) -> bool
		{ return (std::find (_HEADERPATTERN.begin (), _HEADERPATTERN.end (), b) != _HEADERPATTERN.end ()); };
	enum class FileZone { _HEADER, _BLOCK };
	size_t i = 0, iP = 0, iB = 0, nHE = 0, nBE = 0;
	for (i = 0; i < _bytes.bytes ().size () && !isInHeaderPattern (_bytes [i]); i++); // Find the first header character...
	FileZone zone = FileZone::_HEADER;
	for (; i < _bytes.bytes ().size (); i++)
	{
		switch (zone)
		{
			// While the cursor is in the header...
			case FileZone::_HEADER:
				{
					// The header lasts until the first no header character is found... 
					// ...and at least 0x3000 bytes of that type are...
					if (isInHeaderPattern (_bytes [i]))
						nHE++;
					else
					{
						if (nHE >= 0x3000)
						{
							// A new blocks starts...
							zone = FileZone::_BLOCK;
							// The block starts from the current position...
							iB = i;
							// The new block has at least this byte found...
							nBE = 1; 
							// Restart the header counter...
							nHE = 0;

						}
					}
				}

				break;

			// While the cursor is in the data block (supposed to be)...
			case FileZone::_BLOCK:
				{
					if (!isInHeaderPattern (_bytes [i]))
					{
						// The header is not found, so the block continues...
						nHE = 0; 
						// The number of bytes in the block is increased...
						nBE++; 
					}
					else
					{
						if (++nHE >= 0x3000)
						{
							// A new header is found, so the program definition was found...
							// ...but some time ago, and now has to be created!
							result._data.emplace_back (std::move (MCHEmul::DataMemoryBlock (
								MCHEmul::Address (), std::vector <MCHEmul::UByte> 
									(_bytes.bytes ().begin () + iP, _bytes.bytes ().begin () + (i - nHE)))));

							// It is time to start with another program, but from the beginning...
							zone = FileZone::_HEADER; 
							// That was some time ago....
							iP = i - nHE;
							// It is suppossed that the program will start the same position...
							i = iP; 
							// And with the counters reseted...
							nHE = 0; nBE = 0;
						}
					}
				}

				break;
		}
	}

	// Add a last block if any...
	result._data.emplace_back (std::move (MCHEmul::DataMemoryBlock (
		MCHEmul::Address (), std::vector <MCHEmul::UByte> 
			(_bytes.bytes ().begin () + iP, _bytes.bytes ().begin () + i))));

	return (result);
}

// ---
void COMMODORE::TAPFileData::addHeaderFromMemoryBlock (const MCHEmul::ExtendedDataMemoryBlocks& dMB)
{
	_signature = dMB._name;
	_version = (unsigned char) MCHEmul::getAttributeAsInt ("VERSION", dMB._attributes);
	_computerVersion = (COMMODORE::TAPFileData::ComputerVersion) 
		MCHEmul::getAttributeAsInt ("CVERSION", dMB._attributes);
	_videoVersion = (COMMODORE::TAPFileData::VideoVersion) 
		MCHEmul::getAttributeAsInt ("VVERSION", dMB._attributes);
}

// ---
void COMMODORE::TAPFileData::addDataBlockFromMemoryBlock (const MCHEmul::ExtendedDataMemoryBlocks& dMB)
{
	_dataSize = 0;
	for (size_t i = 0; i < dMB._data.size (); 
		_dataSize += (unsigned int) dMB._data [i++].size ());
	std::vector <MCHEmul::UByte> by;
	for (size_t i = 0; i < dMB._data.size (); i++)
		by.insert (by.end (), dMB._data [i].bytes ().begin (), dMB._data [i].bytes ().end ());
	_bytes = by;
}

// ---
bool COMMODORE::TAPFileTypeIO::canRead (const std::string& fN) const
{
	// Extension?
	size_t pp = fN.find_last_of ('.');
	if (pp == std::string::npos || pp == fN.length ())
		return (false); // ...no

	// The right extension?
	std::string ext = MCHEmul::upper (fN.substr (pp + 1));
	if (ext != "TAP")
		return (false); // ...no

	// Possible to open?
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (false); // ...no

	// The right length?
	f.seekg (0, std::ios::end);
	std::streamoff s = (std::streamoff) f.tellg ();
	f.close ();
	if (s < (std::streamoff) (0x14 /** Header only is needed if length is equal to 0 (checked later) */))
		return (false); // ...no. The length of the file is less than expected. It has to be minimum 0x15!

	return (true);
}

// ---
MCHEmul::FileData* COMMODORE::TAPFileTypeIO::readFile (const std::string& fN, bool bE) const
{
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (nullptr); // Impossible to be open... 
						  // At this point it shouldn't happen but just in case...

	char data [256] = { };
	MCHEmul::FileData* result = new COMMODORE::TAPFileData;
	COMMODORE::TAPFileData* tap = 
		static_cast <COMMODORE::TAPFileData*> (result); // To better manipulation...

	// The name of the file is in an attribute...
	tap -> _attributes ["FNAME"] = fN;

	// The header
	f.read (data, 12); data [12] = 0; // End of char...
	tap -> _signature = std::string (data);
	f.read (data, 1);
	tap -> _version = data [0];
	f.read (data, 1);
	tap -> _computerVersion = (COMMODORE::TAPFileData::ComputerVersion) data [0];
	f.read (data, 1);
	tap -> _videoVersion = (COMMODORE::TAPFileData::VideoVersion) data [0];
	f.read (data, 1); // 1 byte free for future expansion...
	f.read (data, 4);
	tap -> _dataSize = (unsigned int) (((unsigned char) data [3] << 24) + 
		((unsigned char) data [2] << 16) + ((unsigned char) data [1] << 8) + ((unsigned char) data [0]));

	// The data...
	bool e = false;
	if (tap -> _dataSize != 0)
	{
		char* romData = new char [(size_t) tap -> _dataSize];
		f.read (romData, (std::streamsize) tap -> _dataSize);
		e = !f; // The system tried to read more bytes than available, and this is not possible...
		std::vector <MCHEmul::UByte> romBytes;
		for (size_t i = 0; i < (size_t) tap -> _dataSize; 
			romBytes.emplace_back (romData [i++]));
		tap -> _bytes = MCHEmul::UBytes (romBytes);
		delete [] romData;
	}
	else
		tap -> _bytes = { };

	f.close ();

	// If there was a mistake reading the content of the file
	// the file can not be created at all!
	if (e)
	{
		delete result;

		result = nullptr;
	}

	return (result);
}

// ---
bool COMMODORE::TAPFileTypeIO::writeFile (MCHEmul::FileData* fD, const std::string& fN, bool bE) const
{
	COMMODORE::TAPFileData* tap = 
		dynamic_cast <COMMODORE::TAPFileData*> (fD); // To better manipulation...
	if (tap == nullptr)
		return (false); // it is not really a tap structure!

	// There might not be name of the file,
	// It it were the case, the name defined in the data file should be taken instead
	// and if there were not event that, a mistake is generated!
	std::string lFN = fN;
	if (lFN == "")
	{
		MCHEmul::Attributes::const_iterator p = tap -> _attributes.find ("FNAME");
		if (p == tap -> _attributes.end ())
			return (false);
		else
			lFN = (*p).second; // The name of the file to write...
	}

	// Open the file, and keeps its name just for the next time, if any!
	std::ofstream f (tap -> _attributes ["FNAME"] = lFN, 
		std::ios::out | std::ios::binary);
	if (!f)
		return (false); // Impossible to be opened...

	char data [256] = { };

	// The signature...
	size_t i = 0;
	for (; i < tap -> _signature.size () && i < 12; i++)
		data [i] = tap -> _signature [i];
	for (; i < 12; data [i++] = 0);
	f.write (data, 12);

	// The versions...
	data [0] = (char) tap -> _version;
	f.write (data, 1);
	// The computer version
	data [0] = (char) tap -> _computerVersion;
	f.write (data, 1);
	// The video version
	data [0] = (char) tap -> _videoVersion;
	f.write (data, 1);

	// 1 byte free for future expansion...
	data [0] = 0;
	f.write (data, 1); 

	// The data size
	data [3] = (char) ((tap -> _dataSize & 0xff000000) >> 24);
	data [2] = (char) ((tap -> _dataSize & 0x00ff0000) >> 16);
	data [1] = (char) ((tap -> _dataSize & 0x0000ff00) >> 8);
	data [0] = (char) ((tap -> _dataSize & 0x000000ff));
	f.write (data, 4);

	// The data
	if (tap -> _dataSize != 0)
	{
		char* prgData = new char [(size_t) tap -> _dataSize];
		for (size_t i = 0; i < (size_t) tap -> _dataSize; i++)
			prgData [i] = tap -> _bytes.bytes ()[i].value ();
		f.write (prgData, (std::streamsize) tap -> _dataSize);
		delete [] prgData;
	}

	f.close ();

	return (true);
}

// ---
MCHEmul::ExtendedDataMemoryBlocks COMMODORE::PRGFileData::asMemoryBlocks () const
{
	MCHEmul::ExtendedDataMemoryBlocks result;

	MCHEmul::DataMemoryBlock mB (_address, _bytes.bytes ());
	mB.setAttribute ("TYPE", "1");
	result._data = { mB }; // Just the data...

	return (result);
}

// ---
bool COMMODORE::PRGFileTypeIO::canRead (const std::string& fN) const
{
	// Extension?
	size_t pp = fN.find_last_of ('.');
	if (pp == std::string::npos || pp == fN.length ())
		return (false); // ...no

	// The right extension?
	std::string ext = MCHEmul::upper (fN.substr (pp + 1));
	if (ext != "PRG")
		return (false); // ...no

	// Possible to open?
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (false); // ...no

	// The right length?
	f.seekg (0, std::ios::end);
	std::streamoff s = (std::streamoff) f.tellg ();
	f.close ();
	if (s < (std::streamoff) (0x02 /** Header = start address */ + 0x1 /** At least one byte. */))
		return (false); // ...no. The length of the file is less than expected. It has to be minimum 0x03!

	return (true);
}

// ---
MCHEmul::FileData* COMMODORE::PRGFileTypeIO::readFile (const std::string& fN, bool bE) const
{
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (nullptr); // Impossible to be open... 
						  // At this point it shouldn't happen but just in case...

	char data [256] = { };
	MCHEmul::FileData* result = new COMMODORE::PRGFileData;
	COMMODORE::PRGFileData* prg = 
		static_cast <COMMODORE::PRGFileData*> (result); // To better manipulation...

	// The length of the file.
	f.seekg (0, std::ios::end);
	std::streamoff s = (std::streamoff) f.tellg ();
	f.seekg (0, std::ios::beg);

	// The header = starting address
	f.read (data, 2);
	prg -> _address = MCHEmul::Address ({ (unsigned char) data [0], (unsigned char) data [1] }, false);
	// The data...
	char* romData = new char [(size_t) (s - 2)];
	f.read (romData, (std::streamsize) (s - 2));
	std::vector <MCHEmul::UByte> romBytes;
	for (size_t i = 0; i < (size_t) (s - 2);
		romBytes.emplace_back (romData [i++]));
	prg -> _bytes = MCHEmul::UBytes (romBytes);
	delete [] romData;

	f.close ();

	return (result);
}

// ---
MCHEmul::ExtendedDataMemoryBlocks COMMODORE::T64FileData::asMemoryBlocks () const
{
	MCHEmul::ExtendedDataMemoryBlocks result;

	result._name = _tapeRecord._userDescriptor;
	result._attributes ["VERSION"] = std::to_string (_tapeRecord._version);
	result._attributes ["ENTRIES"] = std::to_string (_tapeRecord._usedEntries);

	for (unsigned short i = 0; i < _tapeRecord._usedEntries; i++)
	{
		MCHEmul::DataMemoryBlock mB (_fileRecords [i]._startLoadAddress, 
			_content.subset ((size_t) (_fileRecords [i]._offset - 0x40 - (0x20 * _tapeRecord._usedEntries)), 
				(size_t) (_fileRecords [i]._endLoadAddress - _fileRecords [i]._startLoadAddress + 1)).bytes ());
		mB.setName (_fileRecords [i]._fileName);
		mB.setAttribute ("TYPE", std::to_string (_fileRecords [i]._entryType)); // Important later in header verification e.g.
		result._data.emplace_back (std::move (mB));
	}

	return (result);
}

// ---
std::string COMMODORE::T64FileData::asString () const
{ 
	std::string result;

	bool f = true;
	for (const auto& i : _fileRecords)
	{
		result += (!f ? "," : "") + i._fileName;
		f = false;
	}

	return (_tapeRecord._userDescriptor + ":" + result +
		" (Version:" + std::to_string (_tapeRecord._version) + ")"); 
}

// ---
bool COMMODORE::T64FileTypeIO::canRead (const std::string& fN) const
{
	// Extension?
	size_t pp = fN.find_last_of ('.');
	if (pp == std::string::npos || pp == fN.length ())
		return (false); // ...no

	// The right extension?
	std::string ext = MCHEmul::upper (fN.substr (pp + 1));
	if (ext != "T64")
		return (false); // ...no

	// Possible to open?
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (false); // ...no

	// Has it the right (minimum) length?
	f.seekg (0, std::ios::end);
	std::streamoff s = (std::streamoff) f.tellg ();
	f.close ();
	if (s < (std::streamoff) (0x40 /** 64 bytes = Header. */ + 
			0x20 /** 32 bytes = At east 1 File Record. */ + 0x01 /** At least one byte. */))
		return (false); // ...no. The length in the file has to be at least 0x61 (= 97)

	return (true);
}

// ---
MCHEmul::FileData* COMMODORE::T64FileTypeIO::readFile (const std::string& fN, bool bE) const
{
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (nullptr); // Impossible to be open... 
						  // At this point it shouldn't happen but just in case...

	char data [256] = { };
	MCHEmul::FileData* result = new COMMODORE::T64FileData;
	COMMODORE::T64FileData* rT64 = 
		static_cast <COMMODORE::T64FileData*> (result); // To better manipulation...

	f.seekg (0, std::ios::end);
	std::streamoff size = (std::streamoff) f.tellg ();
	f.seekg (0, std::ios::beg);

	// The header
	f.read (data, 32); data [32] = 0; // End of char...
	rT64 -> _tapeRecord._descriptor = std::string (data);
	f.read (data, 2);
	rT64 -> _tapeRecord._version = (unsigned short) ((unsigned char) data [1] << 8) + ((unsigned char) data [0]);
	f.read (data, 2);
	rT64 -> _tapeRecord._entries = (unsigned short) ((unsigned char) data [1] << 8) + ((unsigned char) data [0]);
	f.read (data, 2);
	rT64 -> _tapeRecord._usedEntries = (unsigned short) (((unsigned char) data [1] << 8) + ((unsigned char) data [0]));
	if (rT64 -> _tapeRecord._usedEntries == 0) 
		rT64->_tapeRecord._usedEntries = 1; // It cannot be 0, 1 at least...
	f.read (data, 2); // 2 bytes free...
	f.read (data, 24); data [24] = 0;
	rT64 -> _tapeRecord._userDescriptor = std::string (data);

	// Per entry in the file, tipically it will be just one...
	for (unsigned int i = 0; i < rT64 -> _tapeRecord._usedEntries; i++)
	{
		COMMODORE::T64FileData::FileRecord fR;

		// The header...
		f.read (data, 1);
		fR._entryType = (COMMODORE::T64FileData::FileRecord::EntryType) data [0];
		f.read (data, 1);
		fR._fileType = data [0];
		f.read (data, 2);
		fR._startLoadAddress = MCHEmul::Address ({ (unsigned char) data [0], (unsigned char) data [1] }, false);
		f.read (data, 2);
		fR._endLoadAddress = MCHEmul::Address ({ (unsigned char) data [0], (unsigned char) data [1] }, false);
		f.read (data, 2); // 2 bytes free...
		f.read (data, 4);
		fR._offset = (unsigned int) (((unsigned char) data [3] << 24) + 
			((unsigned char) data [2] << 16) + ((unsigned char) data [1] << 8) + ((unsigned char) data [0]));
		f.read (data, 4); // 4 bytes free...
		f.read (data, 16); data [16] = 0;
		fR._fileName = std::string (data);

		rT64 -> _fileRecords.emplace_back (std::move (fR));
	}

	// The data...
	unsigned int dSize = (unsigned int) size - 
		64 - (32 * (unsigned int) rT64 -> _tapeRecord._usedEntries) + 1;
	char* romData = new char [dSize];
	f.read (romData, (std::streamsize) dSize);
	std::vector <MCHEmul::UByte> romBytes;
	for (size_t i = 0; i < dSize; romBytes.emplace_back (romData [i++]));
	rT64 -> _content = MCHEmul::UBytes (romBytes);
	delete [] romData;

	f.close ();

	return (result);
}

// ---
COMMODORE::D64FileData::TrackInfo COMMODORE::D64FileData::TrackInfo::trackInfo (unsigned tN)
{ 
	if (tN >= 1 && tN <= 17) 
		return (D64FileData::TrackInfo 
			(tN, 21, MCHEmul::Address (3, ((tN - 1) * 21) * 256)));
	else 
	if (tN >= 18 && tN <= 24) 
		return (D64FileData::TrackInfo 
			(tN, 19, MCHEmul::Address (3, 
				((17 * 21 /** The first 17 tracks have 21 sectors each */) + 
				 ((tN - 18) * 19 /** The rest from 18 onwards have 19 each. */)) * 256)));
	else
	if (tN >= 25 && tN <= 30) 
		return (D64FileData::TrackInfo 
			(tN, 18, MCHEmul::Address (3, 
				((17 * 21) + 
				 (7 * 19 /** From 18 to 24, that's it = 7 more, have 19 sectors each. */) +
				 ((tN - 25) * 18 /** The rest from 25 onwards have 18 each. */)) * 256)));
	else 
	if (tN >= 31 && tN <= 42) 
		return (D64FileData::TrackInfo 
			(tN, 17, MCHEmul::Address (3, 
				((17 * 21) + 
				 (7 * 19) + 
				 (6 * 18 /** From 25 to 30, that's it = 6 more, have 18 sectors each. */) +
				 ((tN - 31) * 17) /** The rest from 31 onwards have 17 each. */) * 256)));
	else 
		_LOG ("D64FileData::TrackInfo::trackInfo: Invalid track number: " + std::to_string (tN));

	// This method should not arrive here...
	// It would mean that the track number is invalid...
	// So, return a default value, but assert (in TrackInfo constructor) shold declare the problem...
	return (D64FileData::TrackInfo (0, 0, MCHEmul::Address ()));
}

// ---
void COMMODORE::D64FileData::Track18Data::setSectorData (unsigned char sn, const MCHEmul::UBytes& data)
{ 
	COMMODORE::D64FileData::TrackData::setSectorData (sn, data); 

	_entriesCreated = false;
}

// ---
const COMMODORE::D64FileData::DirectoryEntries& 
	COMMODORE::D64FileData::Track18Data::entries () const
{ 
	if (!_entriesCreated) 
	{
		createEntries ();

		_entriesCreated = true;
	}

	return (_entries);
}

// ---
std::string COMMODORE::D64FileData::Track18Data::asString () const
{ 
	std::string result = "";

	// Just to force the generation if needed....
	const COMMODORE::D64FileData::DirectoryEntries& e = entries (); 
	for (const auto& i : e)
	{
		for (const auto& j : i)
		{
			if (j._fileName == "")
				continue; // No entry...
			result += (result != "" ? "," : "") + j.asString ();
		}
	}

	return (result);
}

// ---
void COMMODORE::D64FileData::Track18Data::createEntries () const
{
	_entries = { };

	// Enty blocks counter...
	size_t eB = 0;
	// Current track & sector...
	unsigned char cT = 18; // The directory is always in track 18... 
	unsigned char cS = 0;  // Starting from the sector 0...
	do
	{
		// Next track & sector...
		unsigned char nT = 
			_sectorsData [cS].bytes () [0].value ();
		unsigned char nS = 
			_sectorsData [cS].bytes () [1].value ();

		switch (cS)
		{
			// Sector 0 is the directory header...
			case 0:
				{
					// TODO
				}
				break;

			// The rest have entries...
			// ...8 entries per sector...
			default:
				{
					_entries [eB++] = getEntriesFromSector (cS);
				}

				break;
		}

		cT = nT; cS = nS;
	} while (cT != 0);
}

// ---
COMMODORE::D64FileData::DirectoryEntriesPerSector 
	COMMODORE::D64FileData::Track18Data::getEntriesFromSector (unsigned char sN) const
{
	// No entries in sector 0...
	// ...just in case they are requested...
	if (sN == 0)
		return (COMMODORE::D64FileData::DirectoryEntriesPerSector { });
		
	COMMODORE::D64FileData::DirectoryEntriesPerSector result;
	// Each sector has 8 entries of 32 bytes each...
	bool end = false;
	for (unsigned char i = 0; i < 8 && !end; i++)
	{
		COMMODORE::D64FileData::DirectoryEntry dE;
	
		dE._fileType = (COMMODORE::D64FileData::DirectoryEntry::FileType) 
			(_sectorsData [sN].bytes () [(i << 5) + 2].value () & 0x0f); // The LSB Nibble...
		dE._startTrack = _sectorsData [sN].bytes () [(i << 5) + 3].value ();
		dE._startSector = _sectorsData [sN].bytes () [(i << 5) + 4].value ();
		dE._fileSizeBlocks = 
			((unsigned short) (_sectorsData [sN].bytes () [(i << 5) + 31].value ()) << 8) + 
			 (unsigned short) (_sectorsData [sN].bytes () [(i << 5) + 30].value ());
		
		dE._fileName = "";
		for (unsigned char j = 0; j < 16; j++)
		{ unsigned char c = 
			 _sectorsData [sN].bytes () [(i << 5) + 5 + j].value ();
			dE._fileName += (std::isalpha (c) ? (char) std::toupper (c) : ' ');	}
		dE._fileName = MCHEmul::trim (dE._fileName);

		// End? or not end and added?...
		if (!(end = (dE._fileName == "")))
			result [i] = std::move (dE);
	}

	return (result);
}

// ---
COMMODORE::D64FileData::D64FileData (unsigned char nT)
	: _numberTracks (nT),
	  _tracksData ()
{ 
	assert (_numberTracks <= 42);

	// Creating all tracks...
	for (unsigned char i = 1; i <= _numberTracks; i++)
		_tracksData.emplace_back ((i == 18) ? new Track18Data  : new TrackData (i));
}

// ---
MCHEmul::ExtendedDataMemoryBlocks COMMODORE::D64FileData::asMemoryBlocks () const
{
	MCHEmul::ExtendedDataMemoryBlocks result;

	for (unsigned char i = 1; i <= _numberTracks; i++)
	{
		for (unsigned char j = 0; 
				j < _tracksData [i - 1] -> _trackInfo._sectors; j++)
		{
			MCHEmul::DataMemoryBlock mB (
				MCHEmul::Address (3, 
					_tracksData [i - 1] -> _trackInfo._offset.value () + ((unsigned int) j * 256)),  
				_tracksData [i - 1] -> getSectorData (j).bytes ());
			mB.setName ("TRACK " + std::to_string ((int) i) + " SECTOR " + std::to_string ((int) j));
			mB.setAttribute ("TRACK", std::to_string ((int) i));
			mB.setAttribute ("SECTOR", std::to_string ((int) j));
			result._data.emplace_back (std::move (mB));
		}
	}

	return (result);
}

// ---
bool COMMODORE::D64FileTypeIO::canRead (const std::string& fN) const
{
	// Extension?
	size_t pp = fN.find_last_of ('.');
	if (pp == std::string::npos || pp == fN.length ())
		return (false); // ...no

	// The right extension?
	std::string ext = MCHEmul::upper (fN.substr (pp + 1));
	if (ext != "D64")
		return (false); // ...no

	// Possible to open?
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (false); // ...no

	// Has it the right (minimum) length?
	f.seekg (0, std::ios::end);
	std::streamoff s = (std::streamoff) f.tellg ();
	f.close ();
	if (s != (std::streamoff) 0x2ab00 && // 35 tracks...
		s != (std::streamoff) 0x2c000 && // 40 tracks...
		s != (std::streamoff) 0x2c800)	 // 42 tracks...
		return (false); // ...this type of format is very very precissed in the length...

	return (true);
}

// ---
MCHEmul::FileData* COMMODORE::D64FileTypeIO::readFile (const std::string& fN, bool bE) const
{
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (nullptr); // Impossible to be open... 
						  // At this point it shouldn't happen but just in case...

	f.seekg (0, std::ios::end);
	std::streamoff s = (std::streamoff) f.tellg ();
	unsigned char nT = 35; // The usual number of tracks...
	if (s == (std::streamoff) 0x2c000) nT = 40; // ...but ir could be different...
	else if (s == (std::streamoff) 0x2c800) nT = 42;
	MCHEmul::FileData* result = new COMMODORE::D64FileData (nT);
	COMMODORE::D64FileData* rD64 = 
		static_cast <COMMODORE::D64FileData*> (result); // To better manipulation...
	f.seekg (0, std::ios::beg);

	// It is time to read the info per block...
	for (unsigned char i = 1; i <= nT; i++)
	{
		for (unsigned char j = 0; 
				j < rD64 -> _tracksData [i - 1] -> _trackInfo._sectors; j++)
		{
			char data [256] = { };
			f.read (data, 256);
			std::vector <MCHEmul::UByte> dtBytes;
			for (size_t k = 0; k < 256; 
				dtBytes.emplace_back (data [k++]));
			rD64 -> _tracksData [i - 1] -> setSectorData (j, MCHEmul::UBytes (dtBytes));
		}
	}

	f.close ();

	return (result);
}

// ---
MCHEmul::ExtendedDataMemoryBlocks COMMODORE::CRTFileData::asMemoryBlocks () const
{
	MCHEmul::ExtendedDataMemoryBlocks result;

	result._name = _name;
	result._attributes ["TYPE"]		= std::to_string (_cartridgeType);
	result._attributes ["VERSION"]	= std::to_string (_cartridgeVersion);
	result._attributes ["_GAME"]	= _GAMEActive ? "YES" : "NO";
	result._attributes ["_EXROM"]	= _EXROMActive ? "YES" : "NO";
	for (const auto& i : _chipsData)
	{
		MCHEmul::DataMemoryBlock mB (i._startingLoadAddress, i._content.bytes ());
		mB.setAttribute ("BANK", std::to_string (i._bankNumber)); // The bank is important depending on the type of cartridge...
		result._data.emplace_back (std::move (mB)); // No more used...
	}

	return (result);
}

// ---
std::string COMMODORE::CRTFileData::asString () const
{ 
	std::string cD;

	size_t ct = 0;
	for (const auto& i : _chipsData)
	{
		cD += ((ct != 0) ? "," : "") + std::string ("$") +
			MCHEmul::removeAll0 (i._startingLoadAddress.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) +
				" " + std::to_string (i._romSize) + " bytes (bank:" + std::to_string (i._bankNumber) + ")";

		ct++;
	}

	return (_name + 
		" (Type:" + std::to_string (_cartridgeType) +
		", EXROM:" + (_EXROMActive ? "YES" : "NO") +
		", GAME:" + (_GAMEActive ? "YES" : "NO") +
		", ULTIMAX:" + ((_EXROMActive && !_GAMEActive) ? "YES" : "NO") +
		", Version:" + std::to_string (_cartridgeVersion) + 
		", Chips:" + cD + ")"); 
}

// ---
bool COMMODORE::CRTFileTypeIO::canRead (const std::string& fN) const
{
	// Extension?
	size_t pp = fN.find_last_of ('.');
	if (pp == std::string::npos || pp == fN.length ())
		return (false); // ...no

	// The right extension?
	std::string ext = MCHEmul::upper (fN.substr (pp + 1));
	if (ext != "CRT")
		return (false); // ...no

	// Can it be opened?
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (false); // ...no

	// Has it the right length?
	f.seekg (0, std::ios::end);
	std::streamoff s = (std::streamoff) f.tellg ();
	f.close ();
	if (s < (std::streamoff) (0x40 /** Header. */ + 0x10 /** Chip Header. */ + 0x1000 /** 4k = Minimum data length. */))
		return (false); // ...no. The length has to be minimum 0x1050 bytes. */

	return (true);
}

// ---
MCHEmul::FileData* COMMODORE::CRTFileTypeIO::readFile (const std::string& fN, bool bE) const
{
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (nullptr); // Impossible to be open... 
						  // At this point it shouldn't happen but just in case...

	char data [256] = { };
	MCHEmul::FileData* result = new COMMODORE::CRTFileData;
	COMMODORE::CRTFileData* rCRT = 
		static_cast <COMMODORE::CRTFileData*> (result); // To better manipulation...

	// The header...
	f.read (data, 16); data [16] = 0; // End of char...
	rCRT -> _signature = std::string (data);
	f.read (data, 4);
	rCRT -> _headerSize = (unsigned int) (((unsigned char) data [0] << 24) + 
		((unsigned char) data [1] << 16) + ((unsigned char) data [2] << 8) + ((unsigned char) data [3]));
	f.read (data, 2);
	rCRT -> _cartridgeVersion = (unsigned short) (((unsigned char) data [0] << 8) + ((unsigned char) data [1]));
	f.read (data, 2);
	rCRT -> _cartridgeType = (unsigned short) (((unsigned char) data [0] << 8) + ((unsigned char) data [1]));
	f.read (data, 1);
	rCRT -> _EXROMActive = (data [0] == 0) ? false : true;  
	f.read (data, 1);
	rCRT -> _GAMEActive = (data [0] == 0) ? false : true;  
	f.read (data, 6); // 6 bytes for future uses...
	f.read (data, 32); data [32] = 0; // End of char...
	rCRT -> _name = std::string (data);

	// The chip info...
	while (!f.eof ())
	{
		COMMODORE::CRTFileData::ChipData cD;

		// The header...
		f.read (data, 4); data [4] = 0; // End of char...
		if (f.eof ()) continue;
		cD._signature = std::string (data);
		f.read (data, 4);
		cD._packageSize = (unsigned int) (((unsigned char) data [0] << 24) + 
			((unsigned char) data [1] << 16) + ((unsigned char) data [2] << 8) + ((unsigned char) data [3]));
		f.read (data, 2);
		cD._type = (unsigned short) (((unsigned char) data [0] << 8) + ((unsigned char) data [1]));
		f.read (data, 2);
		cD._bankNumber = (unsigned short) (((unsigned char) data [0] << 8) + ((unsigned char) data [1]));
		f.read (data, 2);
		cD._startingLoadAddress = MCHEmul::Address ({ data [0], data [1] }, true);
		f.read (data, 2);
		cD._romSize = (unsigned short) (((unsigned char) data [0] << 8) + ((unsigned char) data [1]));
		
		// The data...
		char* romData = new char [(size_t) cD._romSize];
		f.read (romData, (std::streamsize) cD._romSize);
		std::vector <MCHEmul::UByte> romBytes;
		for (size_t i = 0; i < (size_t) cD._romSize; 
			romBytes.emplace_back (romData [i++]));
		cD._content = MCHEmul::UBytes (romBytes);
		delete [] romData;

		rCRT -> _chipsData.emplace_back (std::move (cD) /** No longer valid. */);
	}

	f.close ();

	return (result);
}

// ---
MCHEmul::Strings COMMODORE::KeystrokeTypeIO::generateKeystrokeForToken (const std::string& t) const
{
	// If the token received is just "complex"
	// because the comnbination of the keys was done...
	if (t.length () > 1)
		return (MCHEmul::Strings { t }); // The keystroke is already the token!

	MCHEmul::Strings result;

	std::string ut = MCHEmul::upper (t);
	std::map <char, MCHEmul::Strings>::const_iterator sp;
	if ((sp = _SPECIALKEYS.find (ut [0])) != _SPECIALKEYS.end ()) result = (*sp).second;
	else  result = MCHEmul::Strings { t };

	return (result);
}
