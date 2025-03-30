/** \ingroup MSX */
/*@{*/

/**
 *	@file
 *	File: CommandBuilder.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/03/2025 \n
 *	Description: To build specific command in a MSX environment.
 *	Versions: 1.0 Initial
 */

#ifndef __MSX_COMMANDBUILDER__
#define __MSX_COMMANDBUILDER__

#include <CORE/incs.hpp>

namespace MSX
{
	class CommandBuilder final : public MCHEmul::CommandBuilder
	{
		public:
		/** This command builder is created together with the COMMODORE Command Builder one. */
		CommandBuilder (MCHEmul::CommandBuilder* nB = nullptr)
			: MCHEmul::CommandBuilder (new MCHEmul::StandardCommandBuilder (nB))
							{ }

		protected:
		virtual MCHEmul::Command* createEmptyCommand (const std::string& cmdName) const override;
	};
}

#endif

// End of the file
/*@}*/
