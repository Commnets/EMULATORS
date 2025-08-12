#include <ZX81/FileIO.hpp>


// ---
// The tokens used in the ZX81...
const MCHEmul::Strings ZX81::KeystrokeTypeIO::_ZX81TOKENS = {
	"+",			"-",			"*",			"**",			"/",			"=",			"<=",			">=",			
	"<>",			"<",			">",			"\"",			"\"\"",			",",			"(",			")",			
	";",			".",			":",			"$",			"?",			"£",			"¶",			"ABS ",		
	"ACS ",			"AND ",			"ASN ",			"AT ",			"ATN ",			"CHR$ ",		"COS ",			"CLEAR ",		
	"CLS ",			"CODE ",		"CONT ",		"COPY ",		"DIM ",			"EXP ",			"FAST ",		"FOR ",		
	"GOSUB ",		"GOTO ",		"IF ",			"INKEY$ ",		"INPUT ",		"INT ",			"LEN ",			"LET ",		
	"LIST ",		"LLIST ",		"LN ",			"LOAD ",		"LPRINT ",		"NEW ",			"NEXT ",		"NOT ",			
	"OR ",			"PAUSE ",		"PEEK ",		"PLOT ",		"POKE ",		"PRINT ",		"RAND ",		"RND ",		
	"REM ",			"RETURN ",		"RUN ",			"SAVE ",		"SCROLL ",		"SGN ",			"SIN ",			"SLOW ",		
	"SQR ",			"STEP ",		"STOP ",		"STR$ ",		"TAB ",			"TAN ",			"THEN ",		"TO ",		
	"UNPLOT ",		"USR ",			"VAL " };

// And the keys used to get them...
const std::map <std::string, MCHEmul::Strings> ZX81::KeystrokeTypeIO::_ZX81KEYSTROKES = {
	{ "\n",			{ "RETURN" } },
	{ " ",			{ "SPACE" } },
	{ "+",			{ "LSHIFT+K" } },
	{ "-",			{ "LSHIFT+J" } },
	{ "*",			{ "LSHIFT+B" } },
	{ "**",			{ "LSHIFT+H" } },
	{ "/",			{ "LSHIFT+V" } },
	{ "=",			{ "LSHIFT+L" } },
	{ "<=",			{ "LSHIFT+R" } },
	{ ">=",			{ "LSHIFT+Y" } },
	{ "<>",			{ "LSHIFT+T" } },
	{ "<",			{ "LSHIFT+N" } },
	{ ">",			{ "LSHIFT+M" } },
	{ "\"",			{ "LSHIFT+P" } },
	{ "\"\"",		{ "LSHIFT+Q" } },
	{ ",",			{ "LSHIFT+." } },
	{ "(",			{ "LSHIFT+I" } },
	{ ")",			{ "LSHIFT+O" } },
	{ ";",			{ "LSHIFT+X" } },
	{ ".",			{ "." } },
	{ ":",			{ "LSHIFT+Z" } },
	{ "$",			{ "LSHIFT+U" } },
	{ "?",			{ "LSHIFT+C" } },
	{ "£",			{ "LSHIFT+ " } },
	{ "¶",			{ "LSHIFT+RETURN", "M" } },
	{ "ABS",		{ "LSHIFT+RETURN", "G" } },
	{ "ACS",		{ "LSHIFT+RETURN", "S" } },
	{ "AND",		{ "LSHIFT+2" } },
	{ "ASN",		{ "LSHIFT+RETURN", "A" } },
	{ "AT",			{ "LSHIFT+RETURN", "C" } },		
	{ "ATN",		{ "LSHIFT+RETURN", "D" } },
	{ "CHR$",		{ "LSHIFT+RETURN", "U" } },		
	{ "COS",		{ "LSHIFT+RETURN", "W" } },		
	{ "CLEAR",		{ "X" } },
	{ "CLS",		{ "V" } },
	{ "CODE",		{ "LSHIFT+RETURN", "I" } },	
	{ "CONT",		{ "C" } },
	{ "COPY",		{ "Z" } },
	{ "DIM",		{ "D" } },
	{ "EXP",		{ "LSHIFT+RETURN", "X" } },
	{ "FAST",		{ "LSHIFT+D" } },
	{ "FOR",		{ "F" } },
	{ "GOSUB",		{ "H" } },
	{ "GOTO",		{ "G" } },
	{ "IF",			{ "U" } },
	{ "INKEY$",		{ "LSHIFT+RETURN", "B" } },		
	{ "INPUT",		{ "I" } },
	{ "INT",		{ "LSHIFT+RETURN", "R" } },		
	{ "LEN",		{ "LSHIFT+RETURN", "K" } },
	{ "LET",		{ "L" } },
	{ "LIST",		{ "K" } },
	{ "LLIST",		{ "LSHIFT+G" } },
	{ "LN",			{ "LSHIFT+RETURN", "Z" } },		
	{ "LOAD",		{ "J" } },
	{ "LPRINT",		{ "LSHIFT+S" } },
	{ "NEW",		{ "A" } },
	{ "NEXT",		{ "N" } },
	{ "NOT",		{ "LSHIFT+RETURN", "N" } },		
	{ "OR",			{ "LSHIFT+W" } },
	{ "PAUSE",		{ "M" } },
	{ "PEEK",		{ "LSHIFT+RETURN", "O" } },	
	{ "PLOT",		{ "Q" } },
	{ "POKE",		{ "O" } },
	{ "PRINT",		{ "P" } },
	{ "RAND",		{ "T" } },
	{ "RND",		{ "LSHIFT+RETURN", "T" } },		
	{ "REM",		{ "E" } },
	{ "RETURN",		{ "Y" } },
	{ "RUN",		{ "R" } },
	{ "SAVE",		{ "S" } },
	{ "SCROLL",		{ "B" } },
	{ "SGN",		{ "LSHIFT+RETURN", "F" } },		
	{ "SIN",		{ "LSHIFT+RETURN", "Q" } },		
	{ "SLOW",		{ "LSHIFT+D" } },
	{ "SQR",		{ "LSHIFT+RETURN", "H" } },		
	{ "STEP",		{ "LSHIFT+E" } },
	{ "STOP",		{ "LSHIFT+A" } },
	{ "STR$",		{ "LSHIFT+RETURN", "Y" } },	
	{ "TAB",		{ "LSHIFT+RETURN", "P" } },
	{ "TAN",		{ "LSHIFT+RETURN", "E" } },		
	{ "THEN",		{ "LSHIFT+3" } },		
	{ "TO",			{ "LSHIFT+4" } },		
	{ "UNPLOT",		{ "W" } },
	{ "USR",		{ "LSHIFT+RETURN", "L" } },		
	{ "VAL",		{ "LSHIFT+RETURN", "J" } } };

// ---
// The tokens used in the ZX80...
const MCHEmul::Strings ZX81::KeystrokeTypeIO::_ZX80TOKENS = {
	"+",			"-",			"*",			"**",			"/",			"=",			"<",			">",			
	"\"",			",",			"(",			")",			";",			".",			":",			"$",			
	"?",			"£",			"AND ",			"CLEAR ",		"CLS ",			"CONT ",		"DIM ",			"FOR ",		
	"GOSUB ",		"GOTO ",		"IF ",			"INPUT ",		"LET ",			"LIST ",		"LOAD ",		"NEW ",		
	"NEXT ",		"NOT ",			"OR ",			"POKE ",		"PRINT ",		"RAND ",		"REM ",			"RETURN ",		
	"RUN ",			"SAVE ",		"STOP ",		"THEN ",		"TO " };

// And the keys used to get them...
const std::map <std::string, MCHEmul::Strings> ZX81::KeystrokeTypeIO::_ZX80KEYSTROKES = {
	{ "\n",			{ "RETURN" } },
	{ " ",			{ "SPACE" } },
	{ "+",			{ "LSHIFT+K" } },			
	{ "-",			{ "LSHIFT+J" } },			
	{ "*",			{ "LSHIFT+P" } },			
	{ "**",			{ "LSHIFT+H" } },			
	{ "/",			{ "LSHIFT+V" } },			
	{ "=",			{ "LSHIFT+L" } },			
	{ "<",			{ "LSHIFT+N" } },			
	{ ">",			{ "LSHIFT+M" } },			
	{ "\"",			{ "LSHIFT+Y" } },			
	{ ",",			{ "LSHIFT+." } },			
	{ "(",			{ "LSHIFT+I" } },			
	{ ")",			{ "LSHIFT+O" } },			
	{ ";",			{ "LSHIFT+X" } },			
	{ ".",			{ "." } },
	{ ":",			{ "LSHIFT+Z" } },			
	{ "$",			{ "LSHIFT+U" } },			
	{ "?",			{ "LSHIFT+C" } },			
	{ "£",			{ "LSHIFT+ " } },			
	{ "AND",		{ "LSHIFT+2" } },			
	{ "CLEAR",		{ "X" } },
	{ "CLS",		{ "C" } },
	{ "CONT",		{ "T" } },
	{ "DIM",		{ "D" } },
	{ "FOR",		{ "F" } },
	{ "GOSUB",		{ "V" } },
	{ "GOTO",		{ "G" } },
	{ "IF",			{ "U" } },
	{ "INPUT",		{ "I" } },
	{ "LET",		{ "K" } },
	{ "LIST",		{ "A" } },
	{ "LOAD",		{ "W" } },
	{ "NEW",		{ "Q" } },
	{ "NEXT",		{ "N" } },
	{ "NOT",		{ "LSHIFT+1" } },
	{ "OR",			{ "LSHIFT+B" } },			
	{ "POKE",		{ "H" } },
	{ "PRINT",		{ "O" } },
	{ "RAND",		{ "J" } },
	{ "REM",		{ "Y" } },
	{ "RETURN",		{ "B" } },
	{ "RUN",		{ "R" } },
	{ "SAVE",		{ "E" } },
	{ "STOP",		{ "S" } },
	{ "THEN",		{ "LSHIFT+3" } },		
	{ "TO",			{ "LSHIFT+4" } } };

// ---
MCHEmul::ExtendedDataMemoryBlocks ZX81::OAndPFileData::asMemoryBlocks () const
{
	MCHEmul::ExtendedDataMemoryBlocks result;

	result._name = std::string ("COMMTYNETS");
	result._attributes = MCHEmul::Attributes ();

	// The place where the data has to be put into, will depend on the type of computer!
	MCHEmul::DataMemoryBlock dMB (_initialAddress, _bytes);
	dMB.setName (_name); // No more attributes per block...
	result._data.emplace_back (std::move (dMB));

	return (result);
}

// ---
bool ZX81::OAndPFileTypeIO::canRead (const std::string& fN) const
{
	// Extension?
	size_t pp = fN.find_last_of ('.');
	if (pp == std::string::npos || pp == fN.length ())
		return (false); // ...no

	// The right extension?
	std::string ext = MCHEmul::upper (fN.substr (pp + 1));
	if (ext != "O" && ext != "80" && 
		ext != "P" && ext != "81" && ext != "P81")
		return (false); // ...no

	// Is a P81 type of file 
	// (these one have name at the header, so they are longer than the others))
	bool p81 = (ext == "P81");

	// Possible to be opened?
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (false); // ...no

	// The right size of the file?
	f.seekg (0, std::ios::end);
	std::streamoff s = f.tellg ();
	if (s < (std::streamoff) (p81 ? 127 /** 126 name + last bye = 0x80. */ : 0))
		return (false);

	if (!p81)
		return (true);

	// If the type of file is P81, the last byte has to be = 0x80...
	char dt [1];
	f.seekg (s - 1, std::ios::beg);
	f.read (dt, 1);
	if (dt [0] != 0x80)
		return (false);

	return (true);
}

// ---
MCHEmul::FileData* ZX81::OAndPFileTypeIO::readFile (const std::string& fN, bool bE) const
{
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (nullptr); // Impossible to be open... 
						  // At this point it shouldn't happen but just in case...

	char data [256] = { };
	MCHEmul::FileData* result = new ZX81::OAndPFileData;
	ZX81::OAndPFileData* op = 
		static_cast <ZX81::OAndPFileData*> (result); // To better manipulation...

	// First of all the number of bytes within the file is given...
	f.seekg (0, std::ios::end);
	std::streamoff s = f.tellg ();
	f.seekg (0, std::ios::beg); // back to the beginning...

	// The computer which the information is for, 
	// depends on the extension of the file...
	bool p81 = false;
	std::string ext = MCHEmul::upper (fN.substr (fN.find_last_of ('.') + 1));
	op -> _initialAddress = (ext == "P" || ext == "81" || (p81 = (ext == "P81")))
		? MCHEmul::Address ({ 0x09, 0x40 }, false)
		: MCHEmul::Address ({ 0x00, 0x40 }, false);

	// If the type of file is P81, 
	// ...then the name of the file is at the header!
	if (p81)
	{
		f.read (data, 127); data [128] = 0; // End of char...
		op -> _name = std::string (data);
		s -= 128; // 127 with the name + 1 (last code) = 0x80!
	}

	// Then now the data is read!
	char* prgData = new char [(size_t) s];
	f.read (prgData, s);
	for (size_t i = 0; i < (size_t) s; 
		op -> _bytes.emplace_back (prgData [i++]));
	delete [] prgData;

	f.close ();

	return (result);
}

// ---
MCHEmul::Strings ZX81::KeystrokeTypeIO::generateKeystrokeForToken (const std::string& t) const
{
	// If the keystroke is not found, an enrror is kept into the log and nothing is returned...
	MCHEmul::Strings result;
	std::map <std::string, MCHEmul::Strings>::const_iterator i = _KEYSTROKES.find (t);
	if (i == _KEYSTROKES.end ())
	{
		// All letters ars upperkeys always...
		for (const auto& i : t)
			result.push_back ((i == ' ') 
				// A space is not considering as a token (so it is not in the transaltion list), 
				// but requeries special key for typewriter...
				? "SPACE"
				: std::string (1, std::toupper (i)));
	}
	else
		result = (*i).second;

	return (result);
}

// ---
bool ZX81::KeystrokeTypeIO::isARealToken (char u, const std::string& l, size_t p) const
{
	bool result = true;

	if (_type == ZX81::Type::_ZX80)
	{
		if ((u == '*') &&
			(p + 1) < l.size ())
		{
			// Excludes the operational symbols that are a combination of several...
			char nL = l [p + 1];
			if (nL == '*')
				result = false;
		}
	}
	else
	{
		if ((u == '<' || u == '>' || u == '*' || u == '\"') &&
			(p + 1) < l.size ())
		{
			// Excludes the operational symbols that are a combination of several...
			char nL = l [p + 1];
			if ((u == '<' && nL == '=') ||
				(u == '<' && nL == '>') ||
				(u == '>' && nL == '=') ||
				(u == '*' && nL == '*') ||
				(u == '\"' && nL == '\"'))
				result = false;
		}
	}
		

	return (result);
}
