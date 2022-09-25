/** \ingroup CPU */
/*@{*/

/**
 *	@file
 *	File: StdCommands.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 04/08/2022 \n
 *	Description: The very basic command over the computer.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_STDCOMMANDS__
#define __MCHEMUL_STDCOMMANDS__

#include <CORE/Command.hpp>

namespace MCHEmul
{
	/** To get the status register. */
	class StatusRegisterCommand final : public Command
	{
		public:
		static const int _ID = 0;
	
		StatusRegisterCommand ()
			: Command (_ID)
							{ }

		virtual bool canBeExecuted () const override
							{ return (true); }

		protected:
		virtual void executeImpl (Computer* c, Attributes& rst) override;
	};
}

#endif

// End of the file
/*@}*/
