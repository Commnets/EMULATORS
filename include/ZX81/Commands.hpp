/** \ingroup ZX81 */
/*@{*/

/**
 *	@file
 *	File: Commands.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 24/03/2024 \n
 *	Description: Specific Commands for a ZX81 emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __ZX81_COMMANDS__
#define __ZX81_COMMANDS__

#include <CORE/incs.hpp>

namespace ZX81
{
	/** To get the status of the ULA chip. */
	class ULAStatusCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 200;
		static const std::string _NAME;

		ULAStatusCommand ()
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
