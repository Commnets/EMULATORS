#include <ZXSpectrum/FileIO.hpp>

// ---
MCHEmul::DataMemoryBlock ZXSPECTRUM::TAPFileData::InfoBlock::asMemoryBlock () const
{
	MCHEmul::DataMemoryBlock result;

	result.setAttribute ("LENGTH", std::to_string (_length));
	result.setAttribute ("FLAG", std::to_string ((unsigned int) flag ()));
	result.setAttribute ("CHECKSUM", std::to_string (_checksum));
	result.addBytes (_data.bytes ());

	return (result);
}

// ---
std::string ZXSPECTRUM::TAPFileData::InfoBlock::asString () const
{
	std::string result = "";

	result +=
		std::string ("Flag:") + std::to_string ((unsigned int) flag ()) + "," +
		std::string ("Block Length:") + std::to_string (_length) + "," +
		std::string ("Checksum:") + std::to_string ((unsigned int) _checksum);

	return (result);
}

// ---
void ZXSPECTRUM::TAPFileData::InfoBlock::fillFrom (char* bd, unsigned short nR, bool& e)
{
	if (nR < 3) // The block must have at least 3 bytes...
	{
		e = true;

		return;
	}

	// Including the flag type and the checksum!
	_length = nR; // The length of the block is the number of bytes read...

	// Calculating the checksum...
	_checksum = (unsigned char) bd [nR - 1];
	unsigned char parity = 0x00;
	std::vector <MCHEmul::UByte> dt;
	for (size_t i = 0 /** The byte 0 is the flag. */; 
			i < (size_t) nR /** The checksum of this block is also taken into account. */; i++)
		{ parity ^= (unsigned char) bd [i]; dt.emplace_back (bd [i]); }

	// If the parity is not 0 at this point (the checksum is not rigth), 
	// the info won't be generated and the loop finishes...
	e = (parity != 0);

	// In this case the number of bytes loaded is bl - 1, because the checksum is not loaded either...
	_data = MCHEmul::DataMemoryBlock ({ } /** No address needed. */, dt);
}

// ---
MCHEmul::DataMemoryBlock ZXSPECTRUM::TAPFileData::HeaderBlock::asMemoryBlock () const
{
	MCHEmul::DataMemoryBlock result = 
		std::move (ZXSPECTRUM::TAPFileData::InfoBlock::asMemoryBlock ());

	result.setAttribute ("TYPE", std::to_string ((unsigned int) _type)); // The type of the data following this header...
	result.setAttribute ("NAME", _name);
	result.setAttribute ("DATALENGTH", std::to_string (_dataLength));
	result.setAttribute ("PRM10", std::to_string ((unsigned int) _parameter1 [0]));
	result.setAttribute ("PRM11", std::to_string ((unsigned int) _parameter1 [1]));
	result.setAttribute ("PRM20", std::to_string ((unsigned int) _parameter2 [0]));
	result.setAttribute ("PRM21", std::to_string ((unsigned int) _parameter2 [1]));

	return (result);
}

// ---
std::string ZXSPECTRUM::TAPFileData::HeaderBlock::asString () const
{
	std::string result = std::move (ZXSPECTRUM::TAPFileData::InfoBlock::asString ());

	result +=
		"," + 
		std::string ("Name:") + _name + "," +
		std::string ("Type:") + std::to_string ((unsigned int) _type) + "," +
		std::string ("Data Size:") + std::to_string (_dataLength);

	return (result);
}

// ---
void ZXSPECTRUM::TAPFileData::HeaderBlock::fillFrom (char* bd, unsigned short nR, bool& e)
{
	// The header data must have exactly 19 bytes...
	// ...including the flag and the checksum!
	if (nR != 19)
	{
		e = true;

		return;
	}

	// Including the flag type and the checksum! (total)
	_length = 0x13; // The header has a fixed length of 19 bytes...

	// Within the new infoblock...
	// 1 byte: Type of file...
	// If the type weren't possible, the info won't be generated and the loop finishes...
	// https://sinclair.wiki.zxnet.co.uk/wiki/TAP_format
	if (e = (bd [1] > 3)) return; // It can not be more than 3!
	_type = (ZXSPECTRUM::TAPFileData::HeaderBlock::Type) bd [1];
	// Type 0 = Program. Type 1 = Number Array. Type 2 = Character Array. Type 3 = Code File.

	// 10 bytes: Name of the file data...
	char n [11]; 
	for (size_t i = 0; i < 10; i++) n [i] = bd [i + 2]; 
	n [10] = 0x00;
	_name = std::string (n);

	// 2 bytes: The length of the data defined later!
	_dataLength = (((unsigned short) ((unsigned char) bd [13])) << 8) + 
		((unsigned short) ((unsigned char) bd [12])); // Big endian...
	// 2 bytes: parameter 1 (the interpretacition will come later)
	_parameter1 [0] = bd [14]; _parameter1 [1] = bd [15];
	// 2 bytes: parameter 2 (the interpretation will come later)
	_parameter2 [0] = bd [16]; _parameter2 [1] = bd [17];

	// The cjecksum is kept in the last byte...
	_checksum = (unsigned char) bd [nR - 1];
	unsigned char parity = 0x00;
	std::vector <MCHEmul::UByte> dt;
	for (size_t i = 0; /** The byte 0 is the flag. */
			i < (size_t) nR /** The checksum is also taken into account. */; i++)
		{ parity ^= (unsigned char) bd [i]; dt.emplace_back (bd [i]); }

	// If the parity is not 0 at this point (the checksum is not rigth),
	// the info won't be generated and the loop finishes...
	e = (parity != 0);

	// So the final dara loaded is 18 bytes (and not 19) because the checksum is not loaded!
	_data = MCHEmul::DataMemoryBlock ({ } /** No address needed. */, dt);
}

// ---
ZXSPECTRUM::TAPFileData::~TAPFileData ()
{
	for (const auto& i : _infoBlocks)
		delete (i);
}		

// ---
MCHEmul::ExtendedDataMemoryBlocks ZXSPECTRUM::TAPFileData::asMemoryBlocks () const
{
	MCHEmul::ExtendedDataMemoryBlocks result;
	result._name = "COMMTYNETS";

	for (const auto& i : _infoBlocks)
		result._data.emplace_back (std::move (i -> asMemoryBlock ()));

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
				std::string ("Block:") + std::to_string (ct) + "(" + i -> asString () + ")";

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

	// Whether there is any error with the checksum or with the internal structure
	bool e = false; // No error at this point...
	ZXSPECTRUM::TAPFileData::InfoBlock* iB;
	while (!f.eof () && !e)
	{
		iB = nullptr;

		// 2 bytes: The length of the block...
		f.read (chr, 2);
		if (f.eof ())
			continue;

		unsigned short bl = (((unsigned short) ((unsigned char) chr [1])) << 8) + 
			((unsigned short) ((unsigned char) chr [0])); // Big endian
		// x bytes: The data of the block...
		char* bd = new char [bl];

		f.read (bd, bl);
		if (f.eof ())
		{
			delete [] bd;

			continue;
		}

		// 1 byte: Points out the type of block...
		// Factory method...
		switch ((unsigned char) bd [0]) // It is a unsigned number!
		{
			// Header...
			case 0x00:
				{
					(iB = new ZXSPECTRUM::TAPFileData::HeaderBlock) -> fillFrom (bd, bl, e);
				}

				break;

			// Data...
			case 0xff:
				{
					(iB = new ZXSPECTRUM::TAPFileData::InfoBlock) -> fillFrom (bd, bl, e);
				}

				break;
			
			// No other type is allowed...
			default:
				{
					e = true;
				
					_LOG ("The type of block is not allowed: " + 
						std::to_string ((unsigned int) bd [0]));
				}

				break;
		}

		if (iB != nullptr) // It will be nullptr when the type of block is not allowed...
			op -> _infoBlocks.emplace_back (iB); // IB is not deleted...

		delete [] bd;
	}

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
