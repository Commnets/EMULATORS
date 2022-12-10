#include <EMULATORS/Emulator.hpp>
#include <SDL.h>

// ---
const unsigned char MCHEmul::Emulator::_PARAMBYTEFILE = 'f';
const std::string MCHEmul::Emulator::_BYTEFILE = "BYTEFILE";
const unsigned char MCHEmul::Emulator::_PARAMASMFILE = 'c';
const std::string MCHEmul::Emulator::_ASMFILE = "ASMFILE";
const unsigned char MCHEmul::Emulator::_PARAMLOGLEVEL = 'l';
const std::string MCHEmul::Emulator::_LOGLEVEL = "LOGLEVEL";
const unsigned char MCHEmul::Emulator::_PARAMADDRESS = 'a';
const std::string MCHEmul::Emulator::_ADDRESS = "ADDRESS";
const unsigned char MCHEmul::Emulator::_PARAMADDRESSSTOP = 'd';
const std::string MCHEmul::Emulator::_ADDRESSSTOP = "ADDRESSSTOP";
const unsigned char MCHEmul::Emulator::_PARAMSTOP = 's';
const std::string MCHEmul::Emulator::_STOP = "STOP";

// ---
MCHEmul::Emulator::Emulator (const MCHEmul::Strings& argv, MCHEmul::CommunicationSystem* cS)
	: _attributes (),
	  _communicationSystem (cS),
	  _debugLevel (MCHEmul::_DEBUGNOTHING),
	  _parser (nullptr), _compiler (nullptr),
	  _computer (nullptr),
	  _peripheralBuilder (nullptr),
	  _running (false),
	  _error (MCHEmul::_NOERROR)
{
	static std::map <unsigned char, std::string> _MATCH =
		{ { _PARAMBYTEFILE, _BYTEFILE },
		  { _PARAMASMFILE, _ASMFILE },
		  { _PARAMLOGLEVEL, _LOGLEVEL },
		  { _PARAMADDRESS, _ADDRESS },
		  { _PARAMADDRESSSTOP, _ADDRESSSTOP },
		  { _PARAMSTOP , _STOP }
		};

	for (unsigned int i = 1 /** param 0 = name of the executable */; i < argv.size (); i++)
	{
		std::map <unsigned char, std::string>::const_iterator p;
		if (argv [i].length () < 2 || argv [i][0] != '/' || 
			(p = _MATCH.find (argv [i][1])) == _MATCH.end ())
			continue; // Not valid argument...
		_attributes [(*p).second] = argv [i].substr (2);
	}

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
MCHEmul::Addresses MCHEmul::Emulator::stopAddresses () const
{
	MCHEmul::Addresses result;
	MCHEmul::Attributes::const_iterator i;
	MCHEmul::Strings strs = MCHEmul::getElementsFrom 
		(((i = _attributes.find (_ADDRESSSTOP)) != _attributes.end ()) ? (*i).second : "", ',');
	for (const auto& i : strs)
		result.push_back (MCHEmul::Address::fromStr (i));
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
bool MCHEmul::Emulator::initialize ()
{
	if (_error != MCHEmul::_NOERROR)
	{
		std::cout << "The emulator was not well created" << std::endl;

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

	if (byteFileName () != "" && asmFileName () == "")
	{
		bool r = computer () -> loadInto (byteFileName (), MCHEmul::Address ({ 0x00, 0x00 }));
		if (!r)
		{
			if (_debugLevel >= MCHEmul::_DEBUGERRORS)
				std::cout << "Error loading file: " << byteFileName () << std::endl;

			return (false);
		}
	}

	if (asmFileName () != "" && byteFileName () == "")
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

	return (true);
}

// ---
bool MCHEmul::Emulator::run ()
{
	_running = true;

	computer () -> startsComputerClock ();

	while (runCycle (/** no action. */) && 
		   !computer () -> exit ());

	if (_communicationSystem != nullptr)
		_communicationSystem -> finalize ();

	_running = false;

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

	computer () -> finishCycle ();

	return (result);
}
