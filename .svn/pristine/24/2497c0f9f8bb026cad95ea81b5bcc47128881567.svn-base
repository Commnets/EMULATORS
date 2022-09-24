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
	  _computer (nullptr)
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
Emuls::Emulator::~Emulator ()
{ 
	delete (_computer); 
	
	delete (_communicationSystem);
}

// ---
bool Emuls::Emulator::run ()
{
	if (!computer () -> initialize () ||
		!communicationSystem () -> initialize ())
		return (false);

	if (byteFileName () != "")
		computer () -> load (byteFileName ());

	if (asmFileName () != "")
	{
		MCHEmul::Assembler::Parser parser (computer () -> cpu ());
		MCHEmul::Assembler::Compiler compiler (parser);
		MCHEmul::Assembler::ByteCode cL = compiler.compile (asmFileName ());
		MCHEmul::Address iA; 
		std::vector <MCHEmul::UByte> bt = cL.asSetOfBytes (iA);
		computer () -> memory () -> set (iA, bt);

		// Parser and compiler are destroyed here...
	}

	bool ok = true;
	unsigned int lL = logLevel ();
	while (ok && !computer () -> exit ())
	{
		ok &= communicationSystem () -> processMessagesOn (computer ());
		ok &= computer () -> runComputerCycle (lL);
		ok &= computer () -> runIOCycle (lL);
	}

	return (computer () -> lastError () != MCHEmul::_NOERROR);
}
