/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: CommandBuilder.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 17/09/2022 \n
 *	Description: To build the command to execute instruction over a computer.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_COMMANDBUILDER__
#define __MCHEMUL_COMMANDBUILDER__

#include <CORE/Command.hpp>

namespace MCHEmul
{
	/** To create commands from an string that represents it. 
		The command builder keeps a list with the different types of commands created.
		The commands are reused changing the parameters if any. */
	class CommandBuilder
	{
		public:
		CommandBuilder ()
			: _commands ()
							{ }

		virtual ~CommandBuilder ()
							{ for (const auto& i : _commands) delete (i.second); }

		Command* command (const std::string& cmd) const;

		protected:
		virtual Command* createEmptyCommand (const std::string& cmdName) const = 0;

		// Implementation
		/** Used from "command" method. \n
			To extract the name of the command from the cmd parameter of that method. */
		std::string readCommandName (const std::string& cmd) const;
		/** Used from "command" method too. \n
			Important to know: \n
			The parameters can be provided either in the way NAME=VALUE or simply VALUE (will depend on the use). \n
			In the second form the parameter internally created will be COUNTER=VALUE, 
			where counter is the position (as string with the form XX) in the list of parameters received. \n
			So, no more than 100 parameters are allowed. */
		Attributes readCommandParameters (const std::string& cmd) const;

		mutable std::map <std::string, Command*> _commands;
	};
}

#endif
  
// End of the file
/*@}*/
