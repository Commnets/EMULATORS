/** \ingroup MSX */
/*@{*/

/**
 *	@file
 *	File: Commands.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/03/2024 \n
 *	Description: Specific Commands for a MSX emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __MSX_COMMANDS__
#define __MSX_COMMANDS__

#include <CORE/incs.hpp>
#include <SINCLAIR/incs.hpp>

namespace MSX
{
	/** To get the status of the VDP chip. */
	class VDPStatusCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 100;
		static const std::string _NAME;
	
		VDPStatusCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};
}

#endif

// End of the file
/*@}*/
