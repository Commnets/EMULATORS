#include <CORE/FileIO.hpp>

// ---
MCHEmul::FileIO::~FileIO ()
{
	for (MCHEmul::FileTypeIO* i : _IOList)
		delete (i);

	for (const auto& i : _fileData)
		delete (i.second);
}

// ---
MCHEmul::FileData* MCHEmul::FileIO::readFile (const std::string& fN, bool bE) const
{
	// If the file has read before, just get the result...
	std::map <std::string, MCHEmul::FileData*>::const_iterator i;
	if ((i = _fileData.find (fN)) != _fileData.end ())
		return ((*i).second);

	// Looks for the right io to read the file...
	MCHEmul::FileTypeIO* fR = nullptr;
	for (MCHEmul::FileTypeIOList::const_iterator i = _IOList.begin (); 
			i != _IOList.end () && fR == nullptr; i++)
		fR = (*i) -> canRead (fN) ? (*i) : nullptr;

	// If none exists, then NODATA is given back...
	if (fR == nullptr)
		return (nullptr);

	// Otherwise the file is read and moved into a temporal variable here...
	MCHEmul::FileData* dt = fR -> readFile (fN, bE);
	if (dt == nullptr) // If nothing is read or it is invalid...
		return (nullptr); // ...then nothing is givend then back...

	// Finally the data is stored and returned...
	return (_fileData [fN] = dt);
}

// ---
bool MCHEmul::FileIO::saveFile (MCHEmul::FileData* fD, const std::string& fN, bool bE) const
{
	if (fD == nullptr)
		return (false); // Not possible...

	// Looks for the right io to write the file...
	MCHEmul::FileTypeIO* fR = nullptr;
	for (MCHEmul::FileTypeIOList::const_iterator i = _IOList.begin (); 
			i != _IOList.end () && fR == nullptr; i++)
		fR = (*i) -> canWrite (fD) ? (*i) : nullptr;

	// If none exists, then NODATA is given back...
	if (fR == nullptr)
		return (false);

	// Otherwise the file is written...
	return (fR -> writeFile (fD, fN, bE));
}
