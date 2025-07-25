#include <EMULATORS/Emulator.hpp>
#include <SDL.h>

// ---
const unsigned char MCHEmul::Emulator::_PARAMHELP = 'h';
const unsigned char MCHEmul::Emulator::_PARAMBYTEFILE = 'f';
const unsigned char MCHEmul::Emulator::_PARAMBLOCKFILE = 'k';
const unsigned char MCHEmul::Emulator::_PARAMASMFILE = 'c';
const unsigned char MCHEmul::Emulator::_PARAMLOGLEVEL = 'l';
const unsigned char MCHEmul::Emulator::_PARAMADDRESS = 'a';
const unsigned char MCHEmul::Emulator::_PARAMADDRESSSTOP = 'd';
const unsigned char MCHEmul::Emulator::_PARAMSTOP = 's';
const unsigned char MCHEmul::Emulator::_PARAMLANGUAGE = 'i';
const unsigned char MCHEmul::Emulator::_PARAMSOUND = 'r';
const unsigned char MCHEmul::Emulator::_PARAMPERIPHERALS = 'p';
const unsigned char MCHEmul::Emulator::_PARAMCRTEFFECT = 'g';

// ---
MCHEmul::Emulator::Emulator (const MCHEmul::CommandLineArguments& args, MCHEmul::CommunicationSystem* cS)
	: _cmdlineArguments (args),
	  _communicationSystem (cS),
	  _debugLevel (MCHEmul::_DEBUGNOTHING),
	  _parser (nullptr), _compiler (nullptr),
	  _computer (nullptr),
	  _peripheralBuilder (nullptr),
	  _fileReader (nullptr),
	  _running (false),
	  _error (MCHEmul::_NOERROR)
{
	// The graphical and IO system used is based on SDL...
	int sdlE = 0;
	sdlE += SDL_Init (SDL_INIT_VIDEO);
	sdlE += SDL_InitSubSystem (SDL_INIT_JOYSTICK);
	if (sdlE != 0)
		_error = MCHEmul::_INIT_ERROR;
	else
		SDL_JoystickEventState (SDL_ENABLE);
}

// ---
MCHEmul::Emulator::~Emulator ()
{ 
	delete (_compiler);

	delete (_parser);

	delete (_communicationSystem);

	delete (_computer); 

	delete (_peripheralBuilder);

	delete (_fileReader);

	SDL_Quit ();
}

// ---
void MCHEmul::Emulator::printOutParameters (std::ostream& o) const
{
	o << "Copyright (C) 2022 by Ignacio Cea Fornies" << std::endl << std::endl;

	o << "/h:\t\t" << "Print out this help" << std::endl;
	o << "/f[FILENAME]:\t" << "Load a binary file" << std::endl;
	o << "/c[FILENAME]:\t" << "Load and compile a file" << std::endl;
	o << "/k[FILENAME]:\t" << "Load a block memory file" << std::endl;
	o << "/a[ADDRESS]:\t" << "The initial address of the PC register" << std::endl;
	o << "\t\tOtherwise the one defined by the computer emulated will be used" << std::endl;
	o << "/l[LEVEL]:\t" << "The deepness of the debug. 1 - 4. The bigger the deeper" << std::endl;
	o << "/d[BREAKS]:\t" << "Break points addresses separated by comma" << std::endl;
	o << "/i[LANGID]:\t" << "Language of the machine emulated. It has to be recognized from the emulated computer" << std::endl;
	o << "/p[PER1,...]:\t" << "List of the peripherals id separated per comma initialy connected" << std::endl;
	o << "/r[YES/NO]:\t" << "To activate or desactivate the sound when started. ON if no parameter" << std::endl; 
	o << "\t\tBy default it is activated" << std::endl;
	o << "/s:\t\t" << "Start the emulation stopped" << std::endl;
	o << "\t\tIf active, anytime the emulation starts it will do it stopped" << std::endl;
	o << "/g[YES/NO]:\t" << "To activate or desactivate the CRT effect in the screen. OFF if no parameter" << std::endl;
}

// ---
MCHEmul::Addresses MCHEmul::Emulator::stopAddresses () const
{
	MCHEmul::Addresses result;

	MCHEmul::Attributes::const_iterator i;
	MCHEmul::Strings strs = 
		_cmdlineArguments.existsArgument (_PARAMADDRESSSTOP)
			? MCHEmul::getElementsFrom (_cmdlineArguments.argumentAsString (_PARAMADDRESSSTOP), ',') 
			: MCHEmul::Strings ();
	for (const auto& i : strs)
		result.emplace_back (MCHEmul::Address::fromStr (i));

	return (result);
}

// ---
std::vector <int> MCHEmul::Emulator::peripheralsConnected () const
{
	std::vector <int> result = { };

	MCHEmul::Attributes::const_iterator i;
	MCHEmul::Strings strs = 
		_cmdlineArguments.existsArgument (_PARAMPERIPHERALS)
			? MCHEmul::getElementsFrom (_cmdlineArguments.argumentAsString (_PARAMPERIPHERALS), ',') 
			: MCHEmul::Strings ();
	for (const auto& i : strs)
		result.emplace_back (std::atoi (i.c_str ()));

	return (result);
}

// ---
bool MCHEmul::Emulator::connectPeripheral (int id, const MCHEmul::Attributes& prms, MCHEmul::IODevice* d)
{ 
	assert (d != nullptr); 

	return (computer () -> connectPeripheral (peripherialBuilder () -> peripheral (id, computer (), prms), d)); 
}

// ---
bool MCHEmul::Emulator::connectPeripheral (int id, const MCHEmul::Attributes& prms)
{
	MCHEmul::IOPeripheral* ph = peripherialBuilder () -> peripheral (id, computer (), prms);
	if (ph == nullptr)
		return (false);

	// Los for the first one where can match...
	bool result = false;
	for (MCHEmul::IODevices::const_iterator i = computer () -> devices ().begin ();
			i != computer () -> devices ().end () && !result; i++)
		result = (*i).second -> connectPeripheral (ph);

	return (result);
}

// ---
bool MCHEmul::Emulator::connectPeripherals 
	(std::vector <int> ids, const std::vector <MCHEmul::Attributes>& prms)
{
	std::vector <MCHEmul::Attributes> prmsC = prms;
	if (ids.size () != prms.size ())
		prmsC.resize (ids.size ()); // Using the default Attributes constructor that is n empty list...

	int ct = 0;
	bool result = true;
	for (int i : ids)
		result &= connectPeripheral (i, prmsC [ct++]);
	return (result); // If just 1 fails, all fails...
}

// ---
MCHEmul::FileData* MCHEmul::Emulator::connectDataToPeripheral (const std::string& fN, int id)
{ 
	MCHEmul::IOPeripheral* p = _computer -> peripheral (id);
	if (p == nullptr)
		return (nullptr);

	MCHEmul::FileData* result = 
		fileIO () -> readFile (fN, computer () -> cpu () -> architecture ().bigEndian ());
	if (result != nullptr)
	{
		if (!p -> connectData (result))
			return (nullptr); // There might have been created the data,
							  // but a nullptr is returns as nothing can be reused...
	}

	return (result);
}

// ---
MCHEmul::FileData* MCHEmul::Emulator::createEmptyDataInPeripheral (const std::string& fN, int id)
{
	MCHEmul::IOPeripheral* p = _computer -> peripheral (id);
	if (p == nullptr)
		return (nullptr);

	bool ok = true;
	MCHEmul::FileData* result = p -> emptyData ();
	if (result != nullptr)
		ok = fileIO () -> 
				saveFile (result, fN, computer () -> cpu () -> architecture ().bigEndian ());
	// The data created is no longer needed...
	delete (result);
	// Connect the data to the peripheral...if any!
	return (ok ? connectDataToPeripheral (fN, id) : nullptr);
}

// ---
bool MCHEmul::Emulator::saveDataFromPeripheral (const std::string & fN, int id)
{
	MCHEmul::IOPeripheral* p = _computer -> peripheral (id);
	if (p == nullptr)
		return (false);

	bool result = false;
	MCHEmul::FileData* data = p -> retrieveData ();
	if (data != nullptr)
		result = fileIO () -> 
			saveFile (data, fN, computer () -> cpu () -> architecture ().bigEndian ());

	// The data has to be deleted...
	delete (data);

	return (result);
}

// ---
MCHEmul::DataMemoryBlock MCHEmul::Emulator::loadBinaryFile (const std::string& fN, bool& e)
{
	MCHEmul::DataMemoryBlock result = 
		MCHEmul::DataMemoryBlock::loadBinaryFile (fN, e, 
			_computer -> cpu () -> architecture ().numberBytes (), 
			_computer -> cpu () -> architecture ().bigEndian ());

	if (!e)
		_computer -> memory () -> put ({ result });

	return (result);
}

// ---
MCHEmul::Assembler::ByteCode MCHEmul::Emulator::loadProgram (const std::string& fN, MCHEmul::Assembler::Errors& e)
{
	MCHEmul::Assembler::Compiler* c = compiler ();
	MCHEmul::Assembler::ByteCode cL = c -> compile (fN);

	if (!*c)
		e = c -> errors ();
	else
	{
		e = { };

		_computer -> memory () -> put (cL.asDataMemoryBlocks ());
		_computer -> addActions (cL.listOfActions ());
	}

	return (cL);
}

// ---
MCHEmul::DataMemoryBlocks MCHEmul::Emulator::loadBlocksFile (const std::string& fN, bool& e)
{
	MCHEmul::DataMemoryBlocks result = 
		MCHEmul::DataMemoryBlock::loadBlocksFile (fN, e, 
			_computer -> cpu () -> architecture ().numberBytes (), 
			_computer -> cpu () -> architecture ().bigEndian ());

	if (!e)
		_computer -> memory () -> put (result);

	return (result);
}

// ---
bool MCHEmul::Emulator::initialize ()
{
	// No initialization possible with errors...
	if (_error != MCHEmul::_NOERROR)
	{
		_LOG ("The emulator was not well created");

		return (false);
	}

	// Only once!
	// If the _computer variable already exists, 
	// it would mean that the initialization was already executed
	if (_computer != nullptr)
	{
		_LOG ("The emulator was was already initialized");

		return (false);
	}

	// The most important line: The computer is created!...
	computer ();

	// Before initializing it is needed to define whether...
	// ...the computer has or not to start stopped
	if (stoppedAtStarting ())
		_computer -> setNextStartStopped (true);

	/** If help is needed then no other action is executed. */
	if (helpNeeded ())
	{ 
		printOutParameters (std::cout);

		return (false);
	}

	setDebugLevel (logLevel ());

	// First of all, the computer mut be initialized...
	if (!_computer -> initialize ()) 
	{
		if (_debugLevel >= MCHEmul::_DEBUGERRORS)
			_LOG ("Error initializing computer");

		return (false);
	}

	// Initialize the communication system if it has been defined...
	if (_communicationSystem != nullptr && !_communicationSystem -> initialize ())
	{
		if (_debugLevel >= MCHEmul::_DEBUGERRORS)
			_LOG ("Error initializing communications");

		return (false);
	}

	if (_debugLevel >= MCHEmul::_DEBUGTRACEINTERNALS)
		_LOG (MCHEmul::FormatterBuilder::instance () -> 
			formatter ("Computer") -> format (computer () -> getInfoStructure ()));

	// Connect the peripherals defined...
	std::vector <int> phs; 
	if (!(phs = peripheralsConnected ()).empty ())
	{
		bool e = true;
		for (int i : phs)
			e &= connectPeripheral (i, { });
		if (!e && _debugLevel >= MCHEmul::_DEBUGERRORS)
			_LOG ("There are peripherals not connected. Verify");
	}

	// Load a byte file if defined...
	if (byteFileName () != "")
	{
		bool e = false;
		loadBinaryFile (byteFileName (), e);
		if (e)
		{
			if (_debugLevel >= MCHEmul::_DEBUGERRORS)
				_LOG ("Error loading file:" + byteFileName ());

			return (false);
		}
	}

	// Load an assembler file (and compile if) if defined...
	if (asmFileName () != "")
	{
		MCHEmul::Assembler::Errors e;
		MCHEmul::Assembler::ByteCode cL = loadProgram (asmFileName (), e);
		if (!e.empty ())
		{
			if (_debugLevel >= MCHEmul::_DEBUGERRORS)
				for (const auto& i : e)
					_LOG (i.asString ());

			return (false);
		}
		else
		{
			if (_debugLevel >= MCHEmul::_DEBUGALL)
				for (const auto& i : cL._lines)
					_LOG (MCHEmul::removeAll0 (i.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0')));
		}
	}

	// Load the block data file if defined...
	if (blockFileName () != "")
	{
		bool e = false;
		loadBlocksFile (blockFileName (), e);
		if (e)
		{
			if (_debugLevel >= MCHEmul::_DEBUGERRORS)
				_LOG ("Error loading file:" + blockFileName ());

			return (false);
		}
	}

	// The address where to start the executed if fixed if defined...
	if (startingAddress () != MCHEmul::Address ())
		_computer -> setStartingAddress (startingAddress ());

	// Same with the stop points...
	MCHEmul::Addresses adrs;
	if (!(adrs = stopAddresses ()).empty ())
	{
		MCHEmul::Computer::MapOfActions acts;
		for (const auto& i : adrs)
			acts.insert (MCHEmul::Computer::MapOfActions::value_type (i, MCHEmul::Computer::_ACTIONSTOP));
		_computer -> addActions (acts);
	}

	// There is another option to activate (by default) or desactivate the sound when starting...
	if (!soundAtStarting () && _computer -> sound () != nullptr)
		_computer -> sound () -> setSilence (true);

	// Last option is to activate or desactivate (by default) the sound when starting...
	if (CRTEffectAtStarting() && _computer -> screen () != nullptr)
		_computer -> screen () -> setCRTEffect (true);

	_computer -> startsSpeedClock ();

	return (true);
}

// ---
bool MCHEmul::Emulator::run ()
{
	do
	{
		// Is the emulation is comming from a restarting?
		if (computer () -> restartAfterExit ())
		{ 
			// Once is enough...
			computer () -> setRestartAfterExit (false, 1 /** Full, but without memory. */);
			// ...and whether running again will depens on the restart...
			computer () -> setExit (!computer () -> restart ());
		}

		_running = true;

		computer () -> startsSpeedClock ();

		while (runCycle (/** no action. */) && 
			   !computer () -> exit ());

		_running = false;
	} 
	while (computer () -> restartAfterExit ());

	if (_communicationSystem != nullptr)
		_communicationSystem -> finalize ();

	// When exiting a dump is done if either there was an error 
	// or the right debug level was selected...
	bool result = (_computer -> error () != MCHEmul::_NOERROR);
	if (_debugLevel >= MCHEmul::_DUMPATEXIT || !result)
		_LOG (MCHEmul::removeAll0 (MCHEmul::FormatterBuilder::instance () -> 
			formatter ("Computer") -> format (_computer -> getInfoStructure ())));
	return (result);
}

// ---
bool MCHEmul::Emulator::runCycle (unsigned int a)
{
	bool result = true;

	if (_communicationSystem != nullptr)
	{
		if (!_communicationSystem -> processMessagesOn (computer ()) &&
			_debugLevel >= MCHEmul::_DEBUGERRORS)
				_LOG ("Error Processing Messages");
	}

	result &= computer () -> runComputerCycle (a);
	result &= computer () -> runIOCycle ();
	result &= additionalRunCycle ();

	return (result);
}

// ---
MCHEmul::Assembler::Parser* MCHEmul::Emulator::createParser () const 
{ 
	Assembler::Parser* r = new MCHEmul::Assembler::Parser (_computer -> cpu ());
	
	r -> setPrintOutProcess (true); // To print out the parsing process...
	
	return (r); 
}
