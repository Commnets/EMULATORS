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
	_status = COMMODORE::SerialIOPeripheralSimulation::Status::_NOTLISTENING;
	_statusOpen = COMMODORE::SerialIOPeripheralSimulation::StatusOpen::_CLOSE;

	_lastCPUCycles = 0;
	_commandChannel = 0;
	_firmwareStatus = DiskFirmwareStatus::_IDLE;

	for (auto& channel : _channels)
		channel.reset (definition ()._okResult);
	// The command channel always exists even before an explicit OPEN...
	_channels [15]._open = true;

	setDOSStatus (73, "CBM DOS V2.6 1541");

	return (true);
}

// ---
bool COMMODORE::Disk1540SeriesSimulation::connectData (MCHEmul::FileData* dt)
{
	if ((_d64FileData = dynamic_cast <COMMODORE::D64FileData*> (dt)) == nullptr)
		return (false); // These formats are the only ones accepted...

	_data = dt -> asMemoryBlocks ();

	return (initialize ());
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
		channel._command.pop_back ();
	if (channel._command.empty ())
		return; // An empty command must preserve the previous DOS status...

	switch (channel._command [0])
	{
		case 'N':
		case 'C':
		case 'R':
		case 'S':
		case 'I':
		case 'V':
			{
				_LOG ("Disk1540SeriesSimulation: Command " +
					channel._command + " is not implemented yet.");

				setDOSStatus (3, "UNIMPLEMENTED");
			}

			break;

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
	static const unsigned char _SECONDARY = 0x06;
	static const unsigned char _OPEN = 0x0f;

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
	}
	else if (command == _SECONDARY)
	{
		if (_firmwareStatus == DiskFirmwareStatus::_RECEIVINGCOMMAND)
		{
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
						channel._blockToAnswer =
							std::move (buildAnswerToFileCommand
								(splitCommandIntoPreAndPostData
									(channel._command)));

						channel._byteFromBlockToAnswerToSend = 0;
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
