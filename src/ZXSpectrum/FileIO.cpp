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
	"SIN ",		"SQR",		"STEP ",	"STOP ",	"STR$ ",	"SUB ",		"TAB ",		"TAN ",		
	"THEN ",	"TO",		"USR ",		"VAL ",		"VAL$ ",	"VERIFY " };

// And the keys used to get them...
const std::map <std::string, MCHEmul::Strings> 
		ZXSPECTRUM::KeystrokeTypeIO::_ZXSPECTRUMKEYSTROKES = {
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
		std::string ("Name:") + MCHEmul::trim (_name) + "," +
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
MCHEmul::DataMemoryBlock ZXSPECTRUM::TZXFileData::DataBlock::asDataMemoryBlock () const
{ 
	MCHEmul::DataMemoryBlock result (MCHEmul::Address (), _data);

	result.setAttribute ("TYPE", std::to_string ((unsigned int) _type));

	return (result);
}

// ---
ZXSPECTRUM::TZXFileData::StandardSpeedDataBlock::StandardSpeedDataBlock (const MCHEmul::DataMemoryBlock& dtblk)
	: ZXSPECTRUM::TZXFileData::DataBlock (_ID, dtblk.bytes ()),
	  _pilotPulse ((unsigned short) MCHEmul::getAttributeAsInt ("PILOTPULSE", dtblk.attributes ())),
	  _pilotNumberPulses ((unsigned short) MCHEmul::getAttributeAsInt ("PILOTNUMBERPULSES", dtblk.attributes ())),
	  _syncFirstPulse ((unsigned short) MCHEmul::getAttributeAsInt ("SYNCFIRSTPULSE", dtblk.attributes ())),
	  _syncSecondPulse ((unsigned short) MCHEmul::getAttributeAsInt ("SYNCSECONDPULSE", dtblk.attributes ())),
	  _zeroPulse ((unsigned short) MCHEmul::getAttributeAsInt ("ZEROPULSE", dtblk.attributes ())),
	  _onePulse ((unsigned short) MCHEmul::getAttributeAsInt ("ONEPULSE", dtblk.attributes ())),
	  _bitsUsedLast ((unsigned short) MCHEmul::getAttributeAsInt ("BITSUSEDLAST", dtblk.attributes ())),
	  _blockLapse ((unsigned short) MCHEmul::getAttributeAsInt ("LAPSE", dtblk.attributes ()))
{
	// Nothing else...
}

// ---
MCHEmul::DataMemoryBlock ZXSPECTRUM::TZXFileData::StandardSpeedDataBlock::asDataMemoryBlock () const
{
	MCHEmul::DataMemoryBlock result = 
		std::move (ZXSPECTRUM::TZXFileData::DataBlock::asDataMemoryBlock ());

	result.setAttribute ("PILOTPULSE", std::to_string (_pilotPulse));
	result.setAttribute ("PILOTNUMBERPULSES", std::to_string (_pilotNumberPulses));
	result.setAttribute ("SYNCFIRSTPULSE", std::to_string (_syncFirstPulse));
	result.setAttribute ("SYNCSECONDPULSE", std::to_string (_syncSecondPulse));
	result.setAttribute ("ZEROPULSE", std::to_string (_zeroPulse));
	result.setAttribute ("ONEPULSE", std::to_string (_onePulse));
	result.setAttribute ("BITSUSEDLAST", std::to_string (_bitsUsedLast));
	result.setAttribute ("LAPSE", std::to_string (_blockLapse));

	return (result);
}

// ---
ZXSPECTRUM::TZXFileData::DirectRecordinDataBlock::DirectRecordinDataBlock 
		(const MCHEmul::DataMemoryBlock& dtblk)
	: ZXSPECTRUM::TZXFileData::DataBlock (_ID, dtblk.bytes ()),
	  _tstates ((unsigned short) MCHEmul::getAttributeAsInt ("TSTATES", dtblk.attributes ())),
	  _bitsUsedLast ((unsigned short) MCHEmul::getAttributeAsInt ("BITSUSEDLAST", dtblk.attributes ())),
	  _blockLapse ((unsigned short) MCHEmul::getAttributeAsInt ("LAPSE", dtblk.attributes ()))
{
	// Mothing else...
}

// ---
MCHEmul::DataMemoryBlock ZXSPECTRUM::TZXFileData::DirectRecordinDataBlock::asDataMemoryBlock () const
{
	MCHEmul::DataMemoryBlock result;

	// TODO

	return (result);
}

// ---
ZXSPECTRUM::TZXFileData::InformationDataBlock::InformationDataBlock (const MCHEmul::DataMemoryBlock& dtblk)
	: ZXSPECTRUM::TZXFileData::DataBlock (0x20, dtblk.bytes ()),
	  _title		(MCHEmul::getAttribute ("TITLE", dtblk.attributes ())),
	  _house		(MCHEmul::getAttribute ("HOUSE", dtblk.attributes ())),
	  _author		(MCHEmul::getAttribute ("AUTHOR", dtblk.attributes ())),
	  _year			(MCHEmul::getAttribute ("YEAR", dtblk.attributes ())),
	  _language		(MCHEmul::getAttribute ("LANGUAGE", dtblk.attributes ())),
	  _game			(MCHEmul::getAttribute ("GAME", dtblk.attributes ())),
	  _price		(MCHEmul::getAttribute ("PRICE", dtblk.attributes ())),
	  _protection	(MCHEmul::getAttribute ("PROTECTION", dtblk.attributes ())),
	  _origin		(MCHEmul::getAttribute ("ORIGIN", dtblk.attributes ())),
	  _comments { } // The comments are assigned in the body...
{
	size_t i = 0;
	bool f = true;
	while (!f)
	{
		std::string attrn = 
			"COMMNETS" + MCHEmul::fixLenStr (std::to_string (i++), 3, true, MCHEmul::_SPACES);
		if ((f = MCHEmul::isThereAttribute (attrn, dtblk.attributes ())))
			_comments.push_back (MCHEmul::getAttribute (attrn, dtblk.attributes ()));
	}
}

// ---
MCHEmul::DataMemoryBlock ZXSPECTRUM::TZXFileData::InformationDataBlock::asDataMemoryBlock () const
{
	MCHEmul::DataMemoryBlock result = 
		std::move (ZXSPECTRUM::TZXFileData::DataBlock::asDataMemoryBlock ());

	// In this block there is no data...

	result.setAttribute ("TITLE",		_title);
	result.setAttribute ("HOUSE",		_house);
	result.setAttribute ("AUTHOR",		_author);
	result.setAttribute ("YEAR",		_year);
	result.setAttribute ("LANGUAGE",	_language);
	result.setAttribute ("GAME",		_game);
	result.setAttribute ("PRICE",		_price);
	result.setAttribute ("PROTECTION",	_protection);
	result.setAttribute ("ORIGIN",		_origin);

	// The commnents:: COMMENT00x...
	size_t i = 0;
	for (const auto& j : _comments)
		result.setAttribute ("COMMENTS" + 
			MCHEmul::fixLenStr (std::to_string (i++), 3, true, MCHEmul::_SPACES), j);

	return (result);
}

// ---
ZXSPECTRUM::TZXFileData::~TZXFileData ()
{
	for (const auto& i : _dataBlocks)
		delete (i);
}

// ---
MCHEmul::ExtendedDataMemoryBlocks ZXSPECTRUM::TZXFileData::asMemoryBlocks () const
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
bool ZXSPECTRUM::TZXFileData::isSamplingInfo () const
{
	bool result = true;
	for (const auto& i : _dataBlocks)
		if (i!= nullptr && 
			dynamic_cast <ZXSPECTRUM::TZXFileData::InformationDataBlock*> (i) == nullptr)
			result &= i -> isSamplingInfo (); // Excludes the InformationDataBlock from the comparation---

	return (result);
}

// ---
bool ZXSPECTRUM::TZXFileData::isLikeTAPInfo () const
{
	bool result = true;
	for (const auto& i : _dataBlocks)
		if (i!= nullptr &&
			dynamic_cast <ZXSPECTRUM::TZXFileData::InformationDataBlock*> (i) == nullptr)
			result &= i -> isLikeTAPInfo (); // Excludes the InformationDataBlock from the comparation---

	return (result);
}

// ---
void ZXSPECTRUM::TZXFileData::addHeaderFromMemoryBlock (const MCHEmul::ExtendedDataMemoryBlocks& dMB)
{
	_header._name = MCHEmul::getAttribute ("NAME", dMB._attributes);
	_header._majorVersion = (unsigned char) MCHEmul::getAttributeAsInt ("MAJORVERSION", dMB._attributes);
	_header._minorVersion = (unsigned char) MCHEmul::getAttributeAsInt ("MINORVERSION", dMB._attributes);
}

// ---
void ZXSPECTRUM::TZXFileData::addDataBlockFromMemoryBlock (const MCHEmul::ExtendedDataMemoryBlocks& dMB)
{
	// With all blocks...
	for (const auto& i : dMB._data)
	{
		ZXSPECTRUM::TZXFileData::DataBlock* dB = nullptr;

		// Depending on the type of memorty block is created...
		std::string tp = MCHEmul::trim (MCHEmul::getAttribute ("TYPE", _attributes));
		switch (std::atoi (tp.c_str ()))
		{
			case ZXSPECTRUM::TZXFileData::StandardSpeedDataBlock::_ID:
				{
					dB = new ZXSPECTRUM::TZXFileData::StandardSpeedDataBlock (i);
				}

				break;

			case ZXSPECTRUM::TZXFileData::DirectRecordinDataBlock::_ID:
				{
					dB = new ZXSPECTRUM::TZXFileData::DirectRecordinDataBlock (i);
				}

				break;

			case ZXSPECTRUM::TZXFileData::InformationDataBlock::_ID:
				{
					dB = new ZXSPECTRUM::TZXFileData::InformationDataBlock (i);
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
			_LOG ("Data Memo5ry Block type:" + ((tp == "") ? "-" : tp) + " doesn't exist");
	}
}

// ---
std::string ZXSPECTRUM::TZXFileData::asString () const
{
	std::string result = "";

	// The header...
	result += _header.asString ();
	// ...and the data...
	for (const auto& i : _dataBlocks)
		result += ". " + i -> asString (); 
	// ...depending on the type it will be printed out one thing or another...

	return (result);
}

// ---
bool ZXSPECTRUM::TZXFileTypeIO::canRead (const std::string& fN) const
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
MCHEmul::FileData* ZXSPECTRUM::TZXFileTypeIO::readFile (const std::string& fN, bool bE) const
{
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (nullptr); // Impossible to be open... 
						  // At this point it shouldn't happen but just in case...

	char chr [256] = { }; // To read the info from the file...
	MCHEmul::FileData* result = new ZXSPECTRUM::TZXFileData;
	ZXSPECTRUM::TZXFileData* tzx = 
		static_cast <ZXSPECTRUM::TZXFileData*> (result); // To better manipulation...

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
		ZXSPECTRUM::TZXFileData::DataBlock* dB = nullptr;
		switch (chr [0])
		{
			// Standard type...
			// This is the format used by the load/save standard basic commands...
			case ZXSPECTRUM::TZXFileData::StandardSpeedDataBlock::_ID:
				{
					dB = readBlockType10 (f);
				}

				break;

			// This is the type with information about a pure recording!...
			case ZXSPECTRUM::TZXFileData::DirectRecordinDataBlock::_ID:
				{
					dB = readBlockType15 (f);
				}

				break;

			// The block type 20 os for the comments...
			case ZXSPECTRUM::TZXFileData::InformationDataBlock::_ID:
				{
					dB = readBlockType20 (f);
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
		result = tzx = new ZXSPECTRUM::TZXFileData;
		// ...and empty object will be returned
		// This is something to be managed later as an error...
	}

	return (result);
}

// ---
bool ZXSPECTRUM::TZXFileTypeIO::writeFile (MCHEmul::FileData* fD, const std::string& fN, bool bE) const
{
	ZXSPECTRUM::TZXFileData* tzx = 
		dynamic_cast <ZXSPECTRUM::TZXFileData*> (fD); // To better manipulation...
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

	std::ofstream f (lFN, std::ios::out | std::ios::binary);
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
	for (std::vector <ZXSPECTRUM::TZXFileData::DataBlock*>::const_iterator i = tzx -> _dataBlocks.begin ();
			i != tzx -> _dataBlocks.end () && !e; i++)
	{
		// First of all, it saves the type...
		data [0] = (*i) -> _type;
		f.write (data, 1);
		// ...and depending on it, the resto of the information...
		switch (data [0])
		{
			// The type 10...
			case ZXSPECTRUM::TZXFileData::StandardSpeedDataBlock::_ID:
				{
					writeBlockType10 
						(f, static_cast <ZXSPECTRUM::TZXFileData::StandardSpeedDataBlock*> (*i));
				}

				break;

			// The type 0x20 is the one with the comments...
			case ZXSPECTRUM::TZXFileData::InformationDataBlock::_ID:
				{
					writeBlockType20
						(f, static_cast <ZXSPECTRUM::TZXFileData::InformationDataBlock*> (*i));
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
ZXSPECTRUM::TZXFileData::StandardSpeedDataBlock* ZXSPECTRUM::TZXFileTypeIO::readBlockType10 (std::ifstream& f) const
{
	ZXSPECTRUM::TZXFileData::StandardSpeedDataBlock* result = 
		new ZXSPECTRUM::TZXFileData::StandardSpeedDataBlock;

	bool e = false;
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
void ZXSPECTRUM::TZXFileTypeIO::writeBlockType10 (std::ofstream& f,
	ZXSPECTRUM::TZXFileData::StandardSpeedDataBlock* b) const
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
ZXSPECTRUM::TZXFileData::DirectRecordinDataBlock* ZXSPECTRUM::TZXFileTypeIO::readBlockType15 (std::ifstream& f) const
{
	// TODO 
	
	return (nullptr);
}

// ---
void ZXSPECTRUM::TZXFileTypeIO::writeBlockType15 
	(std::ofstream& f, ZXSPECTRUM::TZXFileData::DirectRecordinDataBlock* b) const
{
	// TODO
}

// ---
ZXSPECTRUM::TZXFileData::InformationDataBlock* ZXSPECTRUM::TZXFileTypeIO::readBlockType20 (std::ifstream& f) const
{
	ZXSPECTRUM::TZXFileData::InformationDataBlock* result = 
		new ZXSPECTRUM::TZXFileData::InformationDataBlock;

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
		std::string txt (dt [p + 2], dt [(p + 2) + dt [p + 1] - 1]);
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
void ZXSPECTRUM::TZXFileTypeIO::writeBlockType20 
	(std::ofstream& f, ZXSPECTRUM::TZXFileData::InformationDataBlock* b) const
{
	// TODO
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
