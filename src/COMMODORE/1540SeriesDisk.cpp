#include <COMMODORE/1540SeriesDisk.hpp>
#include <F6500/C6500.hpp>

const MCHEmul::Attributes COMMODORE::Disk1540SeriesSimulation::_ATTRIBUTES =
	{ { "Name", "Commodore 1540/1541/1570/1571 Disk Injection" },
	  { "Manufacturer", "ICF Software Simulation" } };

// ---
COMMODORE::Disk1540SeriesSimulation::Disk1540SeriesSimulation 
		(int id, unsigned char dN, 
		 const MCHEmul::ASCIIToCodeConverter* cnv,
		 const COMMODORE::SerialIOPeripheralSimulation::Definition& def)
	: COMMODORE::SerialIOPeripheralSimulation (id, dN, def, _ATTRIBUTES),
	  _asciiConverter (cnv),
	  _data { },
	  _d64FileData (nullptr),
	  _channels { },
	  _commandChannel (0),
	  _firmwareStatus (DiskFirmwareStatus::_IDLE),
	  _lastDOSStatus { }
{ 
	// The ascii conververter is mandatory
	// it is used in many blocks of the conversion...
	assert (_asciiConverter != nullptr);

	setClassName ("Disk1540Simulation");
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::initialize ()
{
	// Defined at parent class, but not used any more un this class...
	_status		= COMMODORE::SerialIOPeripheralSimulation::Status::_NOTLISTENING;
	_statusOpen = COMMODORE::SerialIOPeripheralSimulation::StatusOpen::_CLOSE;

	_lastCPUCycles	= 0;
	_commandChannel = 0;
	_firmwareStatus = DiskFirmwareStatus::_IDLE;

	for (auto& channel : _channels)
		channel.reset (definition ()._okResult);
	// The command channel always exists (an open) even before an explicit OPEN...
	_channels [15]._open = true;
	// Standard answer when it is checked...
	setDOSStatus (73, "CBM DOS V2.6 1541");

	return (true);
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::connectData (MCHEmul::FileData* dt)
{
	if ((_d64FileData = dynamic_cast <COMMODORE::D64FileData*> (dt)) == nullptr)
		return (false); // This format are the only ones accepted...

	_data = dt -> asMemoryBlocks ();

	return (initialize ());
}

// ---
MCHEmul::FileData* COMMODORE::Disk1540SeriesSimulation::retrieveData () const
{
	COMMODORE::D64FileData* result = 
		_d64FileData == nullptr
			? nullptr
			: new COMMODORE::D64FileData (*_d64FileData);

	// Adds the name of the file...
	result -> _attributes ["FNAME"] = 
		MCHEmul::getAttribute ("FNAME", _data._attributes);

	return (result);
}

// ---
MCHEmul::InfoStructure COMMODORE::Disk1540SeriesSimulation::getInfoStructure () const
{
	MCHEmul::InfoStructure result =
		std::move (COMMODORE::SerialIOPeripheralSimulation::getInfoStructure ());

	result.add ("DATANAME", (_data._name == "") ? "-" : _data._name);
	result.add ("DATASIZE", _data._data.size ());

	MCHEmul::InfoStructure dS;
	for (size_t i = 0; i < (size_t) _data._data.size (); i++)
	{
		MCHEmul::InfoStructure dSA;
		dSA.add ("ID",		i); // The id...
		dSA.add ("SIZE",	_data._data [i].size ());
		if (_data._data [i].bytes ().size () > 256)
		{
			dSA.add ("FBYTES",	_data._data [i].bytes ()); // All bytes...
			dSA.add ("BYTES",	std::vector <MCHEmul::UByte>
				(_data._data [i].bytes ().begin (),
				 _data._data [i].bytes ().begin () + 256));
		}
		else // This way because the type of method used varies...
			dSA.add ("BYTES", _data._data [i].bytes ());

		dS.add (MCHEmul::fixLenStr (std::to_string (i), 4, true, MCHEmul::_CEROS),
			std::move (dSA));
	}

	result.add ("DATA", dS);

	return (result);
}

// ---
void COMMODORE::Disk1540SeriesSimulation::setDOSStatus
	(unsigned char c, const std::string& m, unsigned char t, unsigned char s)
{
	std::string st =
		MCHEmul::fixLenStr (
			std::to_string ((unsigned int) c), 2, true, MCHEmul::_CEROS) + "," +
		m + "," +
		MCHEmul::fixLenStr (
			std::to_string ((unsigned int) t), 2, true, MCHEmul::_CEROS) + "," +
		MCHEmul::fixLenStr (
			std::to_string ((unsigned int) s), 2, true, MCHEmul::_CEROS);

	_lastDOSStatus = _asciiConverter -> convert (st);
	// The converter doesn't translate control codes, so append CR directly...
	_lastDOSStatus.emplace_back (0x0d);
}

// ---
void COMMODORE::Disk1540SeriesSimulation::executeCommandChannel ()
{
	ChannelState& channel = _channels [15];
	while (!channel._command.empty () &&
		   static_cast <unsigned char> (channel._command.back ()) == 0x0d)
		channel._command.pop_back (); // Take the non usefull characters from back to front...
	// An empty command must preserve the previous DOS status...
	if (channel._command.empty ())
		return;

	// The command is determined by the first character...
	char command = channel._command [0];
	if (command >= 'a' && command <= 'z')
		command = (char) (command - ('a' - 'A'));
	switch (command)
	{
		case 'N':
			{
				executeNewCommand (channel._command);
			}

			break;

		case 'C':
			{
				executeCopyCommand (channel._command);
			}

			break;

		case 'R':
			{
				executeRenameCommand (channel._command);
			}

			break;

		case 'S':
			{
				executeScratchCommand (channel._command);
			}

			break;

		case 'I':
			{
				executeInitializeCommand ();
			}

			break;

		case 'V':
			{
				executeValidateCommand ();
			}

			break;

		// No supported...anycase...
		default:
			{
				setDOSStatus (30, "SYNTAX ERROR");
			}

			break;
	}

	channel._command = "";
	channel.resetAnswer (definition ()._okResult);
}

// ---
void COMMODORE::Disk1540SeriesSimulation::executeOpenChannel ()
{
	static const unsigned char _SERIALERROR = 0x02;
	static const unsigned char _ENDOFFILE = 0x40;
	static const unsigned char _READERROR = _SERIALERROR | _ENDOFFILE;

	ChannelState& channel = _channels [_commandChannel];
	if (!channel._pendingOpen)
		return; // Already opened...

	channel._pendingOpen = false;

	// The channels 0, 1 and 15 are the ones that can have "commands"
	// 15 is the channel for information about the last execution
	// and 0 and 1 are the ones used by the instructions LOAD/SAVE
	if (_commandChannel < 2 || _commandChannel > 14)
		return; // ...so those channles are not opened!

	// The information from the open instruction...
	OpenFileSpec fileSpec = openFileSpec (channel._command);
	// ...can generate several situations...
	// Either the drive is not ready...
	if (!fileSpec._validDrive)
	{
		setDOSStatus (74, "DRIVE NOT READY");

		channel._answerPrepared = true;
		channel._answerStatus = _READERROR;

		return;
	}

	// ...or there is a misthe in the systax of the file name...
	if (!fileSpec._syntaxOK || fileSpec._fileName.empty ())
	{
		setDOSStatus (34, "SYNTAX ERROR");

		channel._answerPrepared = true;
		channel._answerStatus = _READERROR;

		return;
	}

	// In other case, what to do will depend on
	// whether the instruction is to write or to read a new file...
	if (fileSpec._mode == ChannelOpenMode::_WRITE)
	{
		// If there weren't data file it wouldn't be impossible to
		// find a place where to write...
		if (_d64FileData == nullptr)
		{
			setDOSStatus (74, "DRIVE NOT READY");

			channel._answerPrepared = true;
			channel._answerStatus = _READERROR;

			return;
		}

		// This instruction (WRITE) is very specific, and it cannot depend on wildcards...
		if (hasWildcards (fileSpec._fileName))
		{
			setDOSStatus (30, "SYNTAX ERROR");

			channel._answerPrepared = true;
			channel._answerStatus = _READERROR;

			return;
		}

		// Not possible to write with the name of an existing filename...
		// or where it is not possible to find the list of them...
		bool ok = false;
		std::vector <DirectoryEntryPosition> entries =
			findDirectoryEntries (fileSpec._fileName, false, ok);
		if (!ok)
		{
			channel._answerPrepared = true;
			channel._answerStatus = _READERROR;

			return;
		}

		if (!entries.empty ())
		{
			setDOSStatus (63, "FILE EXISTS");

			channel._answerPrepared = true;
			channel._answerStatus = _READERROR;

			return;
		}

		// Finally...
		channel._fileName = fileSpec._fileName;
		channel._fileType = fileSpec._fileType;
		channel._mode = ChannelOpenMode::_WRITE;
		channel._writeBuffer = { };
		channel.resetAnswer (definition ()._okResult);

		setDOSStatus (0, " OK");

		return;
	}

	// This is eecuted when reading...
	channel._fileName = fileSpec._fileName;
	channel._fileType = fileSpec._fileType;
	channel._mode = ChannelOpenMode::_READ;
	channel._writeBuffer = { };
	channel._blockToAnswer =
		std::move (buildAnswerToFileCommand
			(std::make_tuple (fileSpec._fileName, std::string (""))));
	channel._answerPrepared = true;
	channel._answerStatus =
		channel._blockToAnswer.empty () ? _READERROR : definition ()._okResult;
	channel._byteFromBlockToAnswerToSend = 0;
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::executeInitializeCommand ()
{
	// Nothing can be done over a no file data...
	if (_d64FileData == nullptr)
	{
		setDOSStatus (74, "DRIVE NOT READY");

		return (false);
	}

	synchronizeDiskData ();

	for (auto& channel : _channels)
		channel.reset (definition ()._okResult);
	_channels [15]._open = true; // Always...

	setDOSStatus (0, " OK");

	return (true);
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::rebuildBAMFromDirectory (bool setStatus)
{
	static const size_t _BAMTRACK	= 18;
	static const size_t _BAMSECTOR	= 0;
	static const size_t _BAMENTRIES	= 35;

	// Nothing can be done over a no file data...
	if (_d64FileData == nullptr)
	{
		setDOSStatus (74, "DRIVE NOT READY");

		return (false);
	}

	// It should be, but just in case: The BAM info has to be ok!
	if (!isTrackAndSectorValid (_BAMTRACK, _BAMSECTOR))
	{
		setDOSStatus (74, "DRIVE NOT READY");

		return (false);
	}

	// This vector will keep true
	// if a track in a sector is used and false in otehr case...
	// If will be marked following the different entries from the BAM
	std::vector <std::vector <bool>> used
		((size_t) _d64FileData -> _numberTracks + 1);
	for (size_t i = 1; i <= (size_t) _d64FileData -> _numberTracks; i++)
		used [i] = std::vector <bool>
			(_d64FileData -> _tracksData [i - 1] -> _trackInfo._sectors, false);

	// This internal method will simply mark a track and a sector as used
	// when it is not illegal, returned then true!
	auto markSectorAsUsed = [this, &used] (size_t track, size_t sector) -> bool
	{
		if (!isTrackAndSectorValid (track, sector))
		{
			setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
				(unsigned char) track, (unsigned char) sector);

			return (false);
		}

		used [track][sector] = true;

		return (true);
	};

	// This internal method will mark as used all the linked tracks and sector
	// starting from the one received as parameterm and returning true when finishs.
	// It can be used to track where a file is kept!
	auto markFileChainAsUsed = [this, &markSectorAsUsed]
		(size_t track, size_t sector) -> bool
	{
		std::vector <size_t> visitedTracks;
		std::vector <size_t> visitedSectors;
		while (track != 0)
		{
			if (!isTrackAndSectorValid (track, sector))
			{
				setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
					(unsigned char) track, (unsigned char) sector);

				return (false);
			}

			bool visited = false;
			for (size_t i = 0; i < visitedTracks.size () && !visited; i++)
				visited = (visitedTracks [i] == track && visitedSectors [i] == sector);
			if (visited)
			{
				setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
					(unsigned char) track, (unsigned char) sector);

				return (false);
			}

			visitedTracks.emplace_back (track);
			visitedSectors.emplace_back (sector);

			if (!markSectorAsUsed (track, sector))
				return (false);

			std::vector <MCHEmul::UByte> sD = sectorData (track, sector).bytes ();
			if (sD.size () != 256)
			{
				setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
					(unsigned char) track, (unsigned char) sector);

				return (false);
			}

			track = (size_t) sD [0].value ();
			sector = (size_t) sD [1].value ();
		}

		return (true);
	};

	std::vector <MCHEmul::UByte> bam =
		sectorData (_BAMTRACK, _BAMSECTOR).bytes ();
	if (bam.size () != 256)
	{
		setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
			(unsigned char) _BAMTRACK, (unsigned char) _BAMSECTOR);

		return (false);
	}

	// Obviously the BAM track track and sector is used always!
	if (!markSectorAsUsed (_BAMTRACK, _BAMSECTOR))
		return (false);

	// Takes all entries from the BAM and mark all tracks and sectors
	// that are used by the different files saved in the data file...
	size_t dirTrack = (size_t) bam [0].value ();
	size_t dirSector = (size_t) bam [1].value ();
	std::vector <size_t> visitedDirTracks;
	std::vector <size_t> visitedDirSectors;
	while (dirTrack != 0)
	{
		if (!isTrackAndSectorValid (dirTrack, dirSector))
		{
			setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
				(unsigned char) dirTrack, (unsigned char) dirSector);

			return (false);
		}

		bool visited = false;
		for (size_t i = 0; i < visitedDirTracks.size () && !visited; i++)
			visited = (visitedDirTracks [i] == dirTrack &&
				visitedDirSectors [i] == dirSector);
		if (visited)
		{
			setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
				(unsigned char) dirTrack, (unsigned char) dirSector);

			return (false);
		}

		visitedDirTracks.emplace_back (dirTrack);
		visitedDirSectors.emplace_back (dirSector);

		if (!markSectorAsUsed (dirTrack, dirSector))
			return (false);

		std::vector <MCHEmul::UByte> dir =
			sectorData (dirTrack, dirSector).bytes ();
		if (dir.size () != 256)
		{
			setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
				(unsigned char) dirTrack, (unsigned char) dirSector);

			return (false);
		}

		for (size_t i = 0; i < 8; i++)
		{
			size_t pos = (i << 5) + 0x02;
			if (dir [pos].value () == 0x00)
				continue;

			size_t fileTrack = (size_t) dir [pos + 1].value ();
			size_t fileSector = (size_t) dir [pos + 2].value ();
			if (fileTrack != 0 &&
				!markFileChainAsUsed (fileTrack, fileSector))
				return (false);
		}

		dirTrack = (size_t) dir [0].value ();
		dirSector = (size_t) dir [1].value ();
	}

	size_t maxBAMTrack =
		((size_t) _d64FileData -> _numberTracks < _BAMENTRIES)
			? (size_t) _d64FileData -> _numberTracks
			: _BAMENTRIES;
	for (size_t track = 1; track <= maxBAMTrack; track++)
	{
		size_t bamEntry = 0x04 + ((track - 1) << 2);
		if ((bamEntry + 3) >= bam.size ())
		{
			setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
				(unsigned char) _BAMTRACK, (unsigned char) _BAMSECTOR);

			return (false);
		}

		bam [bamEntry] = MCHEmul::UByte::_0;
		bam [bamEntry + 1] = MCHEmul::UByte::_0;
		bam [bamEntry + 2] = MCHEmul::UByte::_0;
		bam [bamEntry + 3] = MCHEmul::UByte::_0;

		unsigned char freeSectors = 0;
		for (size_t sector = 0; sector < used [track].size (); sector++)
		{
			if (used [track][sector])
				continue;

			freeSectors++;
			bam [bamEntry + 1 + (sector >> 3)].setBit (sector & 0x07, true);
		}

		bam [bamEntry] = MCHEmul::UByte (freeSectors);
	}

	if (!setSectorData (_BAMTRACK, _BAMSECTOR, MCHEmul::UBytes (bam)))
	{
		setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
			(unsigned char) _BAMTRACK, (unsigned char) _BAMSECTOR);

		return (false);
	}

	if (setStatus)
		setDOSStatus (0, " OK");

	return (true);
}

// ---
size_t COMMODORE::Disk1540SeriesSimulation::commandKeywordLength
	(const std::string& command) const
{
	// All possible commands managed by a 1541 Disk
	static const std::vector <std::string> _COMMANDS =
		{ "INITIALIZE", "VALIDATE", "SCRATCH", "RENAME", "COPY", "NEW" };

	// Convert into uppercase all letters...
	std::string uCommand = command;
	for (auto& i : uCommand)
		if (i >= 'a' && i <= 'z')
			i = (char) (i - ('a' - 'A'));

	// If the command received is longer than "full" command possible
	// the length returned is the length of the command that matches...
	for (const auto& i : _COMMANDS)
		if (uCommand.length () >= i.length () &&
			uCommand.substr (0, i.length ()) == i)
			return (i.length ());

	// in other case, the command must be only 1 letter length if not empty!
	return (command.empty () ? 0 : 1);
}

// ---
std::string COMMODORE::Disk1540SeriesSimulation::commandPayload
	(const std::string& command, bool* validDrive) const
{
	if (validDrive != nullptr)
		*validDrive = true;

	size_t commandLength = commandKeywordLength (command);
	if (command.length () <= commandLength)
		return (""); // No command...

	// Verify whether the number of the unit affected by the command
	// is or not part of the command...
	std::string result = command.substr (commandLength);
	if (result.length () >= 2 && result [1] == ':')
	{
		// if it is, it can only be 0 to be considered as valid!
		if (validDrive != nullptr)
			*validDrive = (result [0] == '0');
		// ...and any case the command itself
		// will be what it is after the :
		return (result.substr (2));
	}

	// There is no number of unit but there is :
	// then the command is what is after that symbol and the drive unit is always valid...
	if (!result.empty () && result [0] == ':')
		return (result.substr (1));

	// If there is no : symbol, so the command is everything...
	return (result);
}

// ---
std::string COMMODORE::Disk1540SeriesSimulation::fileNameFromFileSpec
	(const std::string& fileSpec, bool* validDrive) const
{
	if (validDrive != nullptr)
		*validDrive = true;

	// The file name is, by default, with no spaces...
	std::string result = MCHEmul::trim (fileSpec);

	// If there is no number of unit affected but with the symbol : ...
	// ...the name of the file affected is everything after that symbol.
	if (!result.empty () && result [0] == ':')
		result = result.substr (1);

	// If there is number of unit affected before the symbol :
	// The valid is only valid when it is 0, and the name of the
	// file affected us everything after the symbol :
	if (result.length () >= 2 && result [1] == ':')
	{
		if (validDrive != nullptr)
			*validDrive = (result [0] == '0');
		result = result.substr (2);
	}

	// At this point result has the name of the file
	// but potentially including symbols that change what to do with it
	// like save, create,...
	// Those instructions must be extracted from the name of the file
	size_t commaPos = result.find (',');
	if (commaPos != std::string::npos)
		result = result.substr (0, commaPos);

	// Remove potential remaining spaces...
	return (MCHEmul::trim (result));
}

// ---
COMMODORE::Disk1540SeriesSimulation::OpenFileSpec
	COMMODORE::Disk1540SeriesSimulation::openFileSpec (const std::string& fileSpec) const
{
	OpenFileSpec result;

	// OPEN receives a compact CBM DOS file specification:
	// optional drive, file name, optional type and optional mode.
	std::string text = MCHEmul::trim (fileSpec);

	// ":NAME" means the current drive. In this simulation only drive 0 exists.
	if (!text.empty () && text [0] == ':')
		text = text.substr (1);

	// "0:NAME" is valid; any other drive keeps the parsed name but marks the
	// specification as not usable by this single-drive implementation.
	if (text.length () >= 2 && text [1] == ':')
	{
		result._validDrive = (text [0] == '0');
		text = text.substr (2);
	}

	// The remaining fields are comma-separated: NAME,TYPE,MODE.
	// This is what allows OPEN8,8,8,"0:NUEVO,S,W".
	std::vector <std::string> parts = splitCommandList (text);
	if (parts.empty ())
	{
		result._syntaxOK = false;

		return (result);
	}

	result._fileName = MCHEmul::trim (parts [0]);
	if (result._fileName.empty ())
		result._syntaxOK = false;

	// CBM DOS accepts the short one-letter forms here. Longer spellings are
	// handled by looking at the first significant character.
	for (size_t i = 1; i < parts.size () && result._syntaxOK; i++)
	{
		std::string part = MCHEmul::trim (parts [i]);
		if (part.empty ())
			continue;

		char option = part [0];
		if (option >= 'a' && option <= 'z')
			option = option - ('a' - 'A'); // To uppercase...
		switch (option)
		{
			case 'S':	result._fileType	= 0x81; break;
			case 'P':	result._fileType	= 0x82; break;
			case 'U':	result._fileType	= 0x83; break;
			case 'R':	result._mode		= ChannelOpenMode::_READ; break;
			case 'W':	result._mode		= ChannelOpenMode::_WRITE; break;
			default:	result._syntaxOK	= false; break;
		}
	}

	return (result);
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::writeNewFile
	(const std::string& fileName, unsigned char fileType, const std::vector <MCHEmul::UByte>& data)
{
	// A write OPEN only reserves the channel. The D64 image is modified when
	// CLOSE arrives and all PRINT# bytes are already accumulated in data.
	if (_d64FileData == nullptr)
	{
		setDOSStatus (74, "DRIVE NOT READY");

		return (false);
	}

	if (fileName.empty () || hasWildcards (fileName))
	{
		setDOSStatus (30, "SYNTAX ERROR");

		return (false);
	}

	// There must not be any entry for the file requested...
	// Otherwise it is not possible to creat it...
	bool ok = false;
	std::vector <DirectoryEntryPosition> entries =
		findDirectoryEntries (fileName, false, ok);
	if (!ok)
		return (false); // ...obviously if error finding the entries...not possible to continue...
	if (!entries.empty ())
	{
		setDOSStatus (63, "FILE EXISTS");

		return (false);
	}

	// Rebuild the BAM before allocation because previous DOS commands may have
	// changed file chains or directory entries.
	if (!rebuildBAMFromDirectory (false))
		return (false);

	// Find an empty directory entry where the new file will be described.
	DirectoryEntryPosition newEntryPos;
	if (!findFreeDirectoryEntry (newEntryPos, true))
		return (false); // ...if not possible, not possible to continue...
	// After this point newEntryPos has the directory position
	// where the info of the new file will be written.

	// Alweays after touching the data the integrity is verified back...
	if (!rebuildBAMFromDirectory (false))
		return (false);

	// Allocate the sector chain that will hold the file payload.
	size_t firstTrack = 0, firstSector = 0;
	unsigned short blocks = 0;
	if (!allocateFileChain (data, firstTrack, firstSector, blocks))
		return (false);

	// Create the 32-byte directory entry. Bit 7 marks the file as closed and
	// the low nibble keeps the CBM DOS file type.
	std::vector <MCHEmul::UByte> entry (32, MCHEmul::UByte::_0);
	entry [0] = MCHEmul::UByte ((fileType & 0x0f) | 0x80);
	entry [1] = MCHEmul::UByte ((unsigned char) firstTrack);
	entry [2] = MCHEmul::UByte ((unsigned char) firstSector);
	std::string paddedName = padPETSCIIName (fileName);
	for (size_t i = 0; i < 16; i++)
		entry [3 + i] = MCHEmul::UByte ((unsigned char) paddedName [i]);
	entry [28] = MCHEmul::UByte ((unsigned char) (blocks & 0xff));
	entry [29] = MCHEmul::UByte ((unsigned char) (blocks >> 8));

	// ...and write it in the directory at the location found for that...
	if (!writeDirectoryEntry (newEntryPos, entry))
	{
		rebuildBAMFromDirectory (false);

		return (false);
	}

	// Mandatory!
	if (!rebuildBAMFromDirectory (false))
		return (false);

	setDOSStatus (0, " OK");

	return (true);
}

// ---
std::vector <std::string> COMMODORE::Disk1540SeriesSimulation::splitCommandList
	(const std::string& text) const
{
	std::vector <std::string> result;

	size_t p = 0;
	while (p <= text.length ())
	{
		size_t nP = text.find (',', p);
		if (nP == std::string::npos)
		{
			result.emplace_back (MCHEmul::trim (text.substr (p)));
			break;
		}

		result.emplace_back (MCHEmul::trim (text.substr (p, nP - p)));
		p = nP + 1;
	}

	return (result);
}

// ---
std::string COMMODORE::Disk1540SeriesSimulation::padPETSCIIName
	(const std::string& name) const
{
	return (COMMODORE::D64FileData::padPETSCIIName (name));
}

// ---
std::vector <COMMODORE::Disk1540SeriesSimulation::DirectoryEntryPosition>
	COMMODORE::Disk1540SeriesSimulation::findDirectoryEntries
		(const std::string& pattern, bool wildcards, bool& ok)
{
	std::vector <DirectoryEntryPosition> result;
	ok = false;

	// Directory traversal starts from the BAM sector, so a missing disk
	// or a malformed BAM means the search itself cannot be trusted.
	if (_d64FileData == nullptr)
	{
		setDOSStatus (74, "DRIVE NOT READY");

		return (result);
	}

	std::vector <MCHEmul::UByte> bam = sectorData (18, 0).bytes ();
	if (bam.size () != 256)
	{
		setDOSStatus (66, "ILLEGAL TRACK OR SECTOR", 18, 0);

		return (result);
	}

	size_t dirTrack = (size_t) bam [0].value ();
	size_t dirSector = (size_t) bam [1].value ();
	std::vector <size_t> visitedTracks;
	std::vector <size_t> visitedSectors;
	// Directory sectors are linked like file sectors. Keep visited sectors
	// to avoid looping forever on a corrupted directory chain.
	while (dirTrack != 0)
	{
		if (!isTrackAndSectorValid (dirTrack, dirSector))
		{
			setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
				(unsigned char) dirTrack, (unsigned char) dirSector);

			return (result);
		}

		bool visited = false;
		for (size_t i = 0; i < visitedTracks.size () && !visited; i++)
			visited =
				(visitedTracks [i] == dirTrack && visitedSectors [i] == dirSector);
		if (visited)
		{
			setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
				(unsigned char) dirTrack, (unsigned char) dirSector);

			return (result);
		}

		visitedTracks.emplace_back (dirTrack);
		visitedSectors.emplace_back (dirSector);

		std::vector <MCHEmul::UByte> dir = sectorData (dirTrack, dirSector).bytes ();
		if (dir.size () != 256)
		{
			setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
				(unsigned char) dirTrack, (unsigned char) dirSector);

			return (result);
		}

		for (size_t i = 0; i < 8; i++)
		{
			// Every directory sector stores eight 32-byte entries after the
			// two-byte next-directory-sector pointer.
			size_t pos = (i << 5) + 0x02;
			if (dir [pos].value () == 0x00)
				continue;

			// File names are PETSCII padded with $a0. The padding is not part
			// of the logical DOS filename used by wildcard matching.
			std::string name = "";
			for (size_t j = 0; j < 16; j++)
			{
				unsigned char c = dir [pos + 3 + j].value ();
				if (c == 0xa0)
					break;

				name += (char) c;
			}

			if (name.empty ())
				continue;

			if ((wildcards && nameMatchesWithPattern (name, pattern)) ||
				(!wildcards && name == pattern))
				result.emplace_back (dirTrack, dirSector, i, pos);
		}

		dirTrack = (size_t) dir [0].value ();
		dirSector = (size_t) dir [1].value ();
	}

	// Reaching this point means the full directory chain was valid, even if
	// no entry matched the requested name or pattern.
	ok = true;

	return (result);
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::findFreeDirectoryEntry
	(DirectoryEntryPosition& pos, bool createDirectorySector)
{
	// Free directory entries live in the directory chain, whose first sector
	// is pointed by the BAM sector at track 18 sector 0.
	std::vector <MCHEmul::UByte> bam = sectorData (18, 0).bytes ();
	if (bam.size () != 256)
	{
		setDOSStatus (66, "ILLEGAL TRACK OR SECTOR", 18, 0);

		return (false);
	}

	size_t dirTrack = (size_t) bam [0].value ();
	size_t dirSector = (size_t) bam [1].value ();
	size_t lastTrack = 0, lastSector = 0;
	std::vector <size_t> visitedTracks;
	std::vector <size_t> visitedSectors;
	// First scan the existing directory sectors. Creating a new sector is
	// only needed if no deleted/empty entry is found.
	while (dirTrack != 0)
	{
		if (!isTrackAndSectorValid (dirTrack, dirSector))
		{
			setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
				(unsigned char) dirTrack, (unsigned char) dirSector);

			return (false);
		}

		bool visited = false;
		for (size_t i = 0; i < visitedTracks.size () && !visited; i++)
			visited =
				(visitedTracks [i] == dirTrack && visitedSectors [i] == dirSector);
		if (visited)
		{
			setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
				(unsigned char) dirTrack, (unsigned char) dirSector);

			return (false);
		}

		visitedTracks.emplace_back (dirTrack);
		visitedSectors.emplace_back (dirSector);

		std::vector <MCHEmul::UByte> dir = sectorData (dirTrack, dirSector).bytes ();
		if (dir.size () != 256)
		{
			setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
				(unsigned char) dirTrack, (unsigned char) dirSector);

			return (false);
		}

		for (size_t i = 0; i < 8; i++)
		{
			// A zero type byte means a reusable directory entry. Other entries
			// may be valid files and must be preserved.
			size_t offset = (i << 5) + 0x02;
			if (dir [offset].value () == 0x00)
			{
				pos = DirectoryEntryPosition (dirTrack, dirSector, i, offset);

				return (true);
			}
		}

		lastTrack = dirTrack;
		lastSector = dirSector;
		dirTrack = (size_t) dir [0].value ();
		dirSector = (size_t) dir [1].value ();
	}

	// If the caller is not allowed to extend the directory, reaching the end
	// of the chain without a free entry is equivalent to disk full.
	if (!createDirectorySector || lastTrack == 0)
	{
		setDOSStatus (72, "DISK FULL");

		return (false);
	}

	// New directory sectors are allocated on track 18. The BAM bitmap tells
	// which sector can be linked after the current last directory sector.
	size_t newSector = 0;
	size_t bamEntry = 0x04 + ((18 - 1) << 2);
	for (size_t sector = 1;
			sector < _d64FileData -> _tracksData [17] -> _trackInfo._sectors &&
			newSector == 0; sector++)
	{
		if (bam [bamEntry + 1 + (sector >> 3)].bit (sector & 0x07))
			newSector = sector;
	}

	if (newSector == 0)
	{
		setDOSStatus (72, "DISK FULL");

		return (false);
	}

	std::vector <MCHEmul::UByte> lastDir = sectorData (lastTrack, lastSector).bytes ();
	if (lastDir.size () != 256)
	{
		setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
			(unsigned char) lastTrack, (unsigned char) lastSector);

		return (false);
	}

	// Link the previous last directory sector to the newly allocated one.
	lastDir [0] = MCHEmul::UByte (18);
	lastDir [1] = MCHEmul::UByte ((unsigned char) newSector);
	if (!setSectorData (lastTrack, lastSector, MCHEmul::UBytes (lastDir)))
		return (false);

	// The new sector starts empty and terminates the directory chain until
	// another extension is required.
	std::vector <MCHEmul::UByte> newDir (256, MCHEmul::UByte::_0);
	newDir [0] = MCHEmul::UByte::_0;
	newDir [1] = MCHEmul::UByte::_FF;
	if (!setSectorData (18, newSector, MCHEmul::UBytes (newDir)))
		return (false);

	pos = DirectoryEntryPosition (18, newSector, 0, 0x02);

	return (true);
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::readDirectoryEntry
	(const DirectoryEntryPosition& pos, std::vector <MCHEmul::UByte>& entry)
{
	// Directory entries are fixed 32-byte records embedded inside a sector.
	entry = { };
	std::vector <MCHEmul::UByte> dir = sectorData (pos._track, pos._sector).bytes ();
	if (dir.size () != 256 || (pos._offset + 31) >= dir.size ())
	{
		setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
			(unsigned char) pos._track, (unsigned char) pos._sector);

		return (false);
	}

	// Copy only the requested entry; callers should not need to know about
	// the surrounding directory sector layout.
	entry.insert (entry.end (), dir.begin () + pos._offset,
		dir.begin () + pos._offset + 32);

	return (true);
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::writeDirectoryEntry
	(const DirectoryEntryPosition& pos, const std::vector <MCHEmul::UByte>& entry)
{
	// A directory entry with any other size would corrupt neighboring entries.
	if (entry.size () != 32)
	{
		setDOSStatus (30, "SYNTAX ERROR");

		return (false);
	}

	// Update a sector copy and then write the whole sector back, matching the
	// D64 storage unit exposed by D64FileData.
	std::vector <MCHEmul::UByte> dir = sectorData (pos._track, pos._sector).bytes ();
	if (dir.size () != 256 || (pos._offset + 31) >= dir.size ())
	{
		setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
			(unsigned char) pos._track, (unsigned char) pos._sector);

		return (false);
	}

	// Only the selected 32-byte slot is replaced; the other seven entries in
	// the same directory sector remain untouched.
	for (size_t i = 0; i < 32; i++)
		dir [pos._offset + i] = entry [i];

	return (setSectorData (pos._track, pos._sector, MCHEmul::UBytes (dir)));
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::fileExists (const std::string& name)
{
	bool ok = false;
	std::vector <DirectoryEntryPosition> entries =
		findDirectoryEntries (name, false, ok);
	return (ok && !entries.empty ());
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::readFilePayloadGuarded
	(size_t track, size_t sector, std::vector <MCHEmul::UByte>& data)
{
	// File contents are a linked list of data sectors. This helper reads the
	// payload while protecting DOS commands from corrupted or cyclic chains.
	data = { };
	std::vector <size_t> visitedTracks;
	std::vector <size_t> visitedSectors;
	// Byte 0/1 of each sector either point to the next Track/Sector or mark
	// the final sector when byte 0 is zero.
	while (track != 0)
	{
		if (!isTrackAndSectorValid (track, sector))
		{
			setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
				(unsigned char) track, (unsigned char) sector);

			return (false);
		}

		bool visited = false;
		for (size_t i = 0; i < visitedTracks.size () && !visited; i++)
			visited =
				(visitedTracks [i] == track && visitedSectors [i] == sector);
		if (visited)
		{
			setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
				(unsigned char) track, (unsigned char) sector);

			return (false);
		}

		visitedTracks.emplace_back (track);
		visitedSectors.emplace_back (sector);

		std::vector <MCHEmul::UByte> sectorBytes = sectorData (track, sector).bytes ();
		if (sectorBytes.size () != 256)
		{
			setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
				(unsigned char) track, (unsigned char) sector);

			return (false);
		}

		// Last sector: byte 1 is the last used byte index, so the payload ends
		// before the physical 256-byte sector ends.
		if (sectorBytes [0].value () == 0x00)
		{
			size_t lastByte = (size_t) sectorBytes [1].value ();
			if (lastByte == 0)
			{
				setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
					(unsigned char) track, (unsigned char) sector);

				return (false);
			}

			if (lastByte >= 2)
				data.insert (data.end (),
					sectorBytes.begin () + 2, sectorBytes.begin () + lastByte + 1);

			track = 0;
		}
		else
		{
			// Non-final sectors always contribute their 254 payload bytes after the
			// two-byte next Track/Sector pointer.
			data.insert (data.end (), sectorBytes.begin () + 2, sectorBytes.end ());
			track = (size_t) sectorBytes [0].value ();
			sector = (size_t) sectorBytes [1].value ();
		}
	}

	return (true);
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::allocateFileChain
	(const std::vector <MCHEmul::UByte>& data,
	 size_t& firstTrack, size_t& firstSector, unsigned short& blocks)
{
	// The caller receives the first Track/Sector pair for the directory entry
	// plus the number of allocated blocks.
	firstTrack = firstSector = 0;
	blocks = 0;

	std::vector <MCHEmul::UByte> bam = sectorData (18, 0).bytes ();
	if (bam.size () != 256)
	{
		setDOSStatus (66, "ILLEGAL TRACK OR SECTOR", 18, 0);

		return (false);
	}

	// Each data sector has two link bytes, so only 254 bytes are available
	// for file payload. Even an empty file gets one final sector.
	size_t neededBlocks = data.empty () ? 1 : ((data.size () + 253) / 254);
	std::vector <size_t> tracks;
	std::vector <size_t> sectors;
	size_t maxTrack =
		((size_t) _d64FileData -> _numberTracks < 35)
			? (size_t) _d64FileData -> _numberTracks
			: 35;
	// Walk the BAM in disk order and reserve candidate free sectors. Track 18
	// is skipped because it contains the BAM and directory.
	for (size_t track = 1; track <= maxTrack && tracks.size () < neededBlocks; track++)
	{
		if (track == 18)
			continue;

		size_t bamEntry = 0x04 + ((track - 1) << 2);
		for (size_t sector = 0;
				sector < _d64FileData -> _tracksData [track - 1] -> _trackInfo._sectors &&
				tracks.size () < neededBlocks; sector++)
		{
			if (bam [bamEntry + 1 + (sector >> 3)].bit (sector & 0x07))
			{
				tracks.emplace_back (track);
				sectors.emplace_back (sector);
			}
		}
	}

	// Allocation is all-or-nothing from the caller perspective: if there are
	// not enough candidate sectors, no file chain is written.
	if (tracks.size () < neededBlocks)
	{
		setDOSStatus (72, "DISK FULL");

		return (false);
	}

	// Now write the linked sector chain. The sector list was selected first so
	// every non-final sector can point to the next one.
	size_t offset = 0;
	for (size_t i = 0; i < neededBlocks; i++)
	{
		std::vector <MCHEmul::UByte> sectorBytes (256, MCHEmul::UByte::_0);
		// Final sector: byte 0 is zero and byte 1 stores the last used byte index.
		if (i == (neededBlocks - 1))
		{
			size_t bytesToCopy = data.size () - offset;
			size_t lastByte = bytesToCopy + 1;
			sectorBytes [0] = MCHEmul::UByte::_0;
			sectorBytes [1] = MCHEmul::UByte ((unsigned char) lastByte);
			for (size_t j = 0; j < bytesToCopy; j++)
				sectorBytes [2 + j] = data [offset + j];
		}
		else
		{
			sectorBytes [0] = MCHEmul::UByte ((unsigned char) tracks [i + 1]);
			sectorBytes [1] = MCHEmul::UByte ((unsigned char) sectors [i + 1]);
			for (size_t j = 0; j < 254; j++)
				sectorBytes [2 + j] = data [offset + j];
			offset += 254;
		}

		// Commit each physical sector immediately; any bad Track/Sector is reported
		// as media corruption instead of silently producing a broken chain.
		if (!_d64FileData -> setSectorData
				((unsigned char) tracks [i], (unsigned char) sectors [i],
				 MCHEmul::UBytes (sectorBytes)))
		{
			setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
				(unsigned char) tracks [i], (unsigned char) sectors [i]);

			return (false);
		}
	}

	// Publish the chain head after all sectors have been written successfully.
	firstTrack = tracks [0];
	firstSector = sectors [0];
	blocks = (unsigned short) neededBlocks;
	synchronizeDiskData ();

	return (true);
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::executeScratchCommand (const std::string& command)
{
	// SCRATCH deletes files by clearing their directory entries. The file data
	// sectors are released later when the BAM is rebuilt from the directory.
	if (_d64FileData == nullptr)
	{
		setDOSStatus (74, "DRIVE NOT READY");

		return (false);
	}

	bool validDrive = true;
	std::string payload = MCHEmul::trim (commandPayload (command, &validDrive));
	if (!validDrive)
	{
		setDOSStatus (74, "DRIVE NOT READY");

		return (false);
	}

	// CBM DOS allows comma-separated scratch patterns. Resolve all matches first
	// so duplicated wildcard matches do not affect the final count.
	std::vector <std::string> patterns = splitCommandList (payload);
	if (patterns.empty ())
	{
		setDOSStatus (30, "SYNTAX ERROR");

		return (false);
	}

	std::vector <DirectoryEntryPosition> entries;
	for (const auto& pattern : patterns)
	{
		if (pattern.empty ())
		{
			setDOSStatus (30, "SYNTAX ERROR");

			return (false);
		}

		bool ok = false;
		std::vector <DirectoryEntryPosition> matches =
			findDirectoryEntries (pattern, true, ok);
		if (!ok)
			return (false);

		for (const auto& match : matches)
		{
			bool alreadyAdded = false;
			for (const auto& entry : entries)
				alreadyAdded =
					alreadyAdded ||
					(entry._track == match._track &&
					 entry._sector == match._sector &&
					 entry._entry == match._entry);
			if (!alreadyAdded)
				entries.emplace_back (match);
		}
	}

	// Locked files cannot be scratched. Check all targets before mutating any
	// directory entry so a partial delete does not happen.
	for (const auto& pos : entries)
	{
		std::vector <MCHEmul::UByte> entry;
		if (!readDirectoryEntry (pos, entry))
			return (false);

		if ((entry [0].value () & (1 << 6)) != 0)
		{
			setDOSStatus (26, "WRITE PROTECT ON");

			return (false);
		}
	}

	for (const auto& pos : entries)
	{
		std::vector <MCHEmul::UByte> entry;
		if (!readDirectoryEntry (pos, entry))
			return (false);

		// A zero file type marks the entry as deleted/free for future directory use.
		entry [0] = MCHEmul::UByte::_0;
		if (!writeDirectoryEntry (pos, entry))
			return (false);
	}

	// Rebuilding the BAM makes the sectors previously owned by scratched files
	// available for later writes.
	if (!rebuildBAMFromDirectory (false))
		return (false);

	setDOSStatus (1, "FILES SCRATCHED", (unsigned char) entries.size (), 0);

	return (true);
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::executeRenameCommand (const std::string& command)
{
	// RENAME only changes the filename bytes in the directory entry. The file
	// chain, type, flags and block count remain untouched.
	if (_d64FileData == nullptr)
	{
		setDOSStatus (74, "DRIVE NOT READY");

		return (false);
	}

	bool validDrive = true;
	std::string payload = MCHEmul::trim (commandPayload (command, &validDrive));
	if (!validDrive)
	{
		setDOSStatus (74, "DRIVE NOT READY");

		return (false);
	}

	// Syntax is R:new=old. The new name must be exact; wildcards are only useful
	// for selecting existing files in other DOS commands.
	size_t eq = payload.find ('=');
	if (eq == std::string::npos)
	{
		setDOSStatus (30, "SYNTAX ERROR");

		return (false);
	}

	std::string newName = MCHEmul::trim (payload.substr (0, eq));
	std::string oldName = MCHEmul::trim (payload.substr (eq + 1));
	if (newName.empty () || oldName.empty () || hasWildcards (newName))
	{
		setDOSStatus (30, "SYNTAX ERROR");

		return (false);
	}

	bool ok = false;
	// Find the source entry before checking the destination, so FILE NOT FOUND
	// has priority when the old name does not exist.
	std::vector <DirectoryEntryPosition> oldEntries =
		findDirectoryEntries (oldName, false, ok);
	if (!ok)
		return (false);
	if (oldEntries.empty ())
	{
		setDOSStatus (62, "FILE NOT FOUND");

		return (false);
	}

	// CBM DOS does not overwrite an existing directory entry during rename.
	if (fileExists (newName))
	{
		setDOSStatus (63, "FILE EXISTS");

		return (false);
	}

	std::vector <MCHEmul::UByte> entry;
	if (!readDirectoryEntry (oldEntries [0], entry))
		return (false);

	// Only bytes 3..18 hold the padded filename, so the rest of the directory
	// entry is preserved exactly.
	std::string paddedName = padPETSCIIName (newName);
	for (size_t i = 0; i < 16; i++)
		entry [3 + i] = MCHEmul::UByte ((unsigned char) paddedName [i]);

	if (!writeDirectoryEntry (oldEntries [0], entry))
		return (false);

	setDOSStatus (0, " OK");

	return (true);
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::executeCopyCommand (const std::string& command)
{
	// COPY creates a new file and leaves every source file untouched. Multiple
	// sources are concatenated in the order requested.
	if (_d64FileData == nullptr)
	{
		setDOSStatus (74, "DRIVE NOT READY");

		return (false);
	}

	bool validDrive = true;
	std::string payload = MCHEmul::trim (commandPayload (command, &validDrive));
	if (!validDrive)
	{
		setDOSStatus (74, "DRIVE NOT READY");

		return (false);
	}

	// Syntax is C:new=source[,source...]. The destination must be a real new
	// filename, not a wildcard expression.
	size_t eq = payload.find ('=');
	if (eq == std::string::npos)
	{
		setDOSStatus (30, "SYNTAX ERROR");

		return (false);
	}

	std::string newName = MCHEmul::trim (payload.substr (0, eq));
	std::string sourceList = payload.substr (eq + 1);
	if (newName.empty () || sourceList.empty () || hasWildcards (newName))
	{
		setDOSStatus (30, "SYNTAX ERROR");

		return (false);
	}

	if (fileExists (newName))
	{
		setDOSStatus (63, "FILE EXISTS");

		return (false);
	}

	// The right side may contain several exact source names separated by commas.
	std::vector <std::string> sourceNames = splitCommandList (sourceList);
	if (sourceNames.empty ())
	{
		setDOSStatus (30, "SYNTAX ERROR");

		return (false);
	}

	// Build the destination payload in memory first. If any source is invalid,
	// no directory entry or sector chain is allocated.
	std::vector <MCHEmul::UByte> newData;
	unsigned char fileType = 0x82;
	bool firstSource = true;
	for (const auto& sourceName : sourceNames)
	{
		if (sourceName.empty () || hasWildcards (sourceName))
		{
			setDOSStatus (30, "SYNTAX ERROR");

			return (false);
		}

		bool ok = false;
		std::vector <DirectoryEntryPosition> sourceEntries =
			findDirectoryEntries (sourceName, false, ok);
		if (!ok)
			return (false);
		if (sourceEntries.empty ())
		{
			setDOSStatus (62, "FILE NOT FOUND");

			return (false);
		}

		std::vector <MCHEmul::UByte> entry;
		if (!readDirectoryEntry (sourceEntries [0], entry))
			return (false);

		if (firstSource)
		{
			fileType = (entry [0].value () & 0x0f) | 0x80;
			firstSource = false;
		}

		std::vector <MCHEmul::UByte> sourceData;
		// Source chains are read with corruption and cycle guards before their data
		// is appended to the new file payload.
		if (!readFilePayloadGuarded
				((size_t) entry [1].value (), (size_t) entry [2].value (), sourceData))
			return (false);

		newData.insert (newData.end (), sourceData.begin (), sourceData.end ());
	}

	// Allocate from a BAM rebuilt from the current directory, so previous command
	// mutations are reflected before choosing free sectors.
	if (!rebuildBAMFromDirectory (false))
		return (false);

	DirectoryEntryPosition newEntryPos;
	if (!findFreeDirectoryEntry (newEntryPos, true))
		return (false);

	if (!rebuildBAMFromDirectory (false))
		return (false);

	size_t firstTrack = 0, firstSector = 0;
	unsigned short blocks = 0;
	if (!allocateFileChain (newData, firstTrack, firstSector, blocks))
		return (false);

	// Create the destination directory entry after the payload chain exists, so
	// the first Track/Sector and block count are known.
	std::vector <MCHEmul::UByte> entry (32, MCHEmul::UByte::_0);
	entry [0] = MCHEmul::UByte (fileType);
	entry [1] = MCHEmul::UByte ((unsigned char) firstTrack);
	entry [2] = MCHEmul::UByte ((unsigned char) firstSector);
	std::string paddedName = padPETSCIIName (newName);
	for (size_t i = 0; i < 16; i++)
		entry [3 + i] = MCHEmul::UByte ((unsigned char) paddedName [i]);
	entry [28] = MCHEmul::UByte ((unsigned char) (blocks & 0xff));
	entry [29] = MCHEmul::UByte ((unsigned char) (blocks >> 8));

	if (!writeDirectoryEntry (newEntryPos, entry))
	{
		rebuildBAMFromDirectory (false);

		return (false);
	}

	if (!rebuildBAMFromDirectory (false))
		return (false);

	setDOSStatus (0, " OK");

	return (true);
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::executeNewCommand (const std::string& command)
{
	// NEW/format rewrites the D64 metadata and clears all sectors. It is a full
	// image-level mutation in this trap-based DOS simulation.
	if (_d64FileData == nullptr)
	{
		setDOSStatus (74, "DRIVE NOT READY");

		return (false);
	}

	bool validDrive = true;
	std::string payload = MCHEmul::trim (commandPayload (command, &validDrive));
	if (!validDrive)
	{
		setDOSStatus (74, "DRIVE NOT READY");

		return (false);
	}

	// Syntax is N:name[,id]. If the id is omitted, real DOS keeps the existing
	// id; this simulation mirrors that by reading the old BAM first.
	size_t comma = payload.find (',');
	std::string diskName =
		MCHEmul::trim (comma == std::string::npos ? payload : payload.substr (0, comma));
	if (diskName.empty ())
	{
		setDOSStatus (30, "SYNTAX ERROR");

		return (false);
	}

	std::string diskID = "";
	if (comma == std::string::npos)
	{
		std::vector <MCHEmul::UByte> oldBAM = sectorData (18, 0).bytes ();
		if (oldBAM.size () == 256)
		{
			diskID += (char) oldBAM [0xa2].value ();
			diskID += (char) oldBAM [0xa3].value ();
		}
		else
			diskID = "  ";
	}
	else
		diskID = MCHEmul::trim (payload.substr (comma + 1));

	// Commit the rebuilt BAM and empty directory, then refresh the memory-block
	// view used by the rest of the emulator.
	if (!_d64FileData -> formatAsEmptyDisk (diskName, diskID))
	{
		setDOSStatus (66, "ILLEGAL TRACK OR SECTOR", 18, 0);

		return (false);
	}

	synchronizeDiskData ();

	setDOSStatus (0, " OK");

	return (true);
}

// ---
unsigned char COMMODORE::Disk1540SeriesSimulation::listen (MCHEmul::CPU* cpu, const MCHEmul::UByte& b)
{ 
	_firmwareStatus = DiskFirmwareStatus::_RECEIVINGCOMMAND; 

	_commandChannel = 0;
							  
	return (definition ()._okResult); 
}

// ---
unsigned char COMMODORE::Disk1540SeriesSimulation::unlisten
	(MCHEmul::CPU* cpu, const MCHEmul::UByte& b)
{
	if (_commandChannel == 15)
		executeCommandChannel ();
	else
		executeOpenChannel ();

	_firmwareStatus = DiskFirmwareStatus::_IDLE;

	return (definition ()._okResult);
}

// ---
unsigned char COMMODORE::Disk1540SeriesSimulation::talk
	(MCHEmul::CPU* cpu, const MCHEmul::UByte& b)
{
	_firmwareStatus = DiskFirmwareStatus::_ANSWERINGCOMMAND;

	_commandChannel = 0;

	// TALK only changes the IEC direction. Every channel keeps its cursor...
	return (definition ()._okResult);
}

// ---
unsigned char COMMODORE::Disk1540SeriesSimulation::untalk
	(MCHEmul::CPU* cpu, const MCHEmul::UByte& b)
{
	_firmwareStatus = DiskFirmwareStatus::_IDLE;

	// Preserve the channel cursor so a later TALK can continue the transfer...
	return (definition ()._okResult);
}

// ---
unsigned char COMMODORE::Disk1540SeriesSimulation::openChannel
	(MCHEmul::CPU* cpu, const MCHEmul::UByte& chn)
{
	static const unsigned char _SECONDARY	= 0x06;
	static const unsigned char _OPEN		= 0x0f;

	// Both IEC OPEN and SECONDARY reach this virtual method.
	// Read the original IEC command to distinguish them.
	unsigned char iecData = cpu -> memoryRef () ->
		value (definition ()._sDataAddress).value ();
	unsigned char command = (iecData >> 4) & 0x0f;

	_commandChannel = chn.value () & 0x0f;

	ChannelState& channel = _channels [_commandChannel];

	if (command == _OPEN)
	{
		// OPEN discards only the previous state of this secondary channel.
		channel.reset (definition ()._okResult);
		channel._open = true;
		channel._pendingOpen =
			(_commandChannel >= 2 && _commandChannel <= 14);
	}
	else if (command == _SECONDARY)
	{
		if (_firmwareStatus == DiskFirmwareStatus::_RECEIVINGCOMMAND)
		{
			channel._pendingOpen = false;

			// LISTEN/SECONDARY starts a new command or output transfer,
			// without closing the selected channel.
			channel._command = "";

			channel.resetAnswer (definition ()._okResult);
		}
		else if (_firmwareStatus == DiskFirmwareStatus::_ANSWERINGCOMMAND &&
				 _commandChannel == 15 &&
				 channel._answerPrepared &&
				 channel._byteFromBlockToAnswerToSend >=
					channel._blockToAnswer.size ())
		{
			// A completed channel-15 status must be rebuilt on the next TALK.
			// Partial answers retain their cursor and continue where they stopped.
			channel.resetAnswer (definition ()._okResult);
		}
	}

	return (definition ()._okResult);
}

// ---
unsigned char COMMODORE::Disk1540SeriesSimulation::closeChannel
	(MCHEmul::CPU* cpu, const MCHEmul::UByte& chn)
{
	_commandChannel = chn.value () & 0x0f;

	ChannelState& channel = _channels [_commandChannel];

	if (_commandChannel == 15)
	{
		// The command channel always remains allocated.
		channel._command = "";
		channel.resetAnswer (definition ()._okResult);
		channel._open = true;

		setDOSStatus (0, " OK");
	}
	else
	{
		// On a real drive the file opened for writing becomes visible when the
		// channel is closed. This trap-based simulation mirrors that moment.
		if (channel._open &&
			channel._mode == ChannelOpenMode::_WRITE)
			writeNewFile
				(channel._fileName, channel._fileType, channel._writeBuffer);

		channel.reset (definition ()._okResult);
	}

	_commandChannel = 0;

	return (definition ()._okResult);
}

// ---
unsigned char COMMODORE::Disk1540SeriesSimulation::sendByte
	(MCHEmul::CPU* cpu, const MCHEmul::UByte& b)
{
	ChannelState& channel = _channels [_commandChannel];

	// While OPEN is pending, received bytes are still the file specification.
	// Once the write channel is open, later LISTEN/SECONDARY bytes are payload.
	if (_firmwareStatus == DiskFirmwareStatus::_RECEIVINGCOMMAND &&
		channel._open &&
		_commandChannel >= 2 && _commandChannel <= 14 &&
		channel._mode == ChannelOpenMode::_WRITE &&
		!channel._pendingOpen)
		channel._writeBuffer.emplace_back (b);
	else
		channel._command +=
			std::string (1, static_cast <char> (b.value ()));

	return (definition ()._okResult);
}

// ---
unsigned char COMMODORE::Disk1540SeriesSimulation::receiveByte
	(MCHEmul::CPU* cpu, MCHEmul::UByte& b)
{
	static const unsigned char _SERIALERROR = 0x02;
	static const unsigned char _ENDOFFILE = 0x40;
	static const unsigned char _READERROR = _SERIALERROR | _ENDOFFILE;

	unsigned char result = _READERROR;
	b = MCHEmul::UByte::_0;

	// The disk can only put bytes on the serial bus after TALK/SECONDARY has
	// selected a channel for answering.
	if (_firmwareStatus != DiskFirmwareStatus::_ANSWERINGCOMMAND)
		return (result);

	ChannelState& channel = _channels [_commandChannel];

	// Reading from a closed secondary address is a DOS-visible FILE NOT OPEN.
	if (!channel._open)
	{
		setDOSStatus (61, "FILE NOT OPEN");

		return (result);
	}

	// Answers are built lazily on first byte request. This keeps OPEN/LISTEN
	// state separate from TALK-time transfer state.
	if (!channel._answerPrepared)
	{
		channel._answerPrepared = true;
		channel._answerStatus = definition ()._okResult;
		channel._blockToAnswer = { };
		channel._byteFromBlockToAnswerToSend = 0;

		// Channel 0/1 commands may still contain drive prefixes or directory filters
		// and are normalized only when the host starts reading.
		std::tuple <std::string, std::string> preAndPostData =
			splitCommandIntoPreAndPostData (channel._command);

		switch (_commandChannel)
		{
			// These channels are reserved for load and save operations...
			case 0:
			case 1:
				{
					// A LOAD without a usable name cannot produce a serial data stream.
					if (std::get <0> (preAndPostData).empty ())
					{
						setDOSStatus (34, "SYNTAX ERROR");

						channel._answerStatus = _READERROR;
					}
					// LOAD"$" is a pseudo-file that returns a BASIC listing of the directory.
					else if (std::get <0> (preAndPostData)[0] ==
						_asciiConverter -> convert ('$').value ())
					{
						if (_d64FileData == nullptr)
						{
							setDOSStatus (74, "DRIVE NOT READY");

							channel._answerStatus = _READERROR;
						}
						else
						{
							setDOSStatus (0, " OK");

							channel._blockToAnswer =
								std::move (buildAnswerToDirCommand
									(splitCommandIntoPreAndPostData
										(channel._command.length () == 1
											? ":*" :
											channel._command.substr (1))));

							channel._byteFromBlockToAnswerToSend = 0;
						}
					}
					else
					{
						// Normal LOAD reads a file by name. The helper strips the optional drive
						// prefix without disturbing the special directory path above.
						bool validDrive = true;
						std::string fileName =
							fileNameFromFileSpec (channel._command, &validDrive);

						if (!validDrive || fileName.empty ())
						{
							setDOSStatus (34, "SYNTAX ERROR");

							channel._answerStatus = _READERROR;
						}
						else
						{
							channel._blockToAnswer =
								std::move (buildAnswerToFileCommand
									(std::make_tuple (fileName, std::string (""))));

							channel._byteFromBlockToAnswerToSend = 0;
						}
					}
				}

				break;

			// These channels are reserved for data information...
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
				{
					// Data channels are prepared by OPEN. Reading an unprepared data channel
					// is still outside the implemented streaming-read surface.
					setDOSStatus (70, "NO CHANNEL");
					channel._answerStatus = _READERROR;

					_LOG ("Disk1540SeriesSimulation: Channel " +
						std::to_string (_commandChannel) +
						" is reserved for data, but it is not implemented yet.");
				}

				break;

			// When read, channel 15 returns the persistent DOS status...
			case 15:
				{
					// Channel 15 returns a snapshot of the persistent DOS status. Once fully
					// transmitted, the next status read falls back to OK.
					channel._blockToAnswer = _lastDOSStatus;
					channel._byteFromBlockToAnswerToSend = 0;
					channel._clearDOSStatusAfterAnswer = true;
				}

				break;
		}

		// Empty answers are represented to the KERNAL as SERIAL ERROR | EOF.
		if (channel._blockToAnswer.empty ())
			channel._answerStatus = _READERROR; // SERIAL ERROR | EOF
	}

	// Errors detected while preparing the answer are returned before any byte
	// transfer attempt.
	if (channel._answerStatus != definition ()._okResult)
		result = channel._answerStatus;
	else if (channel._byteFromBlockToAnswerToSend >=
			 channel._blockToAnswer.size ())
		result = _READERROR;
	else
	{
		// Send one byte and advance the per-channel cursor so repeated TALKs can
		// continue the same transfer.
		b = channel._blockToAnswer
			[channel._byteFromBlockToAnswerToSend++];

		// The final byte is delivered together with EOF, matching what KERNAL LOAD
		// and INPUT# expect from the serial status byte.
		if (channel._byteFromBlockToAnswerToSend ==
			channel._blockToAnswer.size ())
		{
			result = _ENDOFFILE;

			if (_commandChannel == 15 &&
				channel._clearDOSStatusAfterAnswer)
			{
				// The transmitted copy remains untouched. Only the next
				// channel-15 answer becomes OK.
				setDOSStatus (0, " OK");
				channel._clearDOSStatusAfterAnswer = false;
			}
		}
		else
			result = definition ()._okResult;
	}

	return (result);
}

// ---
MCHEmul::DataMemoryBlock COMMODORE::Disk1540SeriesSimulation::dataBlockPerTrackAndSector 
	(size_t track, size_t sector) const
{
	MCHEmul::DataMemoryBlock result;

	// The attributes TRACK and SECTOR must exist...
	// ...but it is not checked!!

	for (size_t i = 0; i < _data._data.size (); i++)
		if (_data._data [i].attribute ("TRACK") == std::to_string (track) &&
			_data._data [i].attribute ("SECTOR") == std::to_string (sector))
			return (_data._data [i]);

	return (result);
}

// ---
MCHEmul::DataMemoryBlocks COMMODORE::Disk1540SeriesSimulation::dataBlocksPerTrack (size_t track) const
{
	MCHEmul::DataMemoryBlocks result;

	// The attribute TRACK must exist...
	// ...but it is not checked!!

	for (size_t i = 0; i < _data._data.size (); i++)
		if (_data._data [i].attribute ("TRACK") == std::to_string (track))
			result.emplace_back (_data._data [i]);

	return (result);
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::isTrackAndSectorValid
	(size_t track, size_t sector) const
{
	return (_d64FileData != nullptr &&
		track <= 0xff && sector <= 0xff &&
		_d64FileData -> isTrackAndSectorValid
			((unsigned char) track, (unsigned char) sector));
}

// ---
MCHEmul::UBytes COMMODORE::Disk1540SeriesSimulation::sectorData
	(size_t track, size_t sector) const
{
	return (isTrackAndSectorValid (track, sector)
		? _d64FileData -> sectorData
			((unsigned char) track, (unsigned char) sector)
		: MCHEmul::UBytes::_E);
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::setSectorData
	(size_t track, size_t sector, const MCHEmul::UBytes& data)
{
	bool result =
		(_d64FileData != nullptr &&
		 track <= 0xff && sector <= 0xff &&
		 _d64FileData -> setSectorData
			((unsigned char) track, (unsigned char) sector, data));
	if (result)
		synchronizeDiskData ();

	return (result);
}

// ---
void COMMODORE::Disk1540SeriesSimulation::synchronizeDiskData ()
{
	if (_d64FileData != nullptr)
		_data = _d64FileData -> asMemoryBlocks ();
	else
		_data = MCHEmul::ExtendedDataMemoryBlocks { };
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::nameMatchesWithPattern
	(const std::string& n, const std::string& p) const
{
	size_t pL = std::min (p.length (), (size_t) 16);
	for (size_t i = 0; i < pL; i++)
	{
		if (p [i] == '*')
			return (true);
		if (i >= n.length ())
			return (false);
		if (p [i] == '?')
			continue;
		if (p [i] != n [i])
			return (false);
	}

	return (n.length () == pL);
}

// ---
std::vector <MCHEmul::UByte> COMMODORE::Disk1540SeriesSimulation::buildAnswerToDirCommand 
	(const std::tuple <const std::string, const std::string>& prm) const
{
	// ---
	// Constants used in the building of the answer to the command "$", 
	// that is the command to ask for the directory of the disk...
	static const MCHEmul::UByte _QUOTE_PETSCII = _asciiConverter -> convert ('"');
	static const MCHEmul::UByte _SPACE_PETSCII = _asciiConverter -> convert (' ');
	static const std::array <std::vector <MCHEmul::UByte>, 5> _EXTENSIONS_PETSCII = { 
		_asciiConverter -> convert ("DEL"), _asciiConverter -> convert ("SEQ"),
		_asciiConverter -> convert ("PRG"), _asciiConverter -> convert ("USR"), 
		_asciiConverter -> convert ("REL") };
	static const std::vector <MCHEmul::UByte> _SPACES_PETSCII (50, _asciiConverter -> convert (' '));
	static const MCHEmul::UByte _LESSER_PETSCII = _asciiConverter -> convert ('<');
	static const MCHEmul::UByte _ASTERISK_PETSCII = _asciiConverter -> convert ('*');
	static const std::vector <MCHEmul::UByte> _BLOCKS_PETSCII = _asciiConverter -> convert ("BLOCKS FREE.");
	// ---

	std::vector <MCHEmul::UByte> result;

	// Number of free blocks, that will be added at the end...
	size_t nfb = 0; 
	// Starts from where the BAM is (in the track 18)...
	size_t st = 0;
	// But if nothing was loaded...
	MCHEmul::DataMemoryBlocks dt = std::move (dataBlocksPerTrack (18));
	if (dt.empty ()) 
		return (result); // ...the system returns with nothing!

	bool e = false;
	// The first direction where to load the information of the directory...
	// From that position the rest is loaded...
	MCHEmul::Address iA = _definition._dirAddress;
	while (!e)
	{
		// Reads the information of the directory...
		if (st == 0)
		{
			// The sector 0 is the BAM, 
			// where the information about the disk is...
	
			// The two first bytes of the answer must point to where to load
			// the information with in the basic memory!
			result.emplace_back (iA.bytes ()[1]); // low...
			result.emplace_back (iA.bytes ()[0]); // high...

			// Calculate the number of free blocks...
			// that information will be added at the end of the directory later...
			// But as we are now in the BAM, it is time to get that info!
			// The information at track 18 is not considered, because it is the BAM!
			for (size_t i = 0x04; i <= 0x8f; i += 4)
				nfb += (i == 0x48 /** = 0x04 * 17 (= 18 when From 0) + 0x04. */) 
					? 0 : (size_t) dt [st].byte (i).value ();

			// Reads where the name of the disk is...
			// ...but it can be empty what means that there is no entry at all...
			// ...and probably the sector reading should finish (see below)...
			std::vector <MCHEmul::UByte> n { };
			n.push_back (0x12); // RVS ON (PETSCII symbol)...
			n.push_back (_QUOTE_PETSCII);
			unsigned char nc = 0xa0;
			for (size_t j = 0x90; j <= 0x9f; j++) 
				{ nc = dt [st].byte (j).value (); 
				  n.push_back ((nc == 0xa0) ? _SPACE_PETSCII : MCHEmul::UByte (nc)); }
			n.push_back (_QUOTE_PETSCII);
			n.push_back (_SPACE_PETSCII);
			n.push_back ((dt [st].byte (0xa2) != 0xa0) 
				? dt [st].byte (0xa2) : _SPACE_PETSCII);
			n.push_back (dt [st].byte (0xa3).value () != 0xa0
				? dt [st].byte (0xa3) : _SPACE_PETSCII);
			n.push_back (_SPACE_PETSCII);
			n.push_back ((dt [st].byte (0xa5) != 0xa0)
				? dt [st].byte (0xa5) : _SPACE_PETSCII);
			n.push_back ((dt [st].byte (0xa6) != 0xa0)
				? dt [st].byte (0xa6) : _SPACE_PETSCII);
			// The RVS OFF doesn't exist...

			// Save the info of the entry...
			// Where the next entry will be...
			iA += n.size () + 5;
			// 1.- Saves the position of the next entry...
			result.insert (result.end (), { iA.bytes ()[1], iA.bytes ()[0] });
			// 2.- Saves the size of in blocks of the entry...
			result.insert (result.end (), { MCHEmul::UByte::_0, MCHEmul::UByte::_0 });
			// 3.- Saves the name of the disk...
			result.insert (result.end (), n.begin (), n.end ());
			// 4.- Finally a 0 to indicate the end of this entry...
			result.emplace_back (MCHEmul::UByte::_0);

			// Really where the directory is stored in the BAM...
			// ...it is usually track 18 and sector 1, but...
			MCHEmul::DataMemoryBlocks odt = dt;
			dt = std::move (dataBlocksPerTrack ((size_t) odt [st].byte (0).value ()));
			st = (size_t) odt [st].byte (1).value ();
		}
		else
		{
			// Iterate over all 8 entries in every sector.
			// Empty or deleted entries are skipped because valid entries may follow them.
			// Every entry is 32 bytes long...
			for (size_t i = 0; i < 8; i++)
			{
				if (dt [st].byte ((i << 5) + 0x02).value () == 0x00)
					continue; // Deleted or unused entry...

				size_t nfbt = 
					 (size_t) dt [st].byte ((i << 5) + 0x1e).value () +
					((size_t) dt [st].byte ((i << 5) + 0x1f).value () << 8);
				std::string nfbtstr = MCHEmul::trim (std::to_string (nfbt)); // Just to calculae the length...

				// Read the entry name up to its first $a0 padding byte...
				unsigned char nc = 0xa0;
				std::string nStr = "";
				std::vector <MCHEmul::UByte> n (4 - nfbtstr.length (), _SPACE_PETSCII); 
				n.push_back (_QUOTE_PETSCII); // The file name starts with a quote...
				size_t j = 0x05;
				for (;j < 0x15 && 
					 (nc = dt [st].byte ((i << 5) + j).value ()) != 0xa0; j++)
				{ 
					MCHEmul::UByte nChr = 
						(nc == 0xa0) ? _SPACE_PETSCII : MCHEmul::UByte (nc);
					nStr += (char) nChr.value ();
					n.push_back (nChr);
				}

				n.push_back (_QUOTE_PETSCII);

				// Skip entries without a name or not matching the pattern...
				if (j == 0x05 || 
					!nameMatchesWithPattern (nStr, std::get <1> (prm)))
					continue;

				// Calculates the number of blocks of the line...
				// and also in string format, 
				// because it is crucial to determine how many spaces to add at the end of the name...
				unsigned short nblks = 
					 (unsigned short) dt [st].byte ((i << 5) + 0x1e).value () +
					((unsigned short) dt [st].byte ((i << 5) + 0x1f).value () << 8);
				std::string nblksstr = MCHEmul::trim (std::to_string (nblks)); // Just to calculate the length...

				// Complete the entry with spaces if needed...
				// ...and also the type of file...
				// Add the spaces at the end...
				n.insert (n.end (), 
					_SPACES_PETSCII.begin (),
					_SPACES_PETSCII.begin () + 23 - (nblksstr.length ()) - n.size ());

				unsigned char tp = dt [st].byte ((i << 5) + 0x02).value ();
				// An unclosed file is marked before its type...
				n.push_back (((tp & (1 << 7)) == 0)
					? _ASTERISK_PETSCII : _SPACE_PETSCII);
				switch (tp & 0x0f) // The pure type...
				{
					case 0b0000: n.insert (n.end (),
						_EXTENSIONS_PETSCII [0].begin (), _EXTENSIONS_PETSCII [0].end ()); break;
					case 0b0001: n.insert (n.end (),
						_EXTENSIONS_PETSCII [1].begin (), _EXTENSIONS_PETSCII [1].end ()); break;
					case 0b0010: n.insert (n.end (),
						_EXTENSIONS_PETSCII [2].begin (), _EXTENSIONS_PETSCII [2].end ()); break;
					case 0b0011: n.insert (n.end (),
						_EXTENSIONS_PETSCII [3].begin (), _EXTENSIONS_PETSCII [3].end ()); break;
					case 0b0100: n.insert (n.end (),
						_EXTENSIONS_PETSCII [4].begin (), _EXTENSIONS_PETSCII [4].end ()); break;
					default: break; // Not supported...
				}
				// A locked file is marked after its type...
				n.push_back (((tp & (1 << 6)) != 0)
					? _LESSER_PETSCII : _SPACE_PETSCII);

				n.insert (n.end (), 
					_SPACES_PETSCII.begin (), 
					_SPACES_PETSCII.begin () + 27 - n.size ()); // To complete the max length...

				// Calculates the where the next info must be stored...
				// 5 + len of the name when definetively there is something!
				iA += n.size () + 5;
				// Saves the info of the entry...
				// Saves the position of the next entry...
				result.insert (result.end (), { iA.bytes ()[1], iA.bytes ()[0] });
				// Saves the size in blocks (as the number of line)...
				result.insert (result.end (), 
					{ dt [st].byte ((i << 5) + 0x1e), dt [st].byte ((i << 5) + 0x1f) });
				// Saves the name of the entry...
				result.insert (result.end (), n.begin (), n.end ());
				// Finally a double 0 to indicate the end of this entry...
				result.emplace_back (MCHEmul::UByte::_0);
			}

			// If next next track pointed is 0...
			// It would mean that it is the last sector with dir info...
			// If not, a new track and sector must be loaded...
			if (!(e = (dt [st].byte (0) == MCHEmul::UByte::_0)))
			{
				size_t ost = st;
				dt = std::move (dataBlocksPerTrack ((size_t) (dt [ost].byte (0).value ())));
				st = (size_t) dt [ost].byte (1).value ();
			}
		}
	}

	// Now it is time to build the final name...
	std::vector <MCHEmul::UByte> n = _BLOCKS_PETSCII;
	n.insert (n.end (), 
		_SPACES_PETSCII.begin (), 
		_SPACES_PETSCII.begin () + 25 - n.size ()); // To complete the max length...

	// Calculates the where the next info must be stored...
	// 5 + len of the name when definetively there is something!
	iA += n.size () + 5;
	// Saves the info of the entry...
	// Saves the position of the next entry...
	result.insert (result.end (), { iA.bytes ()[1], iA.bytes ()[0] });
	// Puts the number of free blocks...
	// The number of free blocks is the number of the line...
	result.insert (result.end (), { (unsigned char) (nfb % 0x100), (unsigned char) (nfb / 0x100) });
	// Then the text...
	result.insert (result.end (), n.begin (), n.end ());
	// The block is end...
	result.emplace_back (MCHEmul::UByte::_0);

	// And there is no more entries...
	result.insert (result.end (), { MCHEmul::UByte::_0, MCHEmul::UByte::_0 });

	return (result);
}

// ---
std::vector <MCHEmul::UByte>
	COMMODORE::Disk1540SeriesSimulation::buildAnswerToFileCommand
		(const std::tuple <const std::string, const std::string>& prm)
{
	if (_d64FileData == nullptr)
	{
		setDOSStatus (74, "DRIVE NOT READY");

		return (std::vector <MCHEmul::UByte> { });
	}

	size_t t = 0, s = 0;

	// Get the file wich name is received as parameter, with or without wildcards
	// Get then the track and the sector where the file starts...
	std::tie (t, s) =
		getInitialTrackAndSectorOfFile (std::get <0> (prm));
	if (t == 0 && s == 0)
	{
		setDOSStatus (62, "FILE NOT FOUND");

		return (std::vector <MCHEmul::UByte> { });
	}

	setDOSStatus (0, " OK");

	return (getDataOfFileFromTrackAndSector (t, s));
}

// ---
std::tuple <size_t, size_t> 
COMMODORE::Disk1540SeriesSimulation::getInitialTrackAndSectorOfFile (const std::string& fN) const
{
	size_t t = 0, s = 0;

	// All file info is n the track 18, until found...
	bool f = false;
	const auto& dE = _d64FileData -> directoryEntries ();
	for (size_t i = 0; i < dE.size () && !f; i++)
	{
		for (size_t j = 0; j < 8 && !f; j++)
		{
			if (dE [i][j]._fileName.empty ())
				continue;

			if (f = nameMatchesWithPattern (dE [i][j]._fileName, fN))
			{
				t = dE [i][j]._startTrack;
				s = dE [i][j]._startSector;
			}
		}
	}

	// If no found the track and sector returned will be 0...
	return (std::make_tuple (t, s));
}

// ---
std::vector <MCHEmul::UByte> 
COMMODORE::Disk1540SeriesSimulation::getDataOfFileFromTrackAndSector (size_t t, size_t s) const
{
	std::vector <MCHEmul::UByte> result;

	// Get the info of the right track and sector...
	std::vector <MCHEmul::UByte> dt = dataBlockPerTrackAndSector (t, s).bytes ();
	if (dt.size () != 256) // It shouldn't happen, just in case...
	{
		_LOG ("Disk1540SeriesSimulation: Invalid track or sector.");

		return (result);
	}
	// If there were no more data in ithers tracks and sector...
	// ...the byte 0 of the data retrieved would be 0, and the byte 1 will ponted
	// the size in daa of this last block...
	if (dt [0] == 0)
	{
		size_t lastByte = (size_t) dt [1].value ();
		if (lastByte == 0)
		{
			_LOG ("Disk1540SeriesSimulation: Invalid last sector length.");

			return (result);
		}

		result.insert (result.end (),
			dt.begin () + 2, dt.begin () + lastByte + 1);
	}
	// In other case, bytes 0 and 1 will hold the next track and sector to read, 
	// and the data will be all the rest of the block...
	else
	{
		result.insert (result.end (), dt.begin () + 2, dt.end ());
		std::vector <MCHEmul::UByte> next = // Notice that is recursive...
			std::move (getDataOfFileFromTrackAndSector (dt [0].value (), dt [1].value ()));
		result.insert (result.end (), next.begin (), next.end ());
	}

	return (result);
}
