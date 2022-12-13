#include <CORE/CmdlineArgs.hpp>

const std::string MCHEmul::CommandLineArguments::_NOARGUMENT = "-";

// ---
std::map <char, std::string> MCHEmul::CommandLineArguments::readArgumentsFrom (const MCHEmul::Strings& prms)
{
	std::map <char, std::string> result;
	for (const auto& i : prms)
	{
		std::map <unsigned char, std::string>::const_iterator p;
		if (i.length () < 2 || i [0] != '/') continue; // Not valid argument...
		else result [i [1]] = i.substr (2);
	}

	return (result);
}
