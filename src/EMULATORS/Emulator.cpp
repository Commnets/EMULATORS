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

// ---
MCHEmul::Emulator::Emulator (const MCHEmul::CommandLineArguments& args, MCHEmul::CommunicationSystem* cS)
	: _cmdlineArguments (args),
	  _communicationSystem (cS),
	  _debugLevel (MCHEmul::_DEBUGNOTHING),
	  _parser (nullptr), _compiler (nullptr),
	  _computer (nullptr),
	  _peripheralBuilder (nullptr),
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

	SDL_Quit ();
}

// ---
void MCHEmul::Emulator::printOutParameters (std::ostream& o) const
{
	o << "Copyright (C) 2022 by Ignacio Cea Fornies" << std::endl << std::endl;

	o << "/h:\t\t" << "Print out this help." << std::endl;
	o << "/f[FILENAME]:\t" << "Load a binary file." << std::endl;
	o << "/c[FILENAME]:\t" << "Load and compile a file." << std::endl;
	o << "/k[FILENAME]:\t" << "Load a block memory file." << std::endl;
	o << "/a[ADDRESS]:\t" << "The initial address of the PC register." << std::endl;
	o << "\t\tOtherwise the one defined by the computer emulated will be used." << std::endl;
	o << "/l[LEVEL]:\t" << "The deepness of the debug. 1 - 4. The bigger the deeper." << std::endl;
	o << "/d[BREAKS]:\t" << "Break points address separated by comma." << std::endl;
	o << "/s:\t\t" << "Start the emulation stopped." << std::endl;
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
MCHEmul::DataMemoryBlock MCHEmul::Emulator::loadBinaryFile (const std::string& fN, bool& e)
{
	MCHEmul::DataMemoryBlock result = 
		MCHEmul::DataMemoryBlock::loadBinaryFile (fN, e, 
			_computer -> cpu () -> architecture ().numberBytes (), 
			_computer -> cpu () -> architecture ().bigEndian ());

	if (!e)
		_computer -> memory () -> set ({ result });

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

		_computer -> memory () -> set (cL.asDataMemoryBlocks ());
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
		_computer -> memory () -> set (result);

	return (result);
}

// ---
bool MCHEmul::Emulator::initialize ()
{
	if (_error != MCHEmul::_NOERROR)
	{
		std::cout << "The emulator was not well created" << std::endl;

		return (false);
	}

	/** If help is needed then no other action is executed. */
	if (helpNeeded ())
	{ 
		printOutParameters (std::cout);

		return (false);
	}

	setDebugLevel (logLevel ());

	if (!_computer -> initialize ()) 
	{
		if (_debugLevel >= MCHEmul::_DEBUGERRORS)
			std::cout << "Error initializing computer" << std::endl;

		return (false);
	}

	if (_communicationSystem != nullptr && !_communicationSystem -> initialize ())
	{
		if (_debugLevel >= MCHEmul::_DEBUGERRORS)
			std::cout << "Error initializing communications" << std::endl;

		return (false);
	}

	if (_debugLevel >= MCHEmul::_DEBUGTRACEINTERNALS)
		std::cout << *computer () << std::endl;

	bool lF = false;

	if (byteFileName () != "" && asmFileName () == "" && blockFileName () == "")
	{
		bool e = false;
		loadBinaryFile (byteFileName (), e);
		if (e)
		{
			if (_debugLevel >= MCHEmul::_DEBUGERRORS)
				std::cout << "Error loading file: " << byteFileName () << std::endl;

			return (false);
		}

		lF = true;
	}

	if (asmFileName () != "" && byteFileName () == "" && blockFileName () == "")
	{
		MCHEmul::Assembler::Errors e;
		MCHEmul::Assembler::ByteCode cL = loadProgram (asmFileName (), e);
		if (!e.empty ())
		{
			if (_debugLevel >= MCHEmul::_DEBUGERRORS)
				for (const auto& i : e)
					std::cout << i << std::endl;

			return (false);
		}
		else
		{
			if (_debugLevel >= MCHEmul::_DEBUGALL)
				for (const auto& i : cL._lines)
					std::cout << i << std::endl;
		}

		lF = true;
	}

	if (blockFileName () != "" && byteFileName () == "" && asmFileName () == "")
	{
		bool e = false;
		loadBlocksFile (blockFileName (), e);
		if (e)
		{
			if (_debugLevel >= MCHEmul::_DEBUGERRORS)
				std::cout << "Error loading file: " << blockFileName () << std::endl;

			return (false);
		}

		lF = true;
	}

	if (!lF && (byteFileName () != "" || blockFileName () != "" || asmFileName () != ""))
	{
		if (_debugLevel >= MCHEmul::_DEBUGERRORS)
			std::cout << "Only one type of file can be loaded at the same time" << std::endl
					  << "No file has been loaded" << std::endl;
	}

	if (startingAddress () != MCHEmul::Address ())
		_computer -> cpu () -> programCounter ().setAddress (startingAddress ());

	MCHEmul::Addresses adrs;
	if (!(adrs = stopAddresses ()).empty ())
	{
		MCHEmul::Computer::MapOfActions acts;
		for (const auto& i : adrs)
			acts.insert (MCHEmul::Computer::MapOfActions::value_type (i, MCHEmul::Computer::_ACTIONSTOP));
		_computer -> addActions (acts);
	}

	if (stoppedAtStarting ())
		_computer -> setActionForNextCycle (MCHEmul::Computer::_ACTIONSTOP);

	computer () -> startsComputerClock ();

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
			computer () -> setRestartAfterExit (false);
			// ...and whether running again will depens on the restart...
			computer () -> setExit (!computer () -> restart ());
		}

		_running = true;

		computer () -> startsComputerClock ();

		while (runCycle (/** no action. */) && 
			   !computer () -> exit ());

		_running = false;
	} 
	while (computer () -> restartAfterExit ());

	if (_communicationSystem != nullptr)
		_communicationSystem -> finalize ();

	return (computer () -> error () != MCHEmul::_NOERROR);
}

// ---
bool MCHEmul::Emulator::runCycle (unsigned int a)
{
	bool result = true;

	if (_communicationSystem != nullptr)
	{
		if (!_communicationSystem -> processMessagesOn (computer ()) &&
			_debugLevel >= MCHEmul::_DEBUGERRORS)
				std::cout << "Error Processing Messages" << std::endl;
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
