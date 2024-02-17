/** \ingroup COMMS */
/*@{*/

/**
*	@file
*	File: CommandBuilder.hpp \n
*	Framework: CPU Emulators library \n
*	Author: Ignacio Cea Fornies (EMULATORS library) \n
*	Creation Date: 03/12/2022 \n
*	Description:	To build only the command related with an answer!
*					It is used in the consoles o similar sending commands to a remote emultation.
*	Versions: 1.0 Initial
*/

#ifndef __MCHEMUL_COMMSCOMMANDBLD__
#define __MCHEMUL_COMMSCOMMANDBLD__

#include <CORE/incs.hpp>

namespace MCHEmul
{
	class CommsSystemAnswerCommandBuilder : public CommandBuilder
	{
		public:
		CommsSystemAnswerCommandBuilder (CommandBuilder* nB = nullptr)
			: CommandBuilder (nB)
							{ }

		protected:
		virtual Command* createEmptyCommand (const std::string& cmdName) const override;
	};
}

#endif

// End of the file
/*@}*/
