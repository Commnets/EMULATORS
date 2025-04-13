/** \ingroup COMMODORE */
/*@{*/

/**
 *	@file
 *	File: CommandBuilder.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 01/10/2022 \n
 *	Description: To build specific command in a COMMODORE environment.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_COMMANDBUILDER__
#define __COMMODORE_COMMANDBUILDER__

#include <CORE/incs.hpp>

namespace COMMODORE
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
