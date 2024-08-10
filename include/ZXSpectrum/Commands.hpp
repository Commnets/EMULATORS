/** \ingroup ZXSPECTRUM */
/*@{*/

/**
 *	@file
 *	File: Commands.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/08/2024 \n
 *	Description: Specific Commands for a ZXSpectrum emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __ZXSPECTRUM_COMMANDS__
#define __ZXSPECTRUM_COMMANDS__

#include <CORE/incs.hpp>
#include <SINCLAIR/incs.hpp>

namespace ZXSPECTRUM
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

	/** To activate the visualization of the important events
		in the relation between CPU and the Screen. */
	class ULAShowEventsCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 201;
		static const std::string _NAME;

		ULAShowEventsCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		/** The parameter can be ON or OFF. */
		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 1); }

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};
}

#endif

// End of the file
/*@}*/
