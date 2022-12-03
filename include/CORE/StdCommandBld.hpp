/** \ingroup CORE */
/*@{*/

/**
 *	@file
 *	File: StdCommandBld.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 25/09/2022 \n
 *	Description: To build the very basic commands.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_STDCOMMANDBLD__
#define __MCHEMUL_STDCOMMANDBLD__

#include <CORE/CommandBuilder.hpp>

namespace MCHEmul
{
	class StandardCommandBuilder : public CommandBuilder
	{
		protected:
		virtual Command* createEmptyCommand (const std::string& cmdName) const override;
	};
}

#endif

// End of the file
/*@}*/
