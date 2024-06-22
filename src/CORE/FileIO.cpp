#include <CORE/FileIO.hpp>

// ---
MCHEmul::FileIO::~FileIO ()
{
	for (MCHEmul::FileTypeIO* i : _IOList)
		delete (i);

	for (const auto& i : _fileData)
		delete (i.second);
}

// ---
MCHEmul::FileData* MCHEmul::FileIO::readFile (const std::string& fN, bool bE) const
{
	// If the file has read before, just get the result...
	std::map <std::string, MCHEmul::FileData*>::const_iterator i;
	if ((i = _fileData.find (fN)) != _fileData.end ())
		return ((*i).second);

	// Looks for the right io to read the file...
	MCHEmul::FileTypeIO* fR = nullptr;
	for (MCHEmul::FileTypeIOList::const_iterator i = _IOList.begin (); 
			i != _IOList.end () && fR == nullptr; i++)
		fR = (*i) -> canRead (fN) ? (*i) : nullptr;

	// If none exists, then NODATA is given back...
	if (fR == nullptr)
		return (nullptr);

	// Otherwise the file is read and moved into a temporal variable here...
	MCHEmul::FileData* dt = fR -> readFile (fN, bE);
	if (dt == nullptr) // If nothing is read or it is invalid...
		return (nullptr); // ...then nothing is givend then back...

	// Finally the data is stored and returned...
	return (_fileData [fN] = dt);
}

// ---
bool MCHEmul::FileIO::saveFile (MCHEmul::FileData* fD, const std::string& fN, bool bE) const
{
	if (fD == nullptr)
		return (false); // Not possible...

	// Looks for the right io to write the file...
	MCHEmul::FileTypeIO* fR = nullptr;
	for (MCHEmul::FileTypeIOList::const_iterator i = _IOList.begin (); 
			i != _IOList.end () && fR == nullptr; i++)
		fR = (*i) -> canWrite (fD) ? (*i) : nullptr;

	// If none exists, then NODATA is given back...
	if (fR == nullptr)
		return (false);

	// Otherwise the file is written...
	return (fR -> writeFile (fD, fN, bE));
}

// ---
MCHEmul::ExtendedDataMemoryBlocks MCHEmul::RawFileData::asMemoryBlocks () const
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
bool MCHEmul::RawFileTypeIO::canRead (const std::string& fN) const
{
	// Extension?
	size_t pp = fN.find_last_of ('.');
	if (pp == std::string::npos || pp == fN.length ())
		return (false); // ...no

	// The right extension?
	std::string ext = MCHEmul::upper (fN.substr (pp + 1));
	if (ext != "RAW")
		return (false); // ...no

	// Possible to be opened?
	std::ifstream f (fN, std::ios::in || std::ios::binary);
	if (!f)
		return (false); // ...no

	// The right size of the file?
	f.seekg (0, std::ios::end);
	std::streamoff s = f.tellg ();
	f.close ();
	if (s < (std::streamoff) (0x14 /** Header = 16 bytes with the signature + 4 bytes with the number of blocks. */))
		return (false); // ...no. The length of the file is less than expected. Has to have at least 0x14 bytes...

	return (true);
}

// ---
MCHEmul::FileData* MCHEmul::RawFileTypeIO::readFile (const std::string& fN, bool bE) const
{
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (nullptr); // Impossible to be open... 
						  // At this point it shouldn't happen but just in case...

	char data [256] = { };
	MCHEmul::FileData* result = new MCHEmul::RawFileData;
	MCHEmul::RawFileData* tap = 
		static_cast <MCHEmul::RawFileData*> (result); // To better manipulation...

	// The header
	// First of all, the name of the data...
	f.read (data, 16); data [16] = 0; // End of char...
	tap -> _signature = std::string (data);
	// The number of blocks the data is made up of...
	f.read (data, 4);
	tap -> _dataBlocks = (unsigned int) (((unsigned char) data [3] << 24) + 
		((unsigned char) data [2] << 16) + ((unsigned char) data [1] << 8) + ((unsigned char) data [0]));

	// Now to read the blocks info...
	for (unsigned int i = 0; i < tap -> _dataBlocks; i++)
	{
		MCHEmul::RawFileData::Block dB;

		// The header of the block
		// First of all, the size of this block
		f.read (data, 4);
		dB._dataSize = (unsigned int) (((unsigned char) data [3] << 24) + 
			((unsigned char) data [2] << 16) + ((unsigned char) data [1] << 8) + ((unsigned char) data [0]));
		
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
bool MCHEmul::RawFileTypeIO::writeFile (MCHEmul::FileData* fD, const std::string& fN, bool bE) const
{
	MCHEmul::RawFileData* tap = 
		dynamic_cast <MCHEmul::RawFileData*> (fD); // To better manipulation...
	if (tap == nullptr)
		return (false); // It is not really a RawFileData structure...

	std::ofstream f (fN, std::ios::out | std::ios::binary);
	if (!f)
		return (false); // Impossible to be opened...

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
