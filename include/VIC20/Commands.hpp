/** \ingroup VIC20 */
/*@{*/

/**
 *	@file
 *	File: Commands.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/12/2023 \n
 *	Description: Specific Commands for a VIC20 emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_COMMANDS__
#define __VIC20_COMMANDS__

#include <CORE/incs.hpp>

namespace VIC20
{
	/** To get the status of the VIA1 chip. */
	class VIA1StatusCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 200;
		static const std::string _NAME;

		VIA1StatusCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};

	/** To get the status of the VIA2 chip. */
	class VIA2StatusCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 201;
		static const std::string _NAME;

		VIA2StatusCommand ()
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
