#include <CORE/FileReader.hpp>

// ---
MCHEmul::FileReader::~FileReader ()
{
	for (MCHEmul::FileTypeReader* i : _readerList)
		delete (i);

	for (const auto& i : _fileData)
		delete (i.second);
}

// ---
MCHEmul::FileData* MCHEmul::FileReader::readFile (const std::string& fN, bool bE)
{
	// If the file has read before, just get the result...
	std::map <std::string, MCHEmul::FileData*>::const_iterator i;
	if ((i = _fileData.find (fN)) != _fileData.end ())
		return ((*i).second);

	// Looks for the right reader to read the file...
	MCHEmul::FileTypeReader* fR = nullptr;
	for (MCHEmul::FileTypeReaderList::const_iterator i = _readerList.begin (); 
			i != _readerList.end () && fR == nullptr; i++)
		fR = (*i) -> canRead (fN) ? (*i) : nullptr;

	// If none exists, then NODATA is given back...
	if (fR == nullptr)
		return (nullptr);

	// Otherwise the file is read and moved into a temporal variable here...
	MCHEmul::FileData* dt = fR -> readFile (fN, bE);
	if (dt == nullptr) // If nothing is read ot ir is invalid...
		return (nullptr); // ...then nothing is givend then back...

	// Finally the data is stored and returned...
	return (_fileData [fN] = dt);
}
