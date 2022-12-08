#include <CORE/CommandBuilder.hpp>

// ---
MCHEmul::Command* MCHEmul::CommandBuilder::command (const std::string& cmd) const
{
	MCHEmul::Command* result = nullptr;

	std::string CMD = MCHEmul::trim (cmd);
	std::string CName = readCommandName (CMD);
	std::map <std::string, MCHEmul::Command*>::const_iterator i;
	if ((i = _commands.find (CName)) != _commands.end ())
		(result = (*i).second) -> setParameters (readCommandParameters (CMD));
	else
	{
		if ((result = createEmptyCommand (CName)) != nullptr)
		{
			result -> setParameters (readCommandParameters (CMD)); // If there is instruction set the prms!

			_commands.insert (std::pair <std::string, MCHEmul::Command*> (CName, result)); // ...and keep it for later!
		}
	}

	return (result);
}

// ---
std::string MCHEmul::CommandBuilder::readCommandName (const std::string& cmd) const
{
	std::string result = cmd;

	std::size_t p;
	if ((p = cmd.find_first_of (' ')) != std::string::npos)
		result = result.substr (0, p);

	return (result);
}

// ---
MCHEmul::Attributes MCHEmul::CommandBuilder::readCommandParameters (const std::string& cmd) const
{
	MCHEmul::Attributes result;

#define ENDLOOP { prms = ""; break; }

	std::string prms = cmd;
	size_t iP = prms.find_first_of (' '); // Take off the first word!
	if (iP == std::string::npos || iP >= (cmd.length () - 1)) // Are there other things after?...they could be parameters
		return (result); // There is nothing...so no parameters!
	else
		if ((prms = MCHEmul::trim (prms.substr (iP))) == "")
			return (result); // But what is after are only spaces...

	// Now it is time to investigate what is left!
	unsigned int ctP = 0;
	while (prms != "")
	{
		// The equal symbol defines (almost always) a parameter...
		iP = prms.find_first_of ('=');
		// but when there is none, it could be considerered as a simple parameter...
		if (iP == std::string::npos)
		{
			// The name of the parameter will be a consecutive number ocuppying two positions!
			std::string fName = std::to_string (ctP++); 
			fName = MCHEmul::_CEROS.substr (0, 2 - fName.length ()) + fName;
			
			// The value of that parameter lasts until the following space!
			iP = prms.find_first_of (' '); 
			std::string fValue = (iP == std::string::npos) ? prms : MCHEmul::upper (prms.substr (0, iP));
			prms = (iP == std::string::npos) ? "" : MCHEmul::trim (prms.substr (iP + 1));
			
			result.insert (MCHEmul::Attributes::value_type (fName, fValue));

			continue; // next...
		}

		// There is and equal symbol, but there is nothing else after...
		// so there is an error, and the anaylisis has to end!
		if (iP == (prms.length () - 1))
			ENDLOOP;

		// There is an equal symbol, and there is something after...
		// What is before that is the name of the field...
		std::string fName = MCHEmul::upper (MCHEmul::trim (prms.substr (0, iP)));
		// What is after the equal symbol contains (potentially) the value of that field...
		prms = MCHEmul::trim (prms.substr (iP + 1));
		// If that starts with quotes, the content has to be taken as it is up to the following quote!
		if (prms [0] == '"')
		{
			iP = prms.find_first_of ('"');
			if (iP == std::string::npos) // There must be a quote at the end, otherwise it will be an error!
				ENDLOOP;

			result.insert (MCHEmul::Attributes::value_type (fName, prms.substr (1, iP - 1)));
			prms = prms.substr (iP + 1);
		}
		else
		{
			iP = prms.find_first_of (' ');
			result.insert (MCHEmul::Attributes::value_type (fName, (iP == std::string::npos) ? prms : prms.substr (0, iP)));
			prms = (iP == std::string::npos) ? "" : prms.substr (iP + 1);
		}
	}

	return (result);
}
