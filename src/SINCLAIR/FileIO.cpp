#include <SINCLAIR/FileIO.hpp>

// ---
MCHEmul::Strings SINCLAIR::KeystrokeTypeIO::generateTokensFor (const std::string& str) const
{
	struct TokenPosition 
		{ size_t _position; std::string _token; };
	using TokenPositions = std::vector <TokenPosition>;

	// Converts the line into uppercase to find better the token if any...
	std::string uStr = MCHEmul::trim (str);

	// The tokens...
	MCHEmul::Strings result; 
	size_t lTP = 0;
	std::string c;
	for (size_t i = 0; i < uStr.size(); i++) 
	{
		// Over the current piece under construction...
		// ...collects a new character...
		char l = uStr [i];
		c += l;
		std::string uC = MCHEmul::upper (c);

		// ...and verifies whether a new token (of several) has been reached!
		// Iterating over all and checking them!
		bool tPF = false;
		TokenPosition tP { };
		for (MCHEmul::Strings::const_iterator j = _TOKENS.begin (); 
			j != _TOKENS.end (); j++) // We have to iterate over all...
		{
			// Position where the token has been found...
			size_t tPP = 0;
			// If a reference to a token has been found...(using uppercase)
			if ((tPP = uC.find ((*j))) != std::string::npos)
			{
				// Sometimes the token is very simple (one character)
				// and could be confused with other made up with the next char...
				// So it is needed to ensure that can be inserted... 
				// These types of tokens depends on the computer. Are not the same in a ZXSpectrum than in a ZX80 or ZX81 even...
				bool ins = ((*j).length () == 1)
					? isARealToken (l /* Last letter used. */, uStr /** The line. */, i /** The position. */)
					: true;

				// The insertion is only possible...
				// ...when requested but there is nothing before or something with less length...
				// If there were two tokens possible with the same length, only the first will be taken into account...
				if (ins && 
					(!tPF || (tPF && tP._token.length () < (*j).length ())))
				{
					tPF = true;

					tP = TokenPosition { lTP + tPP, (*j) };
				}
			}
		}

		// If a token were found...
		if (tPF)
		{
			// The previous one could be a token...
			if (lTP < tP._position)
				result.emplace_back (MCHEmul::trim (uStr.substr (lTP, tP._position - lTP)));
			// ...and for sure the token found...
			result.emplace_back (MCHEmul::trim 
				(MCHEmul::upper (uStr.substr (tP._position, tP._token.length ())))); // The insertion in uppercase...
			// The new position...
			lTP = tP._position + tP._token.length (); 
			// Starts again...
			c = "";

			// Everything without spaces...
		}
	}

	// The last part not treated has also to be addedm if any...
	if (c != "")
	{
		std::string tC = MCHEmul::trim (c);
		std::string uTC = MCHEmul::upper (tC);
		result.emplace_back (((std::find (_TOKENS.begin (), _TOKENS.end (), uTC)) == _TOKENS.end ()) 
			? tC : uTC); // It is not the same whether it a token or it is not...
	}

	// ...and always the end of line is added...
	result.push_back ("\n");
	
	return (result);
}

// ---
MCHEmul::Strings SINCLAIR::KeystrokeTypeIO::generateKeystrokeForToken (const std::string& t) const
{
	// If the keystroke is not found, an enrror is kept into the log and nothing is returned...
	MCHEmul::Strings result;
	std::map <std::string, MCHEmul::Strings>::const_iterator i = _KEYSTROKES.find (t);
	if (i == _KEYSTROKES.end ())
	{
		// There is no a specific keystroke associated, so the keystrokes are just the keys...
		// The letter could be in uppercase...
		// ...and a SHIFT could be needed before...
		for (const auto& i : t)
			result.push_back ((i == ' ') 
				// A space is not considering as a token (so it is not in the transaltion list), 
				// but requeries special key for typewriter...
				? "SPACE" 
				: (std::isupper (i) ? "LSHIFT+" : "") + std::string (1, i));
	}
	else
		result = (*i).second;

	return (result);
}
