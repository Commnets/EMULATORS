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
	return (_d64FileData == nullptr
		? nullptr
		: new COMMODORE::D64FileData (*_d64FileData));
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
		return;

	channel._pendingOpen = false;

	if (_commandChannel < 2 || _commandChannel > 14)
		return;

	bool validDrive = true;
	std::string fileName = fileNameFromFileSpec (channel._command, &validDrive);
	if (!validDrive || fileName.empty ())
	{
		setDOSStatus (34, "SYNTAX ERROR");

		channel._answerPrepared = true;
		channel._answerStatus = _READERROR;

		return;
	}

	channel._blockToAnswer =
		std::move (buildAnswerToFileCommand
			(std::make_tuple (fileName, std::string (""))));
	channel._answerPrepared = true;
	channel._answerStatus =
		channel._blockToAnswer.empty () ? _READERROR : definition ()._okResult;
	channel._byteFromBlockToAnswerToSend = 0;
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::executeInitializeCommand ()
{
	if (_d64FileData == nullptr)
	{
		setDOSStatus (74, "DRIVE NOT READY");

		return (false);
	}

	synchronizeDiskData ();

	for (auto& channel : _channels)
		channel.reset (definition ()._okResult);
	_channels [15]._open = true;

	setDOSStatus (0, " OK");

	return (true);
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::executeValidateCommand ()
{
	return (rebuildBAMFromDirectory (true));
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::rebuildBAMFromDirectory (bool setStatus)
{
	static const size_t _BAMTRACK	= 18;
	static const size_t _BAMSECTOR	= 0;
	static const size_t _BAMENTRIES	= 35;

	if (_d64FileData == nullptr)
	{
		setDOSStatus (74, "DRIVE NOT READY");

		return (false);
	}

	if (!isTrackAndSectorValid (_BAMTRACK, _BAMSECTOR))
	{
		setDOSStatus (74, "DRIVE NOT READY");

		return (false);
	}

	std::vector <std::vector <bool>> used
		((size_t) _d64FileData -> _numberTracks + 1);
	for (size_t i = 1; i <= (size_t) _d64FileData -> _numberTracks; i++)
		used [i] = std::vector <bool>
			(_d64FileData -> _tracksData [i - 1] -> _trackInfo._sectors, false);

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

	if (!markSectorAsUsed (_BAMTRACK, _BAMSECTOR))
		return (false);

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
			visited =
				(visitedDirTracks [i] == dirTrack &&
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
	std::string uCommand = command;
	for (auto& i : uCommand)
		if (i >= 'a' && i <= 'z')
			i = (char) (i - ('a' - 'A'));

	static const std::vector <std::string> _COMMANDS =
		{ "INITIALIZE", "VALIDATE", "SCRATCH", "RENAME", "COPY", "NEW" };
	for (const auto& i : _COMMANDS)
		if (uCommand.length () >= i.length () &&
			uCommand.substr (0, i.length ()) == i)
			return (i.length ());

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
		return ("");

	std::string result = command.substr (commandLength);
	if (result.length () >= 2 && result [1] == ':')
	{
		if (validDrive != nullptr)
			*validDrive = (result [0] == '0');

		return (result.substr (2));
	}

	if (!result.empty () && result [0] == ':')
		return (result.substr (1));

	return (result);
}

// ---
std::string COMMODORE::Disk1540SeriesSimulation::fileNameFromFileSpec
	(const std::string& fileSpec, bool* validDrive) const
{
	if (validDrive != nullptr)
		*validDrive = true;

	std::string result = MCHEmul::trim (fileSpec);

	if (!result.empty () && result [0] == ':')
		result = result.substr (1);

	if (result.length () >= 2 && result [1] == ':')
	{
		if (validDrive != nullptr)
			*validDrive = (result [0] == '0');

		result = result.substr (2);
	}

	size_t commaPos = result.find (',');
	if (commaPos != std::string::npos)
		result = result.substr (0, commaPos);

	return (MCHEmul::trim (result));
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
	std::string result = name.substr (0, (name.length () < 16) ? name.length () : 16);
	while (result.length () < 16)
		result += (char) 0xa0;

	return (result);
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::hasWildcards (const std::string& name) const
{
	return (name.find ('*') != std::string::npos ||
		name.find ('?') != std::string::npos);
}

// ---
std::vector <COMMODORE::Disk1540SeriesSimulation::DirectoryEntryPosition>
	COMMODORE::Disk1540SeriesSimulation::findDirectoryEntries
		(const std::string& pattern, bool wildcards, bool& ok)
{
	std::vector <DirectoryEntryPosition> result;
	ok = false;

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
			size_t pos = (i << 5) + 0x02;
			if (dir [pos].value () == 0x00)
				continue;

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

	ok = true;

	return (result);
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::findFreeDirectoryEntry
	(DirectoryEntryPosition& pos, bool createDirectorySector)
{
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

	if (!createDirectorySector || lastTrack == 0)
	{
		setDOSStatus (72, "DISK FULL");

		return (false);
	}

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

	lastDir [0] = MCHEmul::UByte (18);
	lastDir [1] = MCHEmul::UByte ((unsigned char) newSector);
	if (!setSectorData (lastTrack, lastSector, MCHEmul::UBytes (lastDir)))
		return (false);

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
	entry = { };
	std::vector <MCHEmul::UByte> dir = sectorData (pos._track, pos._sector).bytes ();
	if (dir.size () != 256 || (pos._offset + 31) >= dir.size ())
	{
		setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
			(unsigned char) pos._track, (unsigned char) pos._sector);

		return (false);
	}

	entry.insert (entry.end (), dir.begin () + pos._offset,
		dir.begin () + pos._offset + 32);

	return (true);
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::writeDirectoryEntry
	(const DirectoryEntryPosition& pos, const std::vector <MCHEmul::UByte>& entry)
{
	if (entry.size () != 32)
	{
		setDOSStatus (30, "SYNTAX ERROR");

		return (false);
	}

	std::vector <MCHEmul::UByte> dir = sectorData (pos._track, pos._sector).bytes ();
	if (dir.size () != 256 || (pos._offset + 31) >= dir.size ())
	{
		setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
			(unsigned char) pos._track, (unsigned char) pos._sector);

		return (false);
	}

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
	data = { };
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
	firstTrack = firstSector = 0;
	blocks = 0;

	std::vector <MCHEmul::UByte> bam = sectorData (18, 0).bytes ();
	if (bam.size () != 256)
	{
		setDOSStatus (66, "ILLEGAL TRACK OR SECTOR", 18, 0);

		return (false);
	}

	size_t neededBlocks = data.empty () ? 1 : ((data.size () + 253) / 254);
	std::vector <size_t> tracks;
	std::vector <size_t> sectors;
	size_t maxTrack =
		((size_t) _d64FileData -> _numberTracks < 35)
			? (size_t) _d64FileData -> _numberTracks
			: 35;
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

	if (tracks.size () < neededBlocks)
	{
		setDOSStatus (72, "DISK FULL");

		return (false);
	}

	size_t offset = 0;
	for (size_t i = 0; i < neededBlocks; i++)
	{
		std::vector <MCHEmul::UByte> sectorBytes (256, MCHEmul::UByte::_0);
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

		if (!_d64FileData -> setSectorData
				((unsigned char) tracks [i], (unsigned char) sectors [i],
				 MCHEmul::UBytes (sectorBytes)))
		{
			setDOSStatus (66, "ILLEGAL TRACK OR SECTOR",
				(unsigned char) tracks [i], (unsigned char) sectors [i]);

			return (false);
		}
	}

	firstTrack = tracks [0];
	firstSector = sectors [0];
	blocks = (unsigned short) neededBlocks;
	synchronizeDiskData ();

	return (true);
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::executeScratchCommand (const std::string& command)
{
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

		entry [0] = MCHEmul::UByte::_0;
		if (!writeDirectoryEntry (pos, entry))
			return (false);
	}

	if (!rebuildBAMFromDirectory (false))
		return (false);

	setDOSStatus (1, "FILES SCRATCHED", (unsigned char) entries.size (), 0);

	return (true);
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::executeRenameCommand (const std::string& command)
{
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
	std::vector <DirectoryEntryPosition> oldEntries =
		findDirectoryEntries (oldName, false, ok);
	if (!ok)
		return (false);
	if (oldEntries.empty ())
	{
		setDOSStatus (62, "FILE NOT FOUND");

		return (false);
	}

	if (fileExists (newName))
	{
		setDOSStatus (63, "FILE EXISTS");

		return (false);
	}

	std::vector <MCHEmul::UByte> entry;
	if (!readDirectoryEntry (oldEntries [0], entry))
		return (false);

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

	std::vector <std::string> sourceNames = splitCommandList (sourceList);
	if (sourceNames.empty ())
	{
		setDOSStatus (30, "SYNTAX ERROR");

		return (false);
	}

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
		if (!readFilePayloadGuarded
				((size_t) entry [1].value (), (size_t) entry [2].value (), sourceData))
			return (false);

		newData.insert (newData.end (), sourceData.begin (), sourceData.end ());
	}

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

	std::vector <MCHEmul::UByte> emptySector (256, MCHEmul::UByte::_0);
	for (unsigned char track = 1; track <= _d64FileData -> _numberTracks; track++)
		for (unsigned char sector = 0;
				sector < _d64FileData -> _tracksData [track - 1] -> _trackInfo._sectors;
				sector++)
			_d64FileData -> setSectorData
				(track, sector, MCHEmul::UBytes (emptySector));

	std::vector <MCHEmul::UByte> bam (256, MCHEmul::UByte::_0);
	bam [0] = MCHEmul::UByte (18);
	bam [1] = MCHEmul::UByte (1);
	bam [2] = MCHEmul::UByte ('A');

	size_t maxBAMTrack =
		((size_t) _d64FileData -> _numberTracks < 35)
			? (size_t) _d64FileData -> _numberTracks
			: 35;
	for (size_t track = 1; track <= maxBAMTrack; track++)
	{
		size_t bamEntry = 0x04 + ((track - 1) << 2);
		unsigned char freeSectors = 0;
		for (size_t sector = 0;
				sector < _d64FileData -> _tracksData [track - 1] -> _trackInfo._sectors;
				sector++)
		{
			bool freeSector = !(track == 18 && (sector == 0 || sector == 1));
			if (!freeSector)
				continue;

			freeSectors++;
			bam [bamEntry + 1 + (sector >> 3)].setBit (sector & 0x07, true);
		}

		bam [bamEntry] = MCHEmul::UByte (freeSectors);
	}

	std::string paddedName = padPETSCIIName (diskName);
	for (size_t i = 0; i < 16; i++)
		bam [0x90 + i] = MCHEmul::UByte ((unsigned char) paddedName [i]);

	bam [0xa0] = MCHEmul::UByte (0xa0);
	bam [0xa1] = MCHEmul::UByte (0xa0);
	bam [0xa2] = MCHEmul::UByte
		((unsigned char) (diskID.length () > 0 ? diskID [0] : 0xa0));
	bam [0xa3] = MCHEmul::UByte
		((unsigned char) (diskID.length () > 1 ? diskID [1] : 0xa0));
	bam [0xa4] = MCHEmul::UByte (0xa0);
	bam [0xa5] = MCHEmul::UByte ('2');
	bam [0xa6] = MCHEmul::UByte ('A');
	bam [0xa7] = MCHEmul::UByte (0xa0);

	std::vector <MCHEmul::UByte> directory (256, MCHEmul::UByte::_0);
	directory [0] = MCHEmul::UByte::_0;
	directory [1] = MCHEmul::UByte::_FF;

	_d64FileData -> setSectorData (18, 0, MCHEmul::UBytes (bam));
	_d64FileData -> setSectorData (18, 1, MCHEmul::UBytes (directory));
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
		channel.reset (definition ()._okResult);

	_commandChannel = 0;

	return (definition ()._okResult);
}

// ---
unsigned char COMMODORE::Disk1540SeriesSimulation::sendByte
	(MCHEmul::CPU* cpu, const MCHEmul::UByte& b)
{
	// The byte belongs to the secondary channel selected by OPEN or SECONDARY.
	_channels [_commandChannel]._command +=
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

	if (_firmwareStatus != DiskFirmwareStatus::_ANSWERINGCOMMAND)
		return (result);

	ChannelState& channel = _channels [_commandChannel];

	if (!channel._open)
	{
		setDOSStatus (61, "FILE NOT OPEN");

		return (result);
	}

	if (!channel._answerPrepared)
	{
		channel._answerPrepared = true;
		channel._answerStatus = definition ()._okResult;
		channel._blockToAnswer = { };
		channel._byteFromBlockToAnswerToSend = 0;

		std::tuple <std::string, std::string> preAndPostData =
			splitCommandIntoPreAndPostData (channel._command);

		switch (_commandChannel)
		{
			// These channels are reserved for load and save operations...
			case 0:
			case 1:
				{
					if (std::get <0> (preAndPostData).empty ())
					{
						setDOSStatus (34, "SYNTAX ERROR");

						channel._answerStatus = _READERROR;
					}
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
					channel._blockToAnswer = _lastDOSStatus;
					channel._byteFromBlockToAnswerToSend = 0;
					channel._clearDOSStatusAfterAnswer = true;
				}

				break;
		}

		if (channel._blockToAnswer.empty ())
			channel._answerStatus = _READERROR; // SERIAL ERROR | EOF
	}

	if (channel._answerStatus != definition ()._okResult)
		result = channel._answerStatus;
	else if (channel._byteFromBlockToAnswerToSend >=
			 channel._blockToAnswer.size ())
		result = _READERROR;
	else
	{
		b = channel._blockToAnswer
			[channel._byteFromBlockToAnswerToSend++];

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
