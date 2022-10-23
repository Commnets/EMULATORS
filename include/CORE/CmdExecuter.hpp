/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: CmdExecuter.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 22/10/2022 \n
 *	Description: This class defines the root from any class able to execute commands, has to inherit from. 
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_CMDEXECUTER__
#define __MCHEMUL_CMDEXECUTER__

#include <CORE/InfoStructure.hpp>
#include <CORE/CommandBuilder.hpp>
#include <queue>

namespace MCHEmul
{
	class Command;
	class Computer;

	/* Very basic class just to process commands stored in a list. */
	class CommandExecuter
	{
		public:
		CommandExecuter (int id, CommandBuilder* cB)
			: _id (id), _commandBuilder (cB)
							{ assert (_commandBuilder != nullptr); }

		CommandExecuter (CommandExecuter&) = delete;

		CommandExecuter& operator = (const CommandExecuter&) = delete;

		virtual ~CommandExecuter ()
							{ delete (_commandBuilder); }

		int id () const
							{ return (_id); }

		const CommandBuilder* commandBuilder () const
							{ return (_commandBuilder); }
		CommandBuilder* commandBuilder ()
							{ return (_commandBuilder); }

		/** To know whether there is pending commands or not. */
		bool isPendingCommands () const
							{ return (!_commands.empty ()); }

		// Executing commands
		// None of the can be overloaded, but both ends up invoking manageAnswer that can be overlaoded
		/** This method just only adds the command to th list of pending ones.
			The list will only be execute when the method executePendingCommands is executed,
			something that has to be decided by further classes built inheriting from this one. */
		void executeCommand (Command* cmd, Computer* c);
		/** To execute inmediatly the command.
			Returns true if the command was executed properly and false in other case. */
		bool executeCommandNow (Command* cmd, Computer* c);

		protected:
		/** To execute the pending list of commands.
			The method returns true when all command pending were executed right, and false if any was wrong. */
		bool executePendingCommands ();

		/** This method has to be redefined to determine what to do with the answer of a command. \n
			It will depend on the type of CommandExecuter. E.g. in a console, the answer should be printed out to the screen
			whilst in a remote communication it has to be sent back to the invoker. */
		virtual void manageAnswer (Command *c, const InfoStructure& rst) = 0;
		/** This method has also to be redefined to determine ehat to don with an error in 
			the execution of a command. */
		virtual void manageErrorInExecution (Command* c, const InfoStructure& rst) = 0;

		protected:
		/** To keep the invocations. */
		struct ComputerPlusCommand
		{
			ComputerPlusCommand (Computer* c, Command* cmd)
				: _computer (c), _command (cmd)
							{ }

			Computer* _computer;
			Command * _command;
		};

		int _id;
		CommandBuilder* _commandBuilder;
		std::queue <ComputerPlusCommand> _commands;
	};
}

#endif
  
// End of the file
/*@}*/
