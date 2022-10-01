/** \ingroup CPU */
/*@{*/

/**
 *	@file
 *	File: StdCommands.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 04/08/2022 \n
 *	Description:	The very basic commands to execute over a computer. \n
 *					Broadly speaking the structure of the names of name of any command is as follows: \n
 *					COMPUTERELEMENTAFFECTED+Status|Info+Command. \n
 *					Status to indicate that the content of the element affected is going to be get. \n
 *					Info to indicate that the definition of the element affected will be got.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_STDCOMMANDS__
#define __MCHEMUL_STDCOMMANDS__

#include <CORE/Command.hpp>

namespace MCHEmul
{
	/** Just to get info about all commands that can be executed. \n
		That info is extracted from a file. \n
		The structure of the file should be as follows: \n
		# Line of comments. Ignored. \n
		; Line to define that the info for a command starts. 
		  The command name should be just after and it will be stored in UPPERCASE always \n
		Normal line = Info about the command defined in the previous line. */
	class HelpCommand : public Command
	{
		public:
		static const int _ID = -1;
	
		HelpCommand (const std::string& hF = "./commands.txt");

		/** Eiher no parameters to show all commands possible or 
			just a command with the name of the command which info has be shown. */
		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0 || _parameters.size () == 1); }

		protected:
		virtual void executeImpl (Computer* c, Attributes& rst) override;

		private:
		using HelpInfo = std::map <std::string, Strings>;
		HelpInfo _helpInfo;
	};

	/** To get the status of the status register inside any computer. */
	class StatusRegisterStatusCommand final : public Command
	{
		public:
		static const int _ID = 0;
	
		StatusRegisterStatusCommand ()
			: Command (_ID)
							{ }

		virtual bool canBeExecuted () const override
							{ return (true); }

		protected:
		virtual void executeImpl (Computer* c, Attributes& rst) override;
	};

	/** To get the status of every register defined in the cpu of any computer. */ 
	class RegistersStatusCommand final : public Command
	{
		public:
			static const int _ID = 1;
	
			RegistersStatusCommand ()
			: Command (_ID)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		protected:
		virtual void executeImpl (Computer* c, Attributes& rst) override;
	};

	/** To get the status of the program counter. */
	class ProgramCounterStatusCommand final : public Command
	{
		public:
		static const int _ID = 2;

		ProgramCounterStatusCommand ()
			: Command (_ID)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		protected:
		virtual void executeImpl (Computer* c, Attributes& rst) override;

	};

	/** To get the status of the stack.
		The command can be invoked in two different ways:
		One very simple way with no parameters to get just basic information about the stack,
		and other with the paremeter "ALL" to get even the content of the stack itself. */
	class StackStatusCommand final : public Command
	{
		public:
		static const int _ID = 3;

		StackStatusCommand ()
			: Command (_ID)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0 || 
								(_parameters.size () == 1 && _parameters.find ("ALL") != _parameters.end ())); }

		protected:
		virtual void executeImpl (Computer* c, Attributes& rst) override;
	};

	/** To get the status of the CPU inside any computer. \n
		It is a complex command made up of others: StatusRegisterStatusCommand,... \n
		The information about the stack is the simpliest one. */
	class CPUStatusCommand final : public ComplexCommand
	{
		public:
		static const int _ID = 4;

		CPUStatusCommand ()
			: ComplexCommand (_ID, 
				Commands (
					{ new StatusRegisterStatusCommand,
					  new RegistersStatusCommand,
					  new ProgramCounterStatusCommand,
					  new StackStatusCommand
					}))
							{ }

		protected:
		virtual void executeImpl (Computer* c, Attributes& rst) override
							{ /** Nothing special to do. */ }
	};

	/** To get info about the cpu. */
	class CPUInfoCommand final : public Command
	{
		public:
		static const int _ID = 5;

		CPUInfoCommand ()
			: Command (_ID)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		protected:
		virtual void executeImpl (Computer* c, Attributes& rst) override;
	};
}

#endif

// End of the file
/*@}*/
