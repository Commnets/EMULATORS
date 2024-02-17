/** \ingroup CONSOLE */
/*@{*/

/**	
 *	@file	
 *	File: Console.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 04/08/2022 \n
 *	Description: To manage the emulator from a console.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_CONSOLE__
#define __MCHEMUL_CONSOLE__

#include <EMULATORS/incs.hpp>
#include <CONSOLE/ConsoleKeys.hpp>

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
		  * @param cB	The builder used to built up the command introduced throught the console. \n
		  * @param k	The Class amanaging the console keys (will depend on the OS). \n
		  * @param cF	A file where the messages, prompt type and other of the console are defined. \n
		  *				The structure of this file is: \n
		  *				#... line meaning a comment and not taken into account. \n
		  *				First valid line, with the message to print out when a command can't not be executed. \n
		  *				Second valid line, with the message introduce doesn't exist. \n
		  *				Third valid line, with the prompt! \n
		  *				Rest of the lines, with the welcome mesage. \n
		  *				Blank lines are taking into account.
		  * @param oS	A reference to the ostream where to send any message. \n
		  * @param cK	Number of commands kept. It cannot be greater than 255.
		  */
		Console (CommandBuilder* cB, ConsoleKeys* k,
			const std::string& cF = "./console.def", std::ostream& oS = std::cout, size_t cK = 100);

		Console (const Console&) = delete;

		Console& operator = (const Console&) = delete;

		virtual ~Console () override
							{ delete (_consoleKeys); }

		Console (Console&&) = delete;

		Console& operator = (Console&&) = delete;

		/** To get the output stream. */
		std::ostream& outputStream ()
							{ return (_outputStream); }

		/** To change the number of commands kept. */
		size_t maxCommandsKept () const
							{ return (_maxCommandsKept); }
		void setMaxCommandsKept (size_t mC)
							{ if (mC <= 255) _maxCommandsKept = mC; }

		/** To hold the console, it means no instructions are accepted,
			and the only instruction executed within run method is runPerCycle. */
		bool consoleHold () const
							{ return (_consoleHold); }
		void setConsoleHold (bool cH)
							{ _consoleHold = cH; }

		virtual void run ();

		/** Just print out the answer (method comming from the CommandExecuter class). */
		virtual void manageAnswer (Command* c, const InfoStructure& rst) override;
		/** Just print out the error. */
		virtual void manageErrorInExecution (Command* c, const InfoStructure& rst) override;

		protected:
		// Managing commands...
		/** Read and execute the commands if any. \n
			This method uses readCommand one. \n
			This method also uses createAndExecuteCommand. \n
			Returns true when the command execute is QUIT, and false in other circusntance. */
		bool readAndExecuteCommand ();
		/** Returns true when a instruction is ready to be executed and false in other case. */
		bool readCommand ();
		/** And execute the command. 
			It must be overloaded depending omn the type of Console. */
		virtual void createAndExecuteCommand () = 0;

		/** Run things per cycle. \n
			It is and "exit" door for simulations. 
			It returns true if after the execution, the run loop has to finish. */
		virtual bool runPerCycle () = 0;

		protected:
		ConsoleKeys* _consoleKeys;
		std::ostream& _outputStream;
		size_t _maxCommandsKept;
		Clock _clock;

		// Implementation
		std::string _command;
		/** It is even possible to get back the last command introduced. */
		std::vector <std::string> _lastCommands;
		size_t _lastCommandPosition;
		size_t _cursorPosition;
		bool _consoleHold;

		std::string _commandErrorTxt;
		std::string _commandDoesnExitTxt;
		std::string _welcomeTxt;
		std::string _commandPrompt;
	};
}

#endif
  
// End of the file
/*@}*/
