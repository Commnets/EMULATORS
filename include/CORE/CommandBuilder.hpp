/** \ingroup CPU */
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
							{ for (auto i : _commands) delete (i.second); }

		Command* command (const std::string& cmd) const;

		protected:
		virtual Command* createEmptyCommand (const std::string& cmdName) const = 0;

		// Implementation
		std::string readCommandName (const std::string& cmd) const;
		Attributes readCommandParameters (const std::string& cmd) const;

		mutable std::map <std::string, Command*> _commands;
	};
}

#endif
  
// End of the file
/*@}*/
