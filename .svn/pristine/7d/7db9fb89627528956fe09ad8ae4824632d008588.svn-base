#include <EMULATORS/Emulator.hpp>

const unsigned char Emuls::Emulator::_PARAMBYTEFILE = 'f';
const std::string Emuls::Emulator::_BYTEFILE = "BYTEFILE";
const unsigned char Emuls::Emulator::_PARAMASMFILE = 'c';
const std::string Emuls::Emulator::_ASMFILE = "ASMFILE";
const unsigned char Emuls::Emulator::_PARAMLOGLEVEL = 'l';
const std::string Emuls::Emulator::_LOGLEVEL = "LOGLEVEL";
const unsigned char Emuls::Emulator::_PARAMADDRESS = 'a';
const std::string Emuls::Emulator::_ADDRESS = "ADDRESS";

// ---
Emuls::Emulator::Emulator (const std::vector <std::string>& argv)
	: _attributes (),
	  _communicationSystem (nullptr),
	  _debugLevel (MCHEmul::_DEBUGNOTHING),
	  _computer (nullptr),
	  _running (false)
{
	static std::map <unsigned char, std::string> _MATCH =
		{ { _PARAMBYTEFILE, _BYTEFILE },
		  { _PARAMASMFILE, _ASMFILE },
		  { _PARAMLOGLEVEL, _LOGLEVEL },
		  { _PARAMADDRESS, _ADDRESS } };

	for (unsigned int i = 1 /** param 0 = name of the executable */; i < argv.size (); i++)
	{
		std::map <unsigned char, std::string>::const_iterator p;
		if (argv [i].length () < 2 || argv [i][0] != '/' || 
			(p = _MATCH.find (argv [i][1])) == _MATCH.end ())
			continue; // Not valid argument...
		_attributes [(*p).second] = argv [i].substr (2);
	}
}

// ---
void Emuls::Emulator::setCommunicationSystem (MCHEmul::CommunicationSystem* cS)
{
	if (_running)
		return;

	_communicationSystem = cS;
}

// ---
Emuls::Emulator::~Emulator ()
{ 
	delete (_communicationSystem);

	delete (_computer); 
}

// ---
bool Emuls::Emulator::initialize ()
{
	setDebugLevel (logLevel ());

	if (!computer () -> initialize ()) 
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
		bool r = computer () -> load (byteFileName ());
		if (!r)
		{
			if (_debugLevel >= MCHEmul::_DEBUGERRORS)
				std::cout << "Error loading file: " << byteFileName () << std::endl;

			return (false);
		}
	}

	if (asmFileName () != "" && byteFileName () == "")
	{
		MCHEmul::Assembler::Parser parser (computer () -> cpu ());
		MCHEmul::Assembler::Compiler compiler (parser);
		MCHEmul::Assembler::ByteCode cL = compiler.compile (asmFileName ());

		if (!compiler)
		{
			if (_debugLevel >= MCHEmul::_DEBUGERRORS)
				for (auto i : compiler.errors ())
					std::cout << i << std::endl;

			return (false);
		}
		else
		{
			if (_debugLevel >= MCHEmul::_DEBUGALL)
				for (auto i : cL._lines)
					std::cout << i << std::endl;
		}

		MCHEmul::Address iA; 
		std::vector <MCHEmul::UByte> bt = cL.asSetOfBytes (iA);
		computer () -> memory () -> set (iA, bt);

		// Parser and compiler are destroyed here...
	}

	if (startingAddress () != MCHEmul::Address ())
		computer () -> cpu () -> programCounter ().setAddress (startingAddress ());

	return (true);
}

// ---
bool Emuls::Emulator::run ()
{
	_running = true;

	while (runCycle () && !computer () -> exit ());

	if (_communicationSystem != nullptr)
		_communicationSystem -> finalize ();

	_running = false;

	return (computer () -> lastError () != MCHEmul::_NOERROR);
}

// ---
bool Emuls::Emulator::runCycle ()
{
	bool result = true;

	computer () -> startsCycle ();

	if (_communicationSystem != nullptr)
		result &= _communicationSystem -> processMessagesOn (computer ());
	result &= computer () -> runComputerCycle ();
	result &= computer () -> runIOCycle ();

	computer () -> finishCycle ();

	return (result);
}
