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
	/** To create commands from an string that represents it. */
	class CommandBuilder
	{
		public:
		virtual ~CommandBuilder ()
							{ }

		Command* createCommand (const std::string& cmd) const;

		protected:
		virtual Command* createEmptyCommand (const std::string& cmdName) const = 0;

		// Implementation
		std::string readCommandName (const std::string& cmd) const;
		Attributes readCommandParameters (const std::string& cmd) const;
	};
}

#endif
  
// End of the file
/*@}*/
