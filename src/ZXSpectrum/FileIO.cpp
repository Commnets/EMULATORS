#include <ZXSpectrum/FileIO.hpp>

// ---
MCHEmul::ExtendedDataMemoryBlocks ZXSPECTRUM::TAPFileData::asMemoryBlocks () const
{
	MCHEmul::ExtendedDataMemoryBlocks result;
	result._name = "COMMTYNETS";

	for (const auto& i : _infoBlocks)
	{
		// Save the header block...
		MCHEmul::DataMemoryBlock dBH;
		dBH.setAttribute ("LENGTH", std::to_string (i._header._length));
		dBH.setAttribute ("FLAG", "0"); // The header is always 00...
		dBH.setAttribute ("TYPE", std::to_string ((unsigned int) i._header._type)); // The type of the data following this header...
		dBH.setAttribute ("NAME", i._header._name);
		dBH.setAttribute ("DATALENGTH", std::to_string (i._header._dataLength));
		dBH.setAttribute ("PRM10", std::to_string ((unsigned int) i._header._parameter1 [0]));
		dBH.setAttribute ("PRM11", std::to_string ((unsigned int) i._header._parameter1 [1]));
		dBH.setAttribute ("PRM20", std::to_string ((unsigned int) i._header._parameter2 [0]));
		dBH.setAttribute ("PRM21", std::to_string ((unsigned int) i._header._parameter2 [1]));
		dBH.setAttribute ("CHECKSUM", std::to_string (i._header._checksum));
		dBH.addBytes (i._header._data.bytes ());
		result._data.emplace_back (std::move (dBH));

		// Save the data information...
		MCHEmul::DataMemoryBlock dBD;
		dBD.setAttribute ("LENGTH", std::to_string (i._length));
		dBD.setAttribute ("FLAG", "255"); // The flag is always FF = 255...
		dBD.setAttribute ("CHECKSUM", std::to_string (i._checksum));
		dBD.addBytes (i._data.bytes ());
		result._data.emplace_back (std::move (dBD));
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
			std::string ("Block:") + std::to_string (ct) + "(" + MCHEmul::trim (i._header._name) + ") " +
			std::string ("Type:") + std::to_string ((unsigned int) i._header._type) + " " +
			std::string ("Data Size:") + std::to_string (i._header._dataLength);

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
		unsigned short bl = (((unsigned short) ((unsigned char) chr [1])) << 8) + 
			((unsigned short) ((unsigned char) chr [0])); // Big endian
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

					// Including the flag type and the checksum! (total)
					iB._header._length = 0x13;

					// Within the new infoblock...
					// 1 byte: Type of file...
					// If the type weren't possible, the info won't be generated and the loop finishes...
					// https://sinclair.wiki.zxnet.co.uk/wiki/TAP_format
					if (e = (bd [1] > 3)) // It can not be more than 3!
						{ delete [] bd; bd = nullptr; break; }
					iB._header._type = (ZXSPECTRUM::TAPFileData::HeaderBlock::Type) bd [1];
					// Type 0 = Program. Type 1 = Number Array. Type 2 = Character Array. Type 3 = Code File.

					// 10 bytes: Name of the file data...
					char n [11]; 
					for (size_t i = 0; i < 10; n [i] = bd [i + 2]) i++; 
					n [10] = 0x00;
					iB._header._name = std::string (n);

					// 2 bytes: The length of the data defined later!
					iB._header._dataLength = (((unsigned short) ((unsigned char) bd [13])) << 8) + 
						((unsigned short) ((unsigned char) bd [12])); // Big endian...
					// 2 bytes: parameter 1 (the interpretacition will come later)
					iB._header._parameter1 [0] = bd [14]; iB._header._parameter1 [1] = bd [15];
					// 2 bytes: parameter 2 (the interpretation will come later)
					iB._header._parameter2 [0] = bd [16]; iB._header._parameter2 [1] = bd [17];

					// The cjecksum is kept in the last byte...
					iB._header._checksum = (unsigned char) bd [bl - 1];
					unsigned char parity = 0x00;
					std::vector <MCHEmul::UByte> dt;
					for (size_t i = 0; /** The byte 0 is the flag. */
							i < bl /** The checksum is also taken into account. */; i++)
						{ parity ^= (unsigned char) bd [i]; dt.emplace_back (bd [i]); }
					// If the parity is not 0 at this point (the checksum is not rigth),
					// the info won't be generated and the loop finishes...
					if (e = (parity != 0)) 
						{ delete [] bd; bd = nullptr; break; } // When error there is no data to be loaded...

					// So the final dara loaded is 18 bytes (and not 19) because the checksum is not loaded!
					iB._header._data = MCHEmul::DataMemoryBlock ({ } /** No address needed. */, dt);
				}

				break;

			// Data...
			case 0xff:
				{
					// Including the flag type and the checksum!
					iB._length= bl;

					// Calculating the checksum...
					iB._checksum = (unsigned char) bd [bl - 1];
					unsigned char parity = 0x00;
					std::vector <MCHEmul::UByte> dt;
					for (size_t i = 0 /** The byte 0 is the flag. */; 
							i < bl /** The checksum of this block is also taken into account. */; i++)
						{ parity ^= (unsigned char) bd [i]; dt.emplace_back (bd [i]); }
					// If the parity is not 0 at this point (the checksum is not rigth), 
					// the info won't be generated and the loop finishes...
					if (e = (parity != 0)) 
						{ delete [] bd; bd = nullptr; break; }

					// In this case the number of bytes loaded is bl - 1, because the checksum is not loaded either...
					iB._data = MCHEmul::DataMemoryBlock ({ } /** No address needed. */, dt);
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
