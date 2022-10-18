#include <C64/C64Emulator.hpp>
#include <C64/C64.hpp>

// ---
const unsigned char C64::C64Emulator::_PARAMNTSC = 'n';
const std::string C64::C64Emulator::_NTSC = "NTSC";

// ---
bool C64::C64Emulator::initialize ()
{
	if (!MCHEmul::Emulator::initialize ())
		return (false);

	// TODO
	// To link the peripherals...

	return (true);
}

// ---
C64::C64Emulator::C64Emulator (const MCHEmul::Strings& argv, MCHEmul::CommunicationSystem* cS)
	: MCHEmul::Emulator (argv, cS)
{
	static std::map <unsigned char, std::string> _MATCH =
		{ { _PARAMNTSC, _NTSC } };

	for (unsigned int i = 1 /** param 0 = name of the executable */; i < argv.size (); i++)
	{
		std::map <unsigned char, std::string>::const_iterator p;
		if (argv [i].length () < 2 || argv [i][0] != '/' || 
			(p = _MATCH.find (argv [i][1])) == _MATCH.end ())
			continue; // Not valid argument...
		_attributes [(*p).second] = argv [i].substr (2);
	}
}
