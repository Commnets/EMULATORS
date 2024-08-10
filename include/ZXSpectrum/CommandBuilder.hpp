/** \ingroup ZXSPECTRUM */
/*@{*/

/**
 *	@file
 *	File: CommandBuilder.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/08/2024 \n
 *	Description: To build specific command in a ZXSpectrum environment.
 *	Versions: 1.0 Initial
 */

#ifndef __ZXSPECTRUM_COMMANDBUILDER__
#define __ZXSPECTRUM_COMMANDBUILDER__

#include <CORE/incs.hpp>
#include <SINCLAIR/incs.hpp>

namespace ZXSPECTRUM
{
	class CommandBuilder final : public MCHEmul::CommandBuilder
	{
		public:
		/** This command builder is created together with the COMMODORE Command Builder one. */
		CommandBuilder (MCHEmul::CommandBuilder* nB = nullptr)
			: MCHEmul::CommandBuilder (new SINCLAIR::CommandBuilder (nB))
							{ }

		protected:
		virtual MCHEmul::Command* createEmptyCommand (const std::string& cmdName) const override;
	};
}

#endif

// End of the file
/*@}*/
