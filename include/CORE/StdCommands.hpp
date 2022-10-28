/** \ingroup CORE */
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
		static const std::string _NAME;
	
		HelpCommand (const std::string& hF = "./commands.hlp");

		/** Eiher no parameters to show all commands possible or 
			just a command with the name of the command which info has be shown. */
		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0 || _parameters.size () == 1); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;

		private:
		using HelpInfo = std::map <std::string, Strings>;
		HelpInfo _helpInfo;
	};

	/** Just to know the name of the author. */
	class AuthorInfoCommand final : public Command
	{
		public:
		static const int _ID = -2;
		static const std::string _NAME;

		AuthorInfoCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		/** The fields returned are: \n
			AUTHOR	= InfoStructure: Information about the author. */
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To get the status of the status register inside any computer. */
	class StatusRegisterStatusCommand final : public Command
	{
		public:
		static const int _ID = 0;
		static const std::string _NAME;

		StatusRegisterStatusCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (true); }

		private:
		/** The fields returned are: \n
			SR	= Attribute: Status register info. */
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To get the status of every register defined in the cpu of any computer. */ 
	class RegistersStatusCommand final : public Command
	{
		public:
		static const int _ID = 1;
		static const std::string _NAME;
	
		RegistersStatusCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		/** The fields returned are: \n
			REGS	= InfoStructure: Info about the registers. */
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To get the status of the program counter. */
	class ProgramCounterStatusCommand final : public Command
	{
		public:
		static const int _ID = 2;
		static const std::string _NAME;

		ProgramCounterStatusCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		/** The fields returned are: \n
			PC	= Attribute: Prohramm Counter info. */
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To get the status of the stack.
		The command can be invoked in two different ways:
		One very simple way with no parameters to get just basic information about the stack,
		and other with the paremeter "ALL" to get even the content of the stack itself. */
	class StackStatusCommand final : public Command
	{
		public:
		static const int _ID = 3;
		static const std::string _NAME;

		StackStatusCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0 || 
								(_parameters.size () == 1 && _parameters.find ("ALL") != _parameters.end ())); }

		private:
		/** The fields returned are: \n
			@see Stack::getInfoStructure method. \n
			The InfoStructure MEMORY is not included when "ALL" parameter is not provided. */
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To get the status of the CPU inside any computer. \n
		It is a complex command made up of others: StatusRegisterStatusCommand,... \n
		The information about the stack is the simpliest one. */
	class CPUStatusCommand final : public ComplexCommand
	{
		public:
		static const int _ID = 4;
		static const std::string _NAME;

		CPUStatusCommand ();

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override
							{ /** Nothing special to do. */ }
	};

	/** Similar to the previous one but without info about the stack. \n
		it is also used by the command NEXT. */
	class CPUSimpleStatusCommand final : public ComplexCommand
	{
		public:
		static const int _ID = 5;
		static const std::string _NAME;

		CPUSimpleStatusCommand ();

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override
							{ /** Nothing special to do. */ }
	};

	/** To get info about the cpu. */
	class CPUInfoCommand final : public Command
	{
		public:
		static const int _ID = 6;
		static const std::string _NAME;

		CPUInfoCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		/** The fields returned are: \n
			@see CPU::getInfoStructure method. */
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To get the content of the memory. \n
		The command must have 1 parameter at least with the direction which value is requested. \n
		A second parameter might be provided, and the the content between those two memory locations is got. \n
		The address can be in octal, hexadecimal or decimal. */
	class MemoryStatusCommand final : public Command
	{
		public:
		static const int _ID = 7;
		static const std::string _NAME;

		MemoryStatusCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 1 || _parameters.size () == 2); }

		private:
		/** The fields returned are: \n
			BYTES = Attributes: The bytes in the locations requested. */
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To stop the cpu.
		The IO will continue working. */
	class StopCPUCommand final : public Command
	{
		public:
		static const int _ID = 8;
		static const std::string _NAME;

		StopCPUCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To run the cpu, usually after a stop command has been executed. */
	class RunCPUCommand final : public Command
	{
		public:
		static const int _ID = 9;
		static const std::string _NAME;

		RunCPUCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To run just the next instruction of the CPU. */
	class NextInstructionCommand final : public Command
	{
		public:
		static const int _ID = 10;
		static const std::string _NAME;

		NextInstructionCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To get info about the last instruction executed. */
	class LastIntructionCPUCommand final : public Command
	{
		public:
		static const int _ID = 11;
		static const std::string _NAME;

		LastIntructionCPUCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		/** The fields returned are: \n
			INST = Attribute: String with the last instruction executed. */
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To get a list with all break points. */
	class ListOfBreakPointsCommand final : public Command
	{
		public:
		static const int _ID = 12;
		static const std::string _NAME;

		ListOfBreakPointsCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		/** The fields returned are: \n
			BREAKPOINTS	= Attribute: A list (separated by comma) of all break points. */
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To set a break point (or a set of them) in the code. */
	class SetBreakPointCommand final : public Command
	{
		public:
		static const int _ID = 13;
		static const std::string _NAME;

		SetBreakPointCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () > 0); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To remove a break point (or a set of them) in the code. */
	class RemoveBreakPointCommand final : public Command
	{
		public:
		static const int _ID = 14;
		static const std::string _NAME;

		RemoveBreakPointCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () > 0); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To remove all break points. */
	class RemoveAllBreakPointsCommand final : public Command
	{
		public:
		static const int _ID = 15;
		static const std::string _NAME;

		RemoveAllBreakPointsCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To know the speed of the processor. */
	class CPUSpeedCommand final : public Command
	{
		public:
		static const int _ID = 16;
		static const std::string _NAME;

		CPUSpeedCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};
}

#endif

// End of the file
/*@}*/
