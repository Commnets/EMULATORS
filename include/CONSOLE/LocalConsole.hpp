/** \ingroup CONSOLE */
/*@{*/

/**	
*	@file	
*	File: LocalConsole.hpp \n
*	Framework: CPU Emulators library \n
*	Author: Ignacio Cea Forniés (EMULATORS library) \n
*	Creation Date: 02/11/2022 \n
*	Description: The console over a local emulation.
*	Versions: 1.0 Initial
*/

#ifndef __MCHEMUL_LOCALCONSOLE__
#define __MCHEMUL_LOCALCONSOLE__

#include <CONSOLE/Console.hpp>

namespace MCHEmul
{
	/** The console working over an emulation being executed locally. */
	class LocalConsole : public Console
	{
		public:
		/**
		* The constructor reveives: \n
		* @param e	The emulator on top of which this console works. \n
		* And also the parameters needed for the parent class (@see Console).
		*/
		LocalConsole (Emulator* e, CommandBuilder* cB, ConsoleKeys* k,
			const std::string& cF = "./console.def", std::ostream& oS = std::cout, size_t cK = 100);

		virtual void run () override;

		protected:
		virtual void createAndExecuteCommand () override;

		virtual bool runPerCycle () override;

		/** To load a program.
		This is avery special command, using the one defined inthe emulator behind. */
		InfoStructure loadProgram (const std::string& nP) const;

		protected:
		/** The emulator over which the console work. */
		Emulator* _emulator;
	};
}

#endif

// End of the file
/*@}*/
