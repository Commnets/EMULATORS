/** \ingroup SINCLAIR */
/*@{*/

/**
 *	@file
 *	File: CommandBuilder.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 24/03/2024 \n
 *	Description: To build specific command in a SINCLAIR environments.
 *	Versions: 1.0 Initial
 */

#ifndef __SINCLAIR_COMMANDBUILDER__
#define __SINCLAIR_COMMANDBUILDER__

#include <CORE/incs.hpp>

namespace SINCLAIR
{
	class CommandBuilder final : public MCHEmul::CommandBuilder
	{
		public:
		CommandBuilder (MCHEmul::CommandBuilder* nB = nullptr)
			: MCHEmul::CommandBuilder (nB)
							{ }

		protected:
		virtual MCHEmul::Command* createEmptyCommand (const std::string& cmdName) const override;
	};
}

#endif

// End of the file
/*@}*/
