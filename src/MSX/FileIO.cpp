#include <MSX/FileIO.hpp>

// ---
MCHEmul::Strings MSX::KeystrokeTypeIO::generateKeystrokeForToken (const std::string& t) const
{
	// If the token received is just "complex"
	// because the combination of the keys was done...
	if (t.length () > 1)
		return (MCHEmul::Strings { t }); // The keystroke is already the token!

	MCHEmul::Strings result;

	// In the MSX system there is upper case letter and normal case ones...
	// so, if a upper case one is detected the LSHIFT keystroke must be selected!
	if (std::isalpha (t [0]) && std::isupper (t [0]))
		result.emplace_back ("LSHIFT+" + std::string ({ t [0] }));
	else
	{
		std::map <char, MCHEmul::Strings>::const_iterator sp;
		if ((sp = _SPECIALKEYS.find (t [0])) != _SPECIALKEYS.end ()) result = (*sp).second;
		else result = MCHEmul::Strings { t };
	}

	return (result);
}
