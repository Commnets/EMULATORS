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

#ifndef __MCHEMUL_CONSOLE__
#define __MCHEMUL_CONSOLE__

#include <EMULATORS/incs.hpp>

namespace MCHEmul
{
	/** The console represents typically DOS black screen. \n
		The answers can be resent to any type of output device (defined at construction level). */
	class Console : public CommandExecuter
	{
		public:
		static const int _ID = -1;

		Console () = delete;

		/**
		  * The constructor reveives: \n
		  * @param e	The emulator on top of which this console works. \n
		  * @param cB	The builder used to built up the command introduced throught the console. \n
		  * @param cF	A file where the messages, prompt type and other of the console are defined. \n
		  *				The structure of this file is: \n
		  *				#... line meaning a comment and not taken into account. \n
		  *				First valid line, with the message to print out when a command can't not be executed. \n
		  *				Second valid line, with the message introduce doesn't exist. \n
		  *				Third valid line, with the prompt! \n
		  *				Rest of the lines, with the welcome mesage. \n
		  *				Blank lines are taking into account.
		  * @param oS	A reference to the ostream where to send any message.
		  */
		Console (Emulator* e, CommandBuilder* cB,
			const std::string& cF = "./console.def", std::ostream& oS = std::cout);

		Console (const Console&) = delete;

		Console& operator = (const Console&) = delete;

		void run ();

		protected:
		// Managing commands...
		/** Read and execute the commands if any. \n
			This method uses readCommand one. \n
			Returns true when the command execute is QUIT, and false in other circusntance. */
		bool readAndExecuteCommand ();
		/** Returns true when a instruction is ready to be executed and false in other case. */
		bool readCommand ();

		/** Just print out the answer (method comming from the CommandExecuter class). */
		virtual void manageAnswer (Command* c, const InfoStructure& rst) override;
		/** Just print out the error. */
		virtual void manageErrorInExecution (Command* c, const InfoStructure& rst) override;

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
		Emulator* _emulator;
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
		Win32Console (Emulator* e, CommandBuilder* cB)
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
