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
	/** The console working over an emulation being executed locally. \n
		The local console can load the three types of file that are managed in the emulation: \n
		"assembler", "binary" file and "set of blocks" file. @see Emulator for further details. */
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

		// Loading programs
		/** To load a binary file. \n
			where the address where to address to load the program from is at the header of the file. */
		InfoStructure loadBinaryFile (const std::string& nP) const;
		/** To load a program.
		This is avery special command, using the one defined inthe emulator behind. */
		InfoStructure loadProgram (const std::string& nP) const;
		/** To load a block of files. */
		InfoStructure loadBlocksFile (const std::string& nP) const;
		
		/** To decompile the memory. */
		InfoStructure decompileMemory (const std::string& prms);

		protected:
		/** The emulator over which the console work. */
		Emulator* _emulator;
	};
}

#endif

// End of the file
/*@}*/
