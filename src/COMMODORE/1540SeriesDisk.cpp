#include <COMMODORE/1540SeriesDisk.hpp>
#include <F6500/C6500.hpp>

// ---
COMMODORE::Disk1540SeriesSimulation::Disk1540SeriesSimulation 
		(int id, unsigned char dN, 
		 const MCHEmul::ASCIIToCodeConverter* cnv,
		 const COMMODORE::SerialIOPeripheralSimulation::Definition& def)
	: COMMODORE::SerialIOPeripheralSimulation (id, dN,
		def,
		{ { "Name", "1540/1541/1571/1570/1581 Disk Injection" },
		  { "Manufacturer", "ICF 1540/1541/1571/1570/1581 Software Simulation" } }),
	  _asciiConverter (cnv),
	  _data { },
	  _currentCommand (""),
	  _firmwareStatus (DiskFirmwareStatus::_IDLE),
	  _blockToAnswer { },
	  _byteFromBlockToAnswerToSend (0)
{ 
	// The ascii conververter is mandatory
	// it is used in many blocks of the conversion...
	assert (_asciiConverter != nullptr);

	setClassName ("Disk1540Simulation");
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::connectData (MCHEmul::FileData* dt)
{
	if ((_d64FileData = dynamic_cast <COMMODORE::D64FileData*> (dt)) == nullptr)
		return (false); // These formats are the only ones accepted...

	_data = dt -> asMemoryBlocks ();

	// Just in case...
	_currentCommand = { };
	_firmwareStatus = DiskFirmwareStatus::_IDLE;
	_blockToAnswer	= { };

	return (true);
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
			dSA.add ("FBYTES", _data._data [i].bytes ()); // All bytes...
			dSA.add ("BYTES", std::vector <MCHEmul::UByte> 
				(_data._data [i].bytes ().begin (), _data._data [i].bytes ().begin () + 256));
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
unsigned char COMMODORE::Disk1540SeriesSimulation::listen (MCHEmul::CPU* cpu, const MCHEmul::UByte& b)
{ 
	_firmwareStatus = DiskFirmwareStatus::_RECEIVINGCOMMAND; 

	// Start to listen a new command...
	_currentCommand = "";
							  
	return (definition ()._okResult); 
}

// ---
unsigned char COMMODORE::Disk1540SeriesSimulation::unlisten (MCHEmul::CPU* cpu, const MCHEmul::UByte& b)
{ 
	_firmwareStatus = DiskFirmwareStatus::_IDLE; 
							  
	return (definition ()._okResult);
}

// ---
unsigned char COMMODORE::Disk1540SeriesSimulation::talk (MCHEmul::CPU* cpu, const MCHEmul::UByte& b)
{ 
	_firmwareStatus = DiskFirmwareStatus::_ANSWERINGCOMMAND; 

	// What is going to be used is a new command
	// once it is listing fully!
	_blockToAnswer = { };
							  
	return (definition ()._okResult);
}

// ---
unsigned char COMMODORE::Disk1540SeriesSimulation::untalk (MCHEmul::CPU* cpu, const MCHEmul::UByte& b)
{ 
	_firmwareStatus = DiskFirmwareStatus::_IDLE; 
							  
	return (definition ()._okResult);
}

// ---
unsigned char COMMODORE::Disk1540SeriesSimulation::openChannel (MCHEmul::CPU* cpu, const MCHEmul::UByte& chn)
{ 
	// TODO

	// Any case the channel can be oppen whether the firmware status
	// is receiving a command or sending information back...

	return (definition ()._okResult);
}

// ---
unsigned char COMMODORE::Disk1540SeriesSimulation::closeChannel (MCHEmul::CPU* cpu, const MCHEmul::UByte& chn)
{ 
	// TODO

	return (definition ()._okResult);
}

// ---
unsigned char COMMODORE::Disk1540SeriesSimulation::sendByte (MCHEmul::CPU* cpu, const MCHEmul::UByte& b)
{
	// When the disk is receiving bytes is because
	// there is a instruction under construction...
	_currentCommand += std::string (1, (char) b.value ());

	return (definition ()._okResult); 
}

// ---
unsigned char COMMODORE::Disk1540SeriesSimulation::receiveByte (MCHEmul::CPU* cpu, MCHEmul::UByte& b)
{
	unsigned char result = definition ()._okResult;

	// What to do will depend on the command being executed in the Disk...
	// and the satus of what the firmware is doing...
	if (_firmwareStatus == DiskFirmwareStatus::_ANSWERINGCOMMAND)
	{
		if (_currentCommand == "$")
		{
			if (_blockToAnswer.empty ())
			{
				_blockToAnswer = 
					std::move (buildAnswerToDirCommand ());

				_byteFromBlockToAnswerToSend = 0;
			}
		}
		else
		{
			if (_blockToAnswer.empty ())
			{
				_blockToAnswer =
					std::move (buildAnswerToFileCommand ());

				_byteFromBlockToAnswerToSend = 0;
			}
		}

		// Sets the byte received...
		b = _blockToAnswer [_byteFromBlockToAnswerToSend++];
		// ...and If it is at the end the result will be different...
		if (_byteFromBlockToAnswerToSend == _blockToAnswer.size ()) 
			result = 0x40;
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
std::vector <MCHEmul::UByte> COMMODORE::Disk1540SeriesSimulation::buildAnswerToDirCommand () const
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
	MCHEmul::Address iA ({ 0x01, 0x08 }, false);
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
			// Now it is time to iterate over the 8 entries in every sector...
			// until the information of all the entries is loaded, or until an empty entry is found,
			// Every entry is 32 bytes long...
			for (size_t i = 0; i < 8; i++)
			{
				if (dt [st].byte ((i << 5) + 0x03).value () == 0x00 &&
					dt [st].byte ((i << 5) + 0x04).value () == 0x00)
					continue; // The entry is not pointed to any place...

				size_t nfbt = 
					 (size_t) dt [st].byte ((i << 5) + 0x1e).value () +
					((size_t) dt [st].byte ((i << 5) + 0x1f).value () << 8);
				std::string nfbtstr = MCHEmul::trim (std::to_string (nfbt)); // Just to calculae the length...

				// Reads where the name of the entry is...
				// ...but it can be empty what means that there is no entry at all...
				// ...and probably the sector reading process should finish (see below)...
				std::vector <MCHEmul::UByte> n (4 - nfbtstr.length (), _SPACE_PETSCII); 
				n.push_back (_QUOTE_PETSCII); // Everything starts with a comma...
				unsigned char nc = 0xa0;
				size_t j = 0x05;
				for (;j < 0x15 && (nc = dt [st].byte ((i << 5) + j).value ()) != 0xa0; j++) 
						n.push_back ((nc == 0xa0) ? _SPACE_PETSCII : MCHEmul::UByte (nc));
				n.push_back (_QUOTE_PETSCII);

				// If there is no name...
				// continue to the next element...
				// Probably it means that the end of the entries is reached...
				if (j == 0x05)
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

				// Two additional spaces at the end...
				// of with a special symbol...
				if ((tp & (1 << 6)) != 0) n.insert (n.end (), _LESSER_PETSCII);
				else if ((tp & (1 < 7)) != 0) n.insert (n.end (), _ASTERISK_PETSCII);
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
std::vector <MCHEmul::UByte> COMMODORE::Disk1540SeriesSimulation::buildAnswerToFileCommand () const
{
	size_t t = 0, s = 0;
	std::tie (t, s) = getInitialTrackAndSectorOfFile (_currentCommand); // The name of the file with or without woldcards...
	if (t == 0 && s == 0) 
		return (std::vector <MCHEmul::UByte> { }); // No file at all...
	return (getDataOfFileFromTrackAndSector (t, s));
}

// ---
std::tuple <size_t, size_t> 
COMMODORE::Disk1540SeriesSimulation::getInitialTrackAndSectorOfFile (const std::string& fN) const
{
	// The willcards are in fN...
	auto matchWithWildcards = [&](const std::string& fE) -> bool
		{
			bool match = true;
			for (size_t i = 0; i < fN.length () && i < fE.length () && match; i++)
			{
				if (fN [i] == '?')
					continue;			// Still match...
				else
				if (fN [i] == '*')
					i = fN.length ();	// It is not necessary to check the rest...everything matches...
				else
				if (fN [i] != fE [i])
					match = false;		// One char that doesn't match, no worth to continue...
			}

			return (match);
		};

	size_t t = 0, s = 0;

	// All file info is n the track 18, until found...
	bool f = false;
	const auto& dE = _d64FileData -> directoryEntries ();
	for (size_t i = 0; i < dE.size () && !f; i++)
	{
		for (size_t j = 0; j < 8 && !f; j++)
		{
			if (f = (fN == "*" || 
				matchWithWildcards (dE [i][j]._fileName)))
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
	// If there were no more data in ithers tracks and sector...
	// ...the byte 0 of the data retrieved would be 0, and the byte 1 will ponted
	// the size in daa of this last block...
	if (dt [0] == 0)
		result.insert (result.end (), 
			dt.begin () + 2, dt.begin () + 2 + (size_t) dt [1].value ());
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
