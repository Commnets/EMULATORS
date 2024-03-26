#include <COMMODORE/FileIO.hpp>

// ---
MCHEmul::ExtendedDataMemoryBlocks COMMODORE::RawFileData::asMemoryBlocks () const
{
	MCHEmul::ExtendedDataMemoryBlocks result;

	result._name = std::string ("COMMTYNETS");
	result._attributes = MCHEmul::Attributes ();
	for (auto& i : _blocks)
	{ 
		MCHEmul::DataMemoryBlock dMB (MCHEmul::Address (), i._bytes.bytes ()); 
		dMB.setName (i._name); // No more attributes per block...
		result._data.emplace_back (std::move (dMB));
	}

	return (result);
}

// ---
bool COMMODORE::RawFileTypeIO::canRead (const std::string& fN) const
{
	if (fN.size () < 4 || 
		MCHEmul::upper (fN.substr (fN.length () - 3, 3)) != "RAW")
		return (false); // The right extension...

	std::ifstream f (fN, std::ios::in || std::ios::binary);
	if (!f)
		return (false); // Possible to be open...

	f.seekg (0, std::ios::end);
	std::streamoff s = f.tellg ();
	f.close ();
	if (s < (std::streamoff) (0x14 /** Header = 16 bytes with the signature + 4 bytes with the number of blocks. */))
		return (false); // The length of the file is less than expected...

	return (true);
}

// ---
MCHEmul::FileData* COMMODORE::RawFileTypeIO::readFile (const std::string& fN, bool bE) const
{
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (nullptr); // Possible to be open... At this point it shouldn't happen but just in case...

	char data [256] = { };
	MCHEmul::FileData* result = new COMMODORE::RawFileData;
	COMMODORE::RawFileData* tap = 
		static_cast <COMMODORE::RawFileData*> (result); // To better manipulation...

	// The header
	// First of all, the name of the data...
	f.read (data, 16); data [16] = 0; // End of char...
	tap -> _signature = std::string (data);
	// The number of blocks the data is made up of...
	f.read (data, 4);
	tap -> _dataBlocks = (unsigned int) 
		(data [3] << 24) + (data [2] << 16) + (data [1] << 8) + data [0];

	// Now to read the blocks info...
	for (unsigned int i = 0; i < tap -> _dataBlocks; i++)
	{
		COMMODORE::RawFileData::Block dB;

		// The header of the block
		// First of all, the size of this block
		f.read (data, 4);
		dB._dataSize = (unsigned int)
			((data [3] << 24) + (data [2] << 16) + (data [1] << 8) + data [0]);
		
		// ..the name of the block, made up of 16 chars...
		f.read (data, 16); data [16] = 0; // End of char...
		dB._name = std::string (data);

		// ...and the data
		if (dB._dataSize > 0)
		{
			char* romData = new char [(size_t) dB._dataSize];
			f.read (romData, (std::streamsize) dB._dataSize);
			std::vector <MCHEmul::UByte> romBytes;
			for (size_t j = 0; j < (size_t) dB._dataSize; 
				romBytes.emplace_back (romData [j++]));
			dB._bytes = MCHEmul::UBytes (romBytes);
			delete [] romData;
		}

		tap -> _blocks.emplace_back (std::move (dB));
	}

	f.close ();

	return (result);
}

// ---
bool COMMODORE::RawFileTypeIO::writeFile (MCHEmul::FileData* fD, const std::string& fN, bool bE) const
{
	COMMODORE::RawFileData* tap = 
		static_cast <COMMODORE::RawFileData*> (fD); // To better manipulation...
	if (tap == nullptr)
		return (false);

	std::ofstream f (fN, std::ios::out | std::ios::binary);
	if (!f)
		return (false); // Possible to be open...

	char data [256] = { };

	// The signature...
	size_t i = 0;
	for (; i < tap -> _signature.size () && i < 15; i++)
		data [i] = tap -> _signature [i];
	for (; i < 16; data [i++] = 0);
	f.write (data, 16);

	// The number of blocks...
	data [3] = (char) ((tap -> _dataBlocks & 0xff000000) >> 24);
	data [2] = (char) ((tap -> _dataBlocks & 0x00ff0000) >> 16);
	data [1] = (char) ((tap -> _dataBlocks & 0x0000ff00) >> 8);
	data [0] = (char)  (tap -> _dataBlocks & 0x000000ff);
	f.write (data, 4);

	// Every block...
	for (auto& i : tap -> _blocks)
	{
		// The size of the block in bytes
		// including 16 positions with the name of the block...
		data [3] = (char) ((i._dataSize & 0xff000000) >> 24);
		data [2] = (char) ((i._dataSize & 0x00ff0000) >> 16);
		data [1] = (char) ((i._dataSize & 0x0000ff00) >> 8);
		data [0] = (char)  (i._dataSize & 0x000000ff);
		f.write (data, 4);

		// ...the name of the block...
		size_t j = 0;
		for (; j < i._name.size () && j < 15; j++)
			data [j] = i._name [j];
		for (; j < 16; data [j++] = 0);
		f.write (data, 16);

		// ...and finally the block data...
		char* prgData = new char [(size_t) i._dataSize];
		for (size_t j = 0; j < (size_t) i._dataSize; j++)
			prgData [j] = i._bytes.bytes ()[j].value ();
		f.write (prgData, (std::streamsize) i._dataSize);
		delete [] prgData;
	}

	f.close ();

	return (true);
}

// ---
MCHEmul::ExtendedDataMemoryBlocks COMMODORE::TAPFileData::asMemoryBlocks() const
{
	MCHEmul::ExtendedDataMemoryBlocks result;

	result._name = _signature;
	MCHEmul::Attributes attrs;
	attrs ["VERSION"]	= std::to_string ((unsigned int) _version);
	attrs ["CVERSION"]	= std::to_string ((unsigned int) _computerVersion);
	attrs ["VVERSION"]	= std::to_string ((unsigned int) _videoVersion);
	result._attributes	= std::move (attrs); // It is not longer valid...
	result._data = { MCHEmul::DataMemoryBlock (MCHEmul::Address (), _bytes.bytes ()) }; // Just the data...

	return (result);
}

// ---
bool COMMODORE::TAPFileTypeIO::canRead (const std::string& fN) const
{
	if (fN.size () < 4 || 
		MCHEmul::upper (fN.substr (fN.length () - 3, 3)) != "TAP")
		return (false); // The right extension...

	std::ifstream f (fN, std::ios::in || std::ios::binary);
	if (!f)
		return (false); // Possible to be open...

	f.seekg (0, std::ios::end);
	std::streamoff s = (std::streamoff) f.tellg ();
	f.close ();
	if (s < (std::streamoff) (0x14 /** Header. */ + 0x1 /** At least one byte. */))
		return (false); // The length of the file is less than expected...

	return (true);
}

// ---
MCHEmul::FileData* COMMODORE::TAPFileTypeIO::readFile (const std::string& fN, bool bE) const
{
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (nullptr); // Possible to be open... At this point it shouldn't happen but just in case...

	char data [256] = { };
	MCHEmul::FileData* result = new COMMODORE::TAPFileData;
	COMMODORE::TAPFileData* tap = 
		static_cast <COMMODORE::TAPFileData*> (result); // To better manipulation...

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
	tap -> _dataSize = (unsigned int)
		((data [3] << 24) + (data [2] << 16) + (data [1] << 8) + data [0]);
	f.read (data, (std::streamsize) tap -> _dataSize); 

	// The data...
	unsigned int dSize = tap -> _dataSize - 20;
	if (dSize != 0)
	{
		char* romData = new char [(size_t) dSize];
		f.read (romData, (std::streamsize) dSize);
		std::vector <MCHEmul::UByte> romBytes;
		for (size_t i = 0; i < (size_t) dSize; 
			romBytes.emplace_back (romData [i++]));
		tap -> _bytes = MCHEmul::UBytes (romBytes);
		delete [] romData;
	}
	else
		tap -> _bytes = { };

	f.close ();

	return (result);
}

// ---
bool COMMODORE::TAPFileTypeIO::writeFile (MCHEmul::FileData* fD, const std::string& fN, bool bE) const
{
	COMMODORE::TAPFileData* tap = 
		static_cast <COMMODORE::TAPFileData*> (fD); // To better manipulation...
	if (tap == nullptr)
		return (false);

	std::ofstream f (fN, std::ios::out | std::ios::binary);
	if (!f)
		return (false); // Possible to be open...

	char data [256] = { };

	// The signature...
	size_t i = 0;
	for (; i < tap -> _signature.size () && i < 11; i++)
		data [i] = tap -> _signature [i];
	for (; i < 12; data [i++] = 0)
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
	char* prgData = new char [(size_t) tap -> _dataSize];
	for (size_t i = 0; i < (size_t) tap -> _dataSize; i++)
		prgData [i] = tap -> _bytes.bytes ()[i].value ();
	f.write (prgData, (std::streamsize) tap -> _dataSize);
	delete [] prgData;

	f.close ();

	return (true);
}

// ---
MCHEmul::ExtendedDataMemoryBlocks COMMODORE::T64FileData::asMemoryBlocks () const
{
	// TODO

	return (MCHEmul::ExtendedDataMemoryBlocks ());
}

// ---
bool COMMODORE::T64FileTypeIO::canRead (const std::string& fN) const
{
	if (fN.size () < 4 || 
		MCHEmul::upper (fN.substr (fN.length () - 3, 3)) != "T64")
		return (false); // The right extension...

	std::ifstream f (fN, std::ios::in || std::ios::binary);
	if (!f)
		return (false); // Possible to be open...

	f.seekg (0, std::ios::end);
	std::streamoff s = (std::streamoff) f.tellg ();
	f.close ();
	if (s < (std::streamoff) (0x64 /** Header. */ + 0x32 /** Tap Header. */ + 0x1 /** At least one byte. */))
		return (false); // The length of the file is less than expected...

	return (true);
}

// ---
MCHEmul::FileData* COMMODORE::T64FileTypeIO::readFile (const std::string& fN, bool bE) const
{
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (nullptr); // Possible to be open... At this point it shouldn't happen but just in case...

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
	rT64 -> _tapeRecord._version = (data [1] << 8) + data [0];
	f.read (data, 2);
	rT64 -> _tapeRecord._entries = (data [1] << 8) + data [0];
	f.read (data, 2);
	rT64 -> _tapeRecord._usedEntries = (data [1] << 8) + data [0];
	f.read (data, 2); // 2 bytes free...
	f.read (data, 24); data [24] = 0;
	rT64 -> _tapeRecord._userDescriptor = std::string (data);

	// Per entry in the file, tipically it will be just one...
	for (unsigned int i = 0; i < rT64 -> _tapeRecord._entries; i++)
	{
		COMMODORE::T64FileData::FileRecord fR;

		// The header...
		f.read (data, 1);
		fR._entryType = (COMMODORE::T64FileData::FileRecord::EntryType) data [0];
		f.read (data, 1);
		fR._fileType = data [0];
		f.read (data, 2);
		fR._startLoadAddress = MCHEmul::Address ({ data [0], data [1] }, true);
		f.read (data, 2);
		fR._endLoadAddress = MCHEmul::Address ({ data [0], data [1] }, true);
		f.read (data, 2); // 2 bytes free...
		f.read (data, 4);
		fR._offset = (unsigned int) 
			((data [3] << 24) + (data [2] << 16) + (data [1] << 8) + data [0]);
		f.read (data, 4); // 4 bytes free...
		f.read (data, 24); data [24] = 0;
		fR._fileName = std::string (data);

		rT64 -> _fileRecords.emplace_back (std::move (fR));
	}

	// The data...
	unsigned int dSize = (unsigned int) size - 
		64 - (32 * (unsigned int) rT64 -> _tapeRecord._entries);
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
bool COMMODORE::CRTFileTypeIO::canRead (const std::string& fN) const
{
	if (fN.size () < 4 || 
		MCHEmul::upper (fN.substr (fN.length () - 3, 3)) != "CRT")
		return (false); // The right extension...

	std::ifstream f (fN, std::ios::in || std::ios::binary);
	if (!f)
		return (false); // Possible to be open...

	f.seekg (0, std::ios::end);
	std::streamoff s = (std::streamoff) f.tellg ();
	f.close ();
	if (s < (std::streamoff) (0x40 /** Header. */ + 0x10 /** Chip Header. */ + 0x1000 /** 4k = Minimum data length. */))
		return (false); // The length of the file is less than expected...

	return (true);
}

// ---
MCHEmul::FileData* COMMODORE::CRTFileTypeIO::readFile (const std::string& fN, bool bE) const
{
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (nullptr); // Possible to be open... At this point it shouldn't happen but just in case...

	char data [256] = { };
	MCHEmul::FileData* result = new COMMODORE::CRTFileData;
	COMMODORE::CRTFileData* rCRT = 
		static_cast <COMMODORE::CRTFileData*> (result); // To better manipulation...

	// The header...
	f.read (data, 16); data [16] = 0; // End of char...
	rCRT -> _signature = std::string (data);
	f.read (data, 4);
	rCRT -> _headerSize = 
		(unsigned int) ((data [0] << 24) + (data [1] << 16) + (data [2] << 8) + data [3]);
	f.read (data, 2);
	rCRT -> _cartridgeVersion = (unsigned short) ((data [0] << 8) + data [1]);
	f.read (data, 2);
	rCRT -> _cartridgeType = (unsigned short) ((data [0] << 8) + data [1]);
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
		cD._packageSize = (unsigned int) 
			((data [0] << 24) + (data [1] << 16) + (data [2] << 8) + data [3]);
		f.read (data, 2);
		cD._type = (unsigned short) ((data [0] << 8) + data [1]);
		f.read (data, 2);
		cD._bankNumber = (unsigned short) ((data [0] << 8) + data [1]);
		f.read (data, 2);
		cD._startingLoadAddress = MCHEmul::Address ({ data [0], data [1] }, true);
		f.read (data, 2);
		cD._romSize = (unsigned short) ((data [0] << 8) + data [1]);
		
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
