/** \ingroup CORE */
/*@{*/

/**	
*	@file	
*	File: CmdlineArgs.hpp \n
*	Framework: CPU Emulators library \n
*	Author: Ignacio Cea Fornies (EMULATORS library) \n
*	Creation Date: 13/12/2022 \n
*	Description: To read and store the command line arguments.
*	Versions: 1.0 Initial
*/

#ifndef __MCHEMUL_CMDLINEARGS__
#define __MCHEMUL_CMDLINEARGS__

#include <CORE/global.hpp>
#include <CORE/UBytes.hpp>
#include <CORE/UInt.hpp>
#include <CORE/Address.hpp>

namespace MCHEmul
{
	/** The class receives at construction 
		time what is usual in the command line arguments and transform them 
		into a set of pairs of parameters and value. \n
		The commandline arguments should be made up of /XYYYYY, where X is the name of the argument
		and YYYY its value. The next one has to be separated by an space. */
	class CommandLineArguments final
	{
		public:
		CommandLineArguments () = delete;

		CommandLineArguments (int n, char** dt)
			: _arguments (readArgumentsFrom (convertIntoStrings (n, dt)))
							{ }

		CommandLineArguments (int n, wchar_t** dt)
			: _arguments (readArgumentsFrom (convertIntoStrings (n, dt)))
							{ }

		bool existsArgument (char arg) const
							{ return (_arguments.find (arg) != _arguments.end ()); }

		// To get the arguments in different formats, all of them very common...
		std::string argumentAsString (char arg) const
							{ std::map <char, std::string>::const_iterator i = _arguments.find (arg); 
							  return ((i == _arguments.end ()) ? _NOARGUMENT : (*i).second); }
		/** Everything not - will be understood as true. */
		bool argumentAsBool (char arg) const
							{ std::string val = upper (argumentAsString (arg)); 
							  return (val == _NOARGUMENT ? false : (val == "YES" || val == "")); }
		/** A "no number" will return 0. */
		unsigned int argumentAsInt (char arg) const
							{ return (std::atoi (argumentAsString (arg).c_str ())); }
		UInt argumentAsUInt (char arg) const
							{ return (UInt::fromStr (argumentAsString (arg))); }
		UBytes argumentAsUBytes (char arg) const
							{ return (argumentAsUInt (arg).values ()); }
		Address argumentAsAddress (char arg) const
							{ return (Address::fromStr (argumentAsString (arg))); }

		private:
		/** Internal method used at construction. */
		std::map <char, std::string> readArgumentsFrom (const Strings& prms);

		private:
		std::map <char, std::string> _arguments;

		/** The no argument value is returned when a not valid argument is requested. */
		static const std::string _NOARGUMENT;
	};
}

#endif

// End of the file
/*@}*/
