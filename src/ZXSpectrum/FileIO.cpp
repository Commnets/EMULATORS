#include <ZXSpectrum/FileIO.hpp>

// ---
// The tokens used in the ZXSpectrum...
const MCHEmul::Strings ZXSPECTRUM::KeystrokeTypeIO::_ZXSPECTRUMTOKENS = {
	"+",		"-",		"*",		"/",		"=",		"<=",		">=",		"<>",	
	"<",		">",		"^",		"_",		"\"",		",",		"!",		"#",		
	"$",		"%",		"&",		"'",		"(",		")",		"*",		".",		
	":",		";",		"?",		"@",		"_",		"£",		"©",		"|",		
	"\\",		"[",		"]",		"{",		"}",		"~",		"ABS ",		"ACS ",		
	"AND ",		"ASN ",		"AT ",		"ATN ",		"ATTR ",	"BEEP ",	"BIN "		"BORDER ",	
	"BRIGHT ",	"CAT ",		"CHR$ ",	"CIRCLE ",	"CLEAR ",	"CLS ",		"CLOSE# ",	"CODE ",	
	"CONTINUE ","COPY ",	"COS ",		"DATA ",	"DEFFN ",	"DIM ",		"DRAW ",	"ERASE ",
	"EXP ",		"FLASH ",	"FN ",		"FOR ",		"FORMAT ",	"GOTO ",	"IF ",		"IN ",
	"INK ",		"INKEY$ ",	"INPUT ",	"INT ",		"INVERSE ",	"LEN ",		"LET ",		"LINE ",	
	"LIST ",	"LN ",		"LOAD ",	"LLIST ",	"LPRINT ",	"MERGE ",	"MOVE ",	"NEW ",		
	"NEXT ",	"NOT ",		"OPEN# ",	"OR ",		"OUT ",		"OVER ",	"PAUSE ",	"PAPER ",	
	"PEEK ",	"PI ",		"PLOT ",	"POINT ",	"POKE ",	"PRINT ",	"RANDOMIZE ","READ ",	
	"REM ",		"RESTORE ",	"RETURN ",	"RND ",		"RUN ",		"SAVE ",	"SCREEN$ ",	"SGN ",		
	"SIN ",		"SQR ",		"STEP ",	"STOP ",	"STR$ ",	"SUB ",		"TAB ",		"TAN ",		
	"THEN ",	"TO ",		"USR ",		"VAL ",		"VAL$ ",	"VERIFY " };

// And the keys used to get them...
const std::map <std::string, MCHEmul::Strings> 
		ZXSPECTRUM::KeystrokeTypeIO::_ZXSPECTRUMKEYSTROKES = {
	{ "\n",			{ "RETURN" } }, // In K,L or C mode
	{ " ",			{ "SPACE" } }, // In K,L or C mode
	{ "+",			{ "RCTRL+K" } }, // In K,L or C mode
	{ "-",			{ "RCTRL+J" } }, // In K,L or C mode
	{ "*",			{ "RCTRL+B" } }, // In K,L or C mode
	{ "/",			{ "RCTRL+V" } }, // In K,L or C mode
	{ "=",			{ "RCTRL+L" } }, // In K,L or C mode
	{ "<=",			{ "RCTRL+Q" } }, // In K,L or C mode
	{ ">=",			{ "RCTRL+E" } }, // In K,L or C mode
	{ "<>",			{ "RCTRL+W" } }, // In K,L or C mode
	{ "<",			{ "RCTRL+R" } }, // In K,L or C mode
	{ ">",			{ "RCTRL+T" } }, // In K,L or C mode
	{ "^",			{ "RCTRL+H" } }, // In K,L or C mode, This is equivalent to an arrow pointing up!
	{ "_",			{ "RCTRL+0" } }, // In K,L or C mode
	{ "\"",			{ "RCTRL+P" } }, // In K,L or C mode
	{ ";",			{ "RCTRL+O" } }, // In K,L or C mode
	{ ",",			{ "RCTRL+N" } }, // In K,L or C mode
	{ "!",			{ "RCTRL+1" } }, // In K,L or C mode
	{ "\"",			{ "RCTRL+P" } }, // In K,L or C mode
	{ "#",			{ "RCTRL+3" } }, // In K,L or C mode
	{ "$",			{ "RCTRL+4" } }, // In K,L or C mode
	{ "%",			{ "RCTRL+5" } }, // In K,L or C mode
	{ "&",			{ "RCTRL+6" } }, // In K,L or C mode
	{ "'",			{ "RCTRL+7" } }, // In K,L or C mode
	{ "(",			{ "RCTRL+8" } }, // In K,L or C mode
	{ ")",			{ "RCTRL+9" } }, // In K,L or C mode
	{ "*",			{ "RCTRL+B" } }, // In K,L or C mode
	{ ",",			{ "RCTRL+N" } }, // In K,L or C mode
	{ ".",			{ "RCTRL+M" } }, // In K,L or C mode
	{ ":",			{ "RCTRL+Z" } }, // In K,L or C mode
	{ ";",			{ "RCTRL+O" } }, // In K,L or C mode
	{ "?",			{ "RCTRL+C" } }, // In K,L or C mode
	{ "@",			{ "RCTRL+2" } }, // In K,L or C mode
	{ "_",			{ "RCTRL+0" } }, // In K,L or C mode
	{ "£",			{ "RCTRL+X" } }, // In K,L or C mode
	{ "©",			{ "LSHIFT+RCTRL", "RCTRL+P" } },
	{ "|",			{ "LSHIFT+RCTRL", "RCTRL+S" } },
	{ "\\",			{ "LSHIFT+RCTRL", "RCTRL+D" } },
	{ "[",			{ "LSHIFT+RCTRL", "RCTRL+Y" } },
	{ "]",			{ "LSHIFT+RCTRL", "RCTRL+U" } },
	{ "{",			{ "LSHIFT+RCTRL", "RCTRL+F" } },
	{ "}",			{ "LSHIFT+RCTRL", "RCTRL+G" } },
	{ "~",			{ "LSHIFT+RCTRL", "RCTRL+A" } },
	{ "ABS",		{ "LSHIFT+RCTRL", "G" } },
	{ "ACS",		{ "LSHIFT+RCTRL", "RCTRL+W" } },
	{ "AND",		{ "RCTRL+Y" } }, // In K,L or C mode
	{ "ASN",		{ "LSHIFT+RCTRL", "RCTRL+Q" } },
	{ "AT",			{ "RCTRL+I" } }, // In K,L or C mode
	{ "ATN",		{ "LSHIFT+RCTRL", "RCTRL+E" } },
	{ "ATTR",		{ "LSHIFT+RCTRL", "RCTRL+L" } },
	{ "BEEP",		{ "LSHIFT+RCTRL", "RCTRL+Z" } },
	{ "BIN",		{ "LSHIFT+RCTRL", "B" } },
	{ "BORDER",		{ "B" } }, // In K mode
	{ "BRIGHT",		{ "LSHIFT+RCTRL", "RCTRL+B" } },
	{ "CAT",		{ "LSHIFT+RCTRL", "RCTRL+9" } },
	{ "CHR$",		{ "LSHIFT+RCTRL", "U" } },
	{ "CIRCLE",		{ "LSHIFT+RCTRL", "RCTRL+H" } },
	{ "CLEAR",		{ "X" } }, // In K mode
	{ "CLS",		{ "V" } }, // In K mode
	{ "CLOSE#",		{ "LSHIFT+RCTRL", "RCTRL+5" } },
	{ "CODE",		{ "LSHIFT+RCTRL", "I" } },
	{ "CONTINUE",	{ "C" } }, // In K mode
	{ "COPY",		{ "Z" } }, // In K mode
	{ "COS",		{ "LSHIFT+RCTRL", "W" } },
	{ "DATA",		{ "LSHIFT+RCTRL", "D" } },
	{ "DEFFN",		{ "LSHIFT+RCTRL", "RCTRL+1" } },
	{ "DIM",		{ "D" } }, // In K mode
	{ "DRAW",		{ "W" } }, // In K mode
	{ "ERASE",		{ "LSHIFT+RCTRL", "RCTRL+7" } },
	{ "EXP",		{ "LSHIFT+RCTRL", "X" } },
	{ "FLASH",		{ "LSHIFT+RCTRL", "RCTRL+V" } },
	{ "FN",			{ "LSHIFT+RCTRL", "RCTRL+2" } },
	{ "FOR",		{ "F" } }, // In K mode
	{ "FORMAT",		{ "LSHIFT+RCTRL", "RCTRL+0" } },
	{ "GOTO",		{ "G" } }, // In K mode
	{ "IF",			{ "U" } }, // In K mode
	{ "IN",			{ "LSHIFT+RCTRL", "RCTRL+I" } },
	{ "INK",		{ "LSHIFT+RCTRL", "RCTRL+X" } },
	{ "INKEY$",		{ "LSHIFT+RCTRL", "N" } },
	{ "INPUT",		{ "I" } }, // In K mode
	{ "INT",		{ "LSHIFT+RCTRL", "R" } }, // In K mode
	{ "INVERSE",	{ "LSHIFT+RCTRL", "RCTRL+M" } },
	{ "LEN",		{ "LSHIFT+RCTRL", "K" } },
	{ "LET",		{ "L" } }, // In K mode
	{ "LINE",		{ "LSHIFT+RCTRL", "RCTRL+3" } },
	{ "LIST",		{ "K" } }, // In K mode
	{ "LN",			{ "LSHIFT+RCTRL", "Z" } },
	{ "LOAD",		{ "J" } }, // In K mode
	{ "LLIST",		{ "LSHIFT+RCTRL", "V" } },
	{ "LPRINT",		{ "LSHIFT+RCTRL", "C" } },
	{ "MERGE",		{ "LSHIFT+RCTRL", "RCTRL+T" } },
	{ "MOVE",		{ "LSHIFT+RCTRL", "RCTRL+6" } },
	{ "NEW",		{ "A" } }, // In K mode
	{ "NEXT",		{ "N" } }, // In K mode
	{ "NOT",		{ "RCTRL+S" } }, // In K,L or C mode
	{ "OPEN#",		{ "LSHIFT+RCTRL", "RCTRL+4" } },
	{ "OR",			{ "RCTRL+U" } }, // In K, L or C mode
	{ "OUT",		{ "LSHIFT+RCTRL", "RCTRL+O" } },
	{ "OVER",		{ "LSHIFT+RCTRL", "RCTRL+N" } },
	{ "PAUSE",		{ "M" } }, // In K mode
	{ "PAPER",		{ "RCTRL+C" } }, // In K,L or C mode
	{ "PEEK",		{ "LSHIFT+RCTRL", "O" } },
	{ "PI",			{ "LSHIFT+RCTRL", "M" } },
	{ "PLOT",		{ "Q" } }, // In K mode
	{ "POINT",		{ "LSHIFT+RCTRL", "RCTRL+8" } },
	{ "POKE",		{ "O" } }, // In K mode
	{ "PRINT",		{ "P" } }, // In K mode
	{ "RANDOMIZE",	{ "T" } }, // In K mode
	{ "READ",		{ "LSHIFT+RCTRL", "A" } },
	{ "REM",		{ "E" } }, // In K,L or C mode
	{ "RESTORE",	{ "LSHIFT+RCTRL", "S" } },
	{ "RETURN",		{ "Y" } }, // In K mode
	{ "RND",		{ "LSHIFT+RCTRL", "T" } },
	{ "RUN",		{ "R" } }, // In K mode
	{ "SAVE",		{ "S" } }, // In K mode
	{ "SCREEN$",	{ "LSHIFT+RCTRL", "RCTRL+K" } },
	{ "SGN",		{ "LSHIFT+RCTRL", "F" } },
	{ "SIN",		{ "LSHIFT+RCTRL", "Q" } },
	{ "SQR",		{ "LSHIFT+RCTRL", "H" } },
	{ "STEP",		{ "RCTRL+D" } }, // In K,L or C mode
	{ "STOP",		{ "RCTRL+A" } }, // In K,L or C mode
	{ "STR$",		{ "LSHIFT+RCTRL", "Y" } },
	{ "SUB",		{ "H" } }, // equivale a "GO SUB", In K mode
	{ "TAB",		{ "LSHIFT+RCTRL", "P" } },
	{ "TAN",		{ "LSHIFT+RCTRL", "E" } },
	{ "THEN",		{ "RCTRL+G" } }, // In K,L or C mode
	{ "TO",			{ "RCTRL+F" } }, // In K,L or C mode
	{ "USR",		{ "LSHIFT+RCTRL", "L" } },
	{ "VAL",		{ "LSHIFT+RCTRL", "J" } },
	{ "VAL$",		{ "LSHIFT+RCTRL", "RCTRL+J" } },
	{ "VERIFY",		{ "LSHIFT+RCTRL", "RCTRL+R" } } };

// ---
MCHEmul::DataMemoryBlock ZXSPECTRUM::TAPFileData::InfoBlock::asDataMemoryBlock () const
{
	MCHEmul::DataMemoryBlock result;

	result.setAttribute ("FLAG", std::to_string ((unsigned int) flag ()));
	result.setAttribute ("LENGTH", std::to_string (_length));
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
MCHEmul::DataMemoryBlock ZXSPECTRUM::TAPFileData::HeaderBlock::asDataMemoryBlock () const
{
	MCHEmul::DataMemoryBlock result = 
		std::move (ZXSPECTRUM::TAPFileData::InfoBlock::asDataMemoryBlock ());

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
		std::string ("Type:") + std::to_string ((unsigned int) _type) + "," +
		std::string ("Name:") + MCHEmul::trim (_name) + "," +
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
		result._data.emplace_back (std::move (i -> asDataMemoryBlock ()));

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
		return (false); // ...no extension privided, so no valid file...

	// The right extension?
	std::string ext = MCHEmul::upper (fN.substr (pp + 1));
	if (ext != "TAP")
		return (false); // ...no

	// Possible to be opened?
	std::ifstream f (fN, std::ios::in | std::ios::binary);
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
		if (bl == 0) // If there is no data, the block is not valid...
			continue;
		// bl bytes: The data of the block...
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
bool ZXSPECTRUM::KeystrokeTypeIO::isARealToken (char u, const std::string& l, size_t p) const
{
	bool result = true;

	if ((u == '<' || u == '>') &&
		(p + 1) < l.size ())
	{
		// Excludes the operational symbols that are a combination of several...
		char nL = l [p + 1];
		if ((u == '<' && nL == '=') || 
			(u == '<' && nL == '>') ||
			(u == '>' && nL == '='))
			result = false;
	}

	return (result);
}
