#include <SINCLAIR/FileIO.hpp>

// ---
MCHEmul::ExtendedDataMemoryBlocks SINCLAIR::Pand81FileData::asMemoryBlocks () const
{
	MCHEmul::ExtendedDataMemoryBlocks result;

	result._name = std::string ("COMMTYNETS");
	result._attributes = MCHEmul::Attributes ();
	result._attributes ["EXT"] = _extension;
	result._data.emplace_back 
		(std::move (MCHEmul::DataMemoryBlock 
			(MCHEmul::Address ({ 0x09, 0x40 }, false /** little endian. */), _dataBlock.bytes ())));

	return (result);
}

// ---
bool SINCLAIR::Pand81FileTypeIO::canRead (const std::string& fN) const
{
	// Extension?
	size_t pp = fN.find_last_of ('.');
	if (pp == std::string::npos || pp == fN.length ())
		return (false); // ...no

	// The right extension?
	std::string ext = MCHEmul::upper (fN.substr (pp + 1));
	if (ext != "P" && ext != "81")
		return (false); // ...no

	// Can it be opened?
	std::ifstream f (fN, std::ios::in || std::ios::binary);
	if (!f)
		return (false); // ...no

	// Has it the right length?
	char chr = 0x00;
	f.seekg (0, std::ios::end);
	std::streamoff s = f.tellg ();
	f.seekg (s - 1, std::ios::beg);
	f.read (&chr, 1);
	f.close ();
	if (s < (std::streamoff) 0x01)
		return (false); // ...no. At least there must be a character with 0x80!

	// Can it have the right content?
	if ((unsigned char) chr != 0x80)
		return (false); // ...no. The last character has to be 0x80 always...

	return (true);
}

// ---
MCHEmul::FileData* SINCLAIR::Pand81FileTypeIO::readFile (const std::string& fN, bool bE) const
{
	std::ifstream f (fN, std::ios::in | std::ios::binary);
	if (!f)
		return (nullptr); // Possible to be open... At this point it shouldn't happen but just in case...

	MCHEmul::FileData* result = new SINCLAIR::Pand81FileData;
	SINCLAIR::Pand81FileData* p = 
		static_cast <SINCLAIR::Pand81FileData*> (result); // To better manipulation...

	// Get the extension...
	p -> _extension = 
		MCHEmul::upper (fN.substr (fN.find_last_of ('.') + 1));

	// Read the length of the file...
	f.seekg (0, std::ios::end);
	std::streamoff s = f.tellg ();
	f.seekg (0, std::ios::beg);

	// Read the data...
	char* data = new char [(size_t) s];
	std::vector <MCHEmul::UByte> vData;
	f.read (data, (std::streamsize) s);
	for (size_t i = 0; i < (size_t) s; 
		vData.emplace_back (data [i++]));
	// ...and put it into the result structure...
	p -> _dataBlock = MCHEmul::UBytes (vData);

	f.close ();

	return (result);
}

// ---
bool SINCLAIR::Pand81FileTypeIO::writeFile (MCHEmul::FileData* fD, const std::string& fN, bool bE) const
{
	SINCLAIR::Pand81FileData* p = 
		dynamic_cast <SINCLAIR::Pand81FileData*> (fD); // To better manipulation...
	if (p == nullptr)
		return (false); // It is not the same type of structure...

	// ...Is there any extension declared for the new file?
	size_t pp = fN.find_last_of ('.');
	if (pp == std::string::npos || pp == fN.length ())
		return (false); // ...no
	// ...are the original extension and the final extension the same?
	std::string ext = MCHEmul::upper (fN.substr (pp + 1));
	if (p -> _extension != ext)
		return (false); // ...no

	std::ofstream f (fN, std::ios::out | std::ios::binary);
	if (!f)
		return (false); // Impossible to be opened...

	// ...the block data...
	char* data = new char [p -> _dataBlock.size ()];
	for (size_t i = 0; i < (size_t) p -> _dataBlock.size (); i++)
		data [i] = (char) p -> _dataBlock [i].value ();
	f.write (data, (std::streamsize) p -> _dataBlock.size ());
	delete [] data;

	f.close ();

	return (true);
}
