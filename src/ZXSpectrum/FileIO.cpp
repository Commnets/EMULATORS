#include <ZXSpectrum/FileIO.hpp>

// ---
MCHEmul::ExtendedDataMemoryBlocks ZXSPECTRUM::TAPFileData::asMemoryBlocks () const
{
	MCHEmul::ExtendedDataMemoryBlocks result;
	result._name = "COMMTYNETS";

	for (const auto& i : _infoBlocks)
	{
		MCHEmul::DataMemoryBlock dB;
		dB.setAttribute ("HEADERLENGTH", std::to_string (i._headerLength));
		dB.setAttribute ("HEADERCHECKSUM", std::to_string (i._headerChecksum));
		dB.setAttribute ("DATALENGTH", std::to_string (i._dataLength));
		dB.setAttribute ("TYPE", std::to_string ((unsigned int) i._type));
		dB.setName (i._name);
		dB.setAttribute ("PRM10", std::to_string ((unsigned int) i._parameter1 [0]));
		dB.setAttribute ("PRM11", std::to_string ((unsigned int) i._parameter1 [1]));
		dB.setAttribute ("PRM20", std::to_string ((unsigned int) i._parameter2 [0]));
		dB.setAttribute ("PRM21", std::to_string ((unsigned int) i._parameter2 [1]));
		dB.setAttribute ("DATACHECKSUM", std::to_string (i._dataChecksum));
		dB.addBytes (i._block.bytes ());

		result._data.emplace_back (std::move (dB));
	}

	return (result);
}

// ---
std::string ZXSPECTRUM::TAPFileData::asString () const
{
	std::string result;

	size_t ct = 0;
	for (const auto& i : _infoBlocks)
	{
		result += ((ct != 0) ? ", " : "") +
			std::string ("Block:") + std::to_string (ct) + "(" + MCHEmul::trim (i._name) + ") " +
			std::string ("Type:") + std::to_string ((unsigned int) i._type) + " " +
			std::string ("Size:") + std::to_string (i._dataLength);

		ct++;
	}

	return (result);
}

// ---
bool ZXSPECTRUM::TAPFileTypeIO::canRead (const std::string& fN) const
{
	// Extension?
	size_t pp = fN.find_last_of ('.');
	if (pp == std::string::npos || pp == fN.length ())
		return (false); // ...no

	// The right extension?
	std::string ext = MCHEmul::upper (fN.substr (pp + 1));
	if (ext != "TAP")
		return (false); // ...no

	// Possible to be opened?
	std::ifstream f (fN, std::ios::in || std::ios::binary);
	if (!f)
		return (false); // ...no

	// The right size of the file?
	f.seekg (0, std::ios::end);
	std::streamoff s = f.tellg ();
	if (s < (std::streamoff) 21 /** @see header. */)
		return (false);

	// Those first 19 bytes must be the header
	// The header has a very specific structure
	char chr [2];
	f.seekg (0, std::ios::beg);
	f.read (chr, 2);
	if (chr [0] != 0x13 /** = 19 */) // Little endian...
		return (false);
	f.read (chr, 1);
	if (chr [0] != 0x00 /** type header. */)
		return (false);

	f.close ();

	return (true);
}

// ---
MCHEmul::FileData* ZXSPECTRUM::TAPFileTypeIO::readFile (const std::string& fN, bool bE) const
{
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (nullptr); // Impossible to be open... 
						  // At this point it shouldn't happen but just in case...

	char chr [256] = { }; // To read the info from the file...
	MCHEmul::FileData* result = new ZXSPECTRUM::TAPFileData;
	ZXSPECTRUM::TAPFileData* op = 
		static_cast <ZXSPECTRUM::TAPFileData*> (result); // To better manipulation...

	// First of all the number of bytes within the file is given...
	f.seekg (0, std::ios::end);
	std::streamoff s = f.tellg ();
	f.seekg (0, std::ios::beg); // back to the beginning...

	// Whether there is any erro with the checksum or with the internal structure
	bool e = false; 
	// Define whether it is the first block under construction or not...
	bool fB = true; 
	ZXSPECTRUM::TAPFileData::InfoBlock iB;
	while (!f.eof () && !e)
	{
		// 2 bytes: The length of the block...
		f.read (chr, 2);
		unsigned short bl = (((unsigned short) chr [1]) << 8) + ((unsigned short) chr [0]); // Big endian
		// x bytes: The data of the block...
		char* bd = new char [bl];
		f.read (bd, bl);

		// 1 byte: Points out the type of block...
		switch ((unsigned char) bd [0]) // It is a unsigned number!
		{
			// Header...
			// Fix total length 19 bytes (0x13)
			case 0x00:
				{
					// Always, when a new header is read, a new infoblock starts to be built...
					// ...and the previous one must be added to the list of those...
					if (!fB)
					{
						op -> _infoBlocks.emplace_back (iB);

						iB = ZXSPECTRUM::TAPFileData::InfoBlock ();
					}

					fB = false; // It is no longer the first one...

					// Within the new infoblock...
					// 1 byte: Type of file...
					// If the type weren't possible, the info won't be generated and the loop finishes...
					if (e = (bd [1] > 3)) 
						{ delete [] bd; bd = nullptr; break; }
					iB._type = (ZXSPECTRUM::TAPFileData::InfoBlock::Type) bd [1];
					// 10 bytes: Name of the file data...
					char n [11]; for (size_t i = 0; i < 10; n [i++] = bd [i + 2]); n [10] = 0x00;
					iB._name = std::string (n);
					// 2 bytes: The length of the header...
					iB._headerLength = (((unsigned short) bd [13]) << 8) + ((unsigned short) bd [12]); // Big endian...
					// 2 bytes: parameter 1 (the interpretacition will come later)
					iB._parameter1 [0] = bd [14]; iB._parameter1 [1] = bd [15];
					// 2 bytes: parameter 2 (the interpretation will come later)
					iB._parameter2 [0] = bd [16]; iB._parameter2 [1] = bd [17];
					// 1 byte: The check sum...
					// If the checksum didn't match, the info won't be generated and the loop finishes...
					iB._headerChecksum = 0x00;
					for (size_t i = 1; i < size_t (bl - 1); iB._headerChecksum ^= bd [i++]);
					if (e = (iB._headerChecksum != (unsigned char) bd [bl - 1])) 
						{ delete [] bd; bd = nullptr; break; }
				}

				break;

			// Data...
			case 0xff:
				{
					iB._dataLength = bl;
					iB._dataChecksum = 0xff;
					std::vector <MCHEmul::UByte> dt;
					for (size_t i = 1 /** The byte 0 is the flag. */; 
							i < size_t (bl - 1) /** The check sum of this block is not taken into account either. */; i++)
						{ iB._dataChecksum ^= (unsigned char) bd [i]; dt.emplace_back (bd [i]); }
					// If the checksum didn't match, the info won't be generated and the loop finishes...
					if (e = (iB._dataChecksum != (unsigned char) bd [bl - 1])) 
						{ delete [] bd; bd = nullptr; break; }
					iB._block = MCHEmul::DataMemoryBlock ({ } /** No address needed. */, dt);
				}

				break;
		}

		delete [] bd;
	}

	op -> _infoBlocks.emplace_back (iB);

	f.close ();

	// If there were any error...
	if (e)
	{ 
		delete op;
		result = op = new ZXSPECTRUM::TAPFileData;
		// ...and empty object will be returned
		// This is something to be managed later as an error...
	}

	return (result);
}

// ---
bool ZXSPECTRUM::TAPFileTypeIO::writeFile (MCHEmul::FileData* fD, const std::string& fN, bool bE) const
{
	// TODO

	return (true);
}
