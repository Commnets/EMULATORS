/** \ingroup CONSOLE */
/*@{*/

/**	
 *	@file	
 *	File: Console.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 04/08/2022 \n
 *	Description: To manage the emulator from a console.
 *	Versions: 1.0 Initial
 */

#ifndef __CONSOLE_CONSOLE__
#define __CONSOLE_CONSOLE__

#include <EMULATORS/incs.hpp>

namespace Console
{
	class Console 
	{
		public:
		Console () = delete;

		Console (Emuls::Emulator* e, MCHEmul::CommandBuilder* cB);

		Console (const Console&) = delete;

		Console& operator = (const Console&) = delete;

		virtual ~Console ()
							{ delete (_commandBuilder); }

		void run ();

		protected:
		/** Returns true when a instruction is ready and false in other case. */
		bool readCommand ();

		/** The way a character is read from the console is different depending on the OS. \n
			The method returns true when a char is read and false in other case. \n
			The internal variable chr holds the char code read if any!. */
		virtual bool readChar (char& chr) const = 0;

		protected:
		Emuls::Emulator* _emulator;
		MCHEmul::CommandBuilder* _commandBuilder;

		// Implementation
		std::string _command;
	};

#ifdef _WIN32
	/** A expecial class for W32 systems. */
	class Win32Console final : public Console
	{
		public:
		Win32Console (Emuls::Emulator* e, MCHEmul::CommandBuilder* cB)
			: Console (e, cB)
							{ }

		protected:
		virtual bool readChar (char& chr) const override;
	};
#endif
}

#endif
  
// End of the file
/*@}*/
