#include <ZX81/FileIO.hpp>

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
	std::ifstream f (fN, std::ios::in || std::ios::binary);
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
bool ZX81::OAndPFileTypeIO::writeFile (MCHEmul::FileData* fD, const std::string& fN, bool bE) const
{
	ZX81::OAndPFileData* op = 
		dynamic_cast <ZX81::OAndPFileData*> (fD); // To better manipulation...
	if (op == nullptr)
		return (false); // It is not really a OAnPFileData structure...

	std::ofstream f (fN, std::ios::out | std::ios::binary);
	if (!f)
		return (false); // Impossible to be opened...

	char data [256] = { };

	// The name just in the case the type of file is P81...
	if (MCHEmul::upper (fN.substr (fN.find_last_of ('.') + 1)) == "P81")
	{
		size_t i = 0;
		for (; i < op -> _name.size () && i < 127; i++)
			data [i] = op -> _name [i];
		for (; i < 127; data [i++] = 0);
		f.write (data, 127);
	}

	// Then the bytes are written!
	size_t nB = op -> _bytes.size (); // The number of pure info...
	size_t nBT = nB + // When the format is P81 the number of bytes to save will be one more!
		((MCHEmul::upper (fN.substr (fN.find_last_of ('.') + 1)) == "P81") ? 1 : 0);
	char* prgData = new char [nBT];
	for (size_t i = 0; i < nB; i++)
		prgData [i] = (char) op -> _bytes [i].value ();
	if (nBT != nB) prgData [nBT - 1] = (char) 0x80; // Add the last bytes (0x80) when the final file is P81 type...
	f.write (prgData, (std::streamsize) nBT);
	delete [] prgData;

	f.close ();

	return (true);
}
