/** \ingroup VIC20 */
/*@{*/

/**
 *	@file
 *	File: CommandBuilder.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 09/12/2023 \n
 *	Description: To build specific command in a VIC20 environment.
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_COMMANDBUILDER__
#define __VIC20_COMMANDBUILDER__

#include <CORE/incs.hpp>
#include <COMMODORE/incs.hpp>

namespace VIC20
{
	class CommandBuilder final : public MCHEmul::CommandBuilder
	{
		public:
		/** This command builder is created together with the COMMODORE Command Builder one. */
		CommandBuilder (MCHEmul::CommandBuilder* nB = nullptr)
			: MCHEmul::CommandBuilder (new COMMODORE::CommandBuilder (nB))
							{ }

		protected:
		virtual MCHEmul::Command* createEmptyCommand (const std::string& cmdName) const override;
	};
}

#endif

// End of the file
/*@}*/
