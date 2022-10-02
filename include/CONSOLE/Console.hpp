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

		Console (Emuls::Emulator* e, MCHEmul::CommandBuilder* cB, 
			const std::string& cF = "./console.def", std::ostream& oS = std::cout);

		Console (const Console&) = delete;

		Console& operator = (const Console&) = delete;

		virtual ~Console ()
							{ delete (_commandBuilder); }

		void run ();

		protected:
		// Managing commands...
		/** Read and execute the commands if any. \n
			This method uses readCommand one. \n
			Returns true when the command execute is QUIT, and false in other circusntance. */
		bool readAndExecuteCommand ();
		/** Returns true when a instruction is ready to be executed and false in other case. */
		bool readCommand ();

		/** Standard characters managed by the system.
			The console is always inserting characters. */
		static const char _RIGHTKEY = -1;
		static const char _LEFTKEY = -2;
		static const char _ENTERKEY = -3;
		static const char _DELETEKEY = -4;
		static const char _BACKKEY = -5;

		/** The way a character is read from the console is different depending on the OS. \n
			The method returns true when a char is read and false in other case. \n
			The internal variable chr holds the char code read if any!. \n
			This method has to manage also the special characters. */
		virtual bool readChar (char& chr) const = 0;

		protected:
		Emuls::Emulator* _emulator;
		MCHEmul::CommandBuilder* _commandBuilder;
		std::ostream& _outputStream;

		// Implementation
		std::string _command;
		size_t _cursorPosition;

		std::string _commandErrorTxt;
		std::string _commandDoesnExitTxt;
		std::string _welcomeTxt;
		std::string _commandPrompt;
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
