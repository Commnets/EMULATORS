#include <COMMODORE/FileReaders.hpp>

// ---
bool COMMODORE::CRTFileTypeReader::canRead (const std::string& fN)
{
	if (fN.size () < 4 || 
		MCHEmul::upper (fN.substr (fN.length () - 3, 3)) != "CRT")
		return (false); // The right extension...

	std::ifstream f (fN, std::ios::in || std::ios::binary);
	if (!f)
		return (false); // Possible to be open...

	f.seekg (0, std::ios::end);
	std::streamoff s = f.tellg ();
	f.close ();
	if (s < (std::streamoff) (0x40 /** Header. */ + 0x10 /** Chip Header. */ + 0x1000 /** 4k = Minimum data length. */))
		return (false); // The length of the file is less than expected...

	return (true);
}

// ---
MCHEmul::FileData* COMMODORE::CRTFileTypeReader::readFile (const std::string& fN, bool bE)
{
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (nullptr); // Possible to be open... At this point it shouldn't happen but just in case...

	char data [256];
	MCHEmul::FileData* result = new COMMODORE::CRTFileData;
	COMMODORE::CRTFileData* rCRT = 
		static_cast <COMMODORE::CRTFileData*> (result); // To better manipulation...

	// The header...
	f.read (data, 16); data [16] = 0; // End of char...
	rCRT -> _signature = std::string (data);
	f.read (data, 4);
	rCRT -> _headerSize = (data [0] << 24) + (data [1] << 16) + (data [2] << 8) + data [3];
	f.read (data, 2);
	rCRT -> _cartridgeVersion = (data [0] << 8) + data [1];
	f.read (data, 2);
	rCRT -> _cartridgeType = (data [0] << 8) + data [1];
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
		cD._signature = std::string (data);
		f.read (data, 4);
		cD._packageSize = (data [0] << 24) + (data [1] << 16) + (data [2] << 8) + data [3];
		f.read (data, 2);
		cD._type = (data [0] << 8) + data [1];
		f.read (data, 2);
		cD._bankNumber = (data [0] << 8) + data [1];
		f.read (data, 2);
		cD._startingLoadAddress = MCHEmul::Address ({ data [0], data [1] }, true);
		f.read (data, 2);
		cD._romSize = (data [0] << 24) + (data [1] << 16) + (data [2] << 8) + data [3];
		
		// The data...
		char* romData = new char [cD._romSize];
		f.read (romData, cD._romSize);
		std::vector <MCHEmul::UByte> romBytes;
		for (size_t i = 0; i < cD._romSize; romBytes.emplace_back (romData [i++]));
		cD._content = MCHEmul::UBytes (romBytes);
		delete [] romData;

		rCRT -> _chipsData.emplace_back (std::move (cD) /** No longer valid. */);
	}

	return (result);
}
