/** \ingroup C64 */
/*@{*/

/**
 *	@file
 *	File: Commands.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 01/10/2022 \n
 *	Description: Specific Commands for a C64 emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_COMMANDS__
#define __C64_COMMANDS__

#include <CORE/incs.hpp>

namespace C64
{
	/** To get the status of the VIC chip. */
	class VICStatusCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 100;
	
		VICStatusCommand ()
			: MCHEmul::Command (_ID)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};

	/** To get the status of the CIA2 chip. */
	class CIA1StatusCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 101;
	
		CIA1StatusCommand ()
			: MCHEmul::Command (_ID)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};

	/** To get the status of the CIA2 chip. */
	class CIA2StatusCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 102;
	
		CIA2StatusCommand ()
			: MCHEmul::Command (_ID)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};
}

#endif

// End of the file
/*@}*/
