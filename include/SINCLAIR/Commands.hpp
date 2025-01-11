/** \ingroup SINCLAIR */
/*@{*/

/**
 *	@file
 *	File: Commands.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 14/01/2023 \n
 *	Description: Specific Commands for a SINCLAIR environment.
 *	Versions: 1.0 Initial
 */

#ifndef __SINCLAIR_COMMANDS__
#define __SINCLAIR_COMMANDS__

#include <CORE/incs.hpp>

namespace SINCLAIR
{
	/** To get the variables of the system. */
	class SystemVariablesCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 100;
		static const std::string _NAME;

		SystemVariablesCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (MCHEmul::CommandExecuter* cE, 
			MCHEmul::Computer* c, MCHEmul::InfoStructure& rst) override;
	};

	/** To get one variable of the system. */
	class SystemVariableCommand final : public MCHEmul::Command
	{
		public:
		static const int _ID = 101;
		static const std::string _NAME;

		SystemVariableCommand ()
			: MCHEmul::Command (_ID, _NAME)
							{ }

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
