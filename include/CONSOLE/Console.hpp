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
		  * @param oS	A reference to the ostream where to send any message. \n
		  * @param cK	Number of commands kept. It can be greater than 255.
		  */
		Console (Emulator* e, CommandBuilder* cB,
			const std::string& cF = "./console.def", std::ostream& oS = std::cout, size_t cK = 100);

		Console (const Console&) = delete;

		Console& operator = (const Console&) = delete;

		/** To change the number of commands kept. */
		size_t maxCommandsKept () const
							{ return (_maxCommandsKept); }
		void setMaxCommandsKept (size_t mC)
							{ if (mC <= 255) _maxCommandsKept = mC; }

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
		static const char _RIGHTKEY = -1; // Cursor right
		static const char _LEFTKEY = -2; // Cursor left
		static const char _UPKEY = -3; // To recover previous instructions
		static const char _DOWNKEY = -4; // To recover next instructions
		static const char _ENTERKEY = -5; // Execute
		static const char _DELETEKEY = -6; // Del the char selected and maintains the position
		static const char _BACKKEY = -7; // Del the char before the selected position and move into that
		static const char _ENDKEY = -8; // Go to the end of the line
		static const char _BEGINKEY = -9; // Go to the beginning of the line

		/** The way a character is read from the console is different depending on the OS. \n
			The method returns true when a char is read and false in other case. \n
			The internal variable chr holds the char code read if any!. \n
			This method has to manage also the special characters. */
		virtual bool readChar (char& chr) const = 0;

		protected:
		Emulator* _emulator;
		std::ostream& _outputStream;
		size_t _maxCommandsKept;

		// Implementation
		std::string _command;
		/** It is even possible to get back the last command introduced. */
		std::vector <std::string> _lastCommands;
		size_t _lastCommandPosition;
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
