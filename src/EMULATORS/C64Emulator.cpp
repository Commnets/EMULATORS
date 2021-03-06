#include <EMULATORS/C64Emulator.hpp>
#include <C64/C64.hpp>

const unsigned char Emuls::C64Emulator::_PARAMNTSC = 'n';
const std::string Emuls::C64Emulator::_NTSC = "NTSC";

// ---
Emuls::C64Emulator::C64Emulator (const std::vector <std::string>& argv)
	: Emuls::Emulator (argv)
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
