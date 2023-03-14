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
		Command line: HELP (COMMAND) \n
		The structure of the file should be as follows: \n
		# Line of comments. Ignored. \n
		; Line to define that the info for a command starts. 
		  The command name should be just after and it will be stored in UPPERCASE always \n
		Normal line = Info about the command defined in the previous line. */
	class HelpCommand final : public Command
	{
		public:
		static const int _ID = -1;
		static const std::string _NAME;
	
		HelpCommand (const std::string& hF = "./Standard.hlp");

		/** Eiher no parameters to show all commands possible or 
			just a command with the name of the command which info has be shown. */
		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0 || _parameters.size () == 1); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;

		// Implementation
		/** Just to read a file and get the info of the commands managed. \n
			In many help files were nested (starting with '?') they would be all loaded. \n
			The comment lines (starting with '#') are ignored. */
		Strings loadHelpFile (const std::string& hF);

		private:
		using HelpInfo = std::map <std::string, Strings>;
		HelpInfo _helpInfo;
	};

	/** Just to know the name of the author. 
		AUTHOR */
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

	/** To get the status of the status register inside any computer. \n
		Command line: STATUS */
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

	/** To get the status of every register defined in the cpu of any computer. \n
		Command line: STATUS */ 
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

	/** To get the status of the program counter. \n
		Command line: PC */
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
		and other with the paremeter "ALL" to get even the content of the stack itself. 
		Command line: STACK */
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
		The information about the stack is the simpliest one. \n
		Command line: CPUSTATUS */
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
		it is also used by the command NEXT. \n
		Command line: CPUSSTATUS */
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

	/** To get info about the cpu. \n
		Command line: CPUINFO */
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
		The address can be in octal, hexadecimal or decimal. \n
		Command line: MEMORY ADDRESS (OTHER ADDRESS) */
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

	/** To change the value of a memory location. 
		The command is usefull to set only one location of a set of them with the same value. \n
		So 3 parameters could be provided. \n
		Command line: SETMEMORY ADDRESS (FINAL ADDRESS) VALUE */
	class SetMemoryValueCommand final : public Command
	{
		public:
		static const int _ID = 8;
		static const std::string _NAME;

		SetMemoryValueCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 2 /** One location. */ ||
									  _parameters.size () == 3 /** a range. */); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To stop the cpu. \n
		The IO will continue working, so the graphics are still in the screen but stopeed. \n
		Command line: STOP */
	class StopCPUCommand final : public Command
	{
		public:
		static const int _ID = 9;
		static const std::string _NAME;

		StopCPUCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To run the cpu, usually after a stop command has been executed. \n
		The run can start from another location passed optionally as parameter. \n
		Command line: RUN (ADDRESS) */
	class RunCPUCommand final : public Command
	{
		public:
		static const int _ID = 10;
		static const std::string _NAME;

		RunCPUCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0 || _parameters.size () == 1); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** Just to change the value of the Proram Counter. */
	class SetProgramCounterCommand final : public Command
	{
		public:
		static const int _ID = 11;
		static const std::string _NAME;

		SetProgramCounterCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 1); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To run just the next instruction of the CPU. \n
		Command line: NEXT */
	class NextInstructionCommand final : public Command
	{
		public:
		static const int _ID = 12;
		static const std::string _NAME;

		NextInstructionCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To know what the next/nexts instruction/s will be. */
	class ShowNextInstructionCommand final : public Command
	{
		public:
		static const int _ID = 13;
		static const std::string _NAME;

		ShowNextInstructionCommand ()
			: Command (_ID, _NAME)
							{ }

		/** With 0 parameters only the next instruction, otherwise the number indicated. */
		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0 || _parameters.size () == 1); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To get info about the last instruction executed. \n
		Command line: INST */
	class LastIntructionCPUCommand final : public Command
	{
		public:
		static const int _ID = 14;
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

	/** To get a list with all break points. \n
		Command line: BREAKS */
	class ListOfBreakPointsCommand final : public Command
	{
		public:
		static const int _ID = 15;
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

	/** To set a break point (or a set of them) in the code. \n
		Command line: SETBREAK ADDRESS1 (ADDRESS2 ...) */
	class SetBreakPointCommand final : public Command
	{
		public:
		static const int _ID = 16;
		static const std::string _NAME;

		SetBreakPointCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () > 0); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To remove a break point (or a set of them) in the code. \n
		If the solicited address hadn't have any break point defined, nothing will happen. \n
		Command line: REMOVEBREAK ADDRESS1 (ADDRESS2 ...) */
	class RemoveBreakPointCommand final : public Command
	{
		public:
		static const int _ID = 17;
		static const std::string _NAME;

		RemoveBreakPointCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () > 0); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To remove all break points. \n
		Command line: REMOVEBREAKS */
	class RemoveAllBreakPointsCommand final : public Command
	{
		public:
		static const int _ID = 18;
		static const std::string _NAME;

		RemoveAllBreakPointsCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To know the speed of the processor. \n
		Command line: SPEED */
	class CPUSpeedCommand final : public Command
	{
		public:
		static const int _ID = 19;
		static const std::string _NAME;

		CPUSpeedCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To load a binary file. \n
		Command line: LOADBIN PRG ADDRESS*/
	class LoadBinCommand final : public Command
	{
		public:
		static const int _ID = 20;
		static const std::string _NAME;

		LoadBinCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 2); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** To save a binary file. \n
		Command line: SAVEBIN PRG SIZE ADDRESS. */
	class SaveBinCommand final : public Command
	{
		public:
		static const int _ID = 21;
		static const std::string _NAME;

		SaveBinCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 3); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** A very simple command to move the parameters into the answer 
		without doing anything else. */
	class MoveParametersToAnswerCommand final : public Command
	{
		public:
		static const int _ID = 22;
		static const std::string _NAME;

		MoveParametersToAnswerCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** Comand to activate the deep debug. \n
		The parameter needed is the name of the file where to keep in the info. */
	class ActivateDeepDebugCommand final : public Command
	{
		public:
		static const int _ID = 23;
		static const std::string _NAME;

		ActivateDeepDebugCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 1); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** Comand to desactivate the deep debug. \n
		No parameters are needed. */
	class DesactivateDeepDebugCommand final : public Command
	{
		public:
		static const int _ID = 23;
		static const std::string _NAME;

		DesactivateDeepDebugCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** Comand to restart the computer. \n
		No parameters are needed. */
	class RestartComputerCommand final : public Command
	{
		public:
		static const int _ID = 24;
		static const std::string _NAME;

		RestartComputerCommand ()
			: Command (_ID, _NAME)
							{ }

		/** It could have an optional parameter with the "level" of the restart. \n
			By default it will be 0. */
		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0 || _parameters.size () == 1); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** Comand to list infomation about the io devices connected to the computer. \n
		Never get wrong the io devices with the peripherals connected to each, that other command is used. \n
		No parameters are needed. */
	class IODevicesCommand final : public Command
	{
		public:
		static const int _ID = 25;
		static const std::string _NAME;

		IODevicesCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** Command to know the peripehrals connected to the computer. \n
		The command allows to know the peripherals connected to a specific io device or all of them. \n
		So the commnand gets 0 parameters (all peripherals) or 1 with the name of the io device which
		peripherals is nneded to know. */
	class PeripheralsCommand final : public Command
	{
		public:
		static const int _ID = 26;
		static const std::string _NAME;

		PeripheralsCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 0 || _parameters.size () == 1); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** Command to send an instruction to one peripheral. \n
		The instruction / command has to be understood by the peripheral. \n
		The command needs minimum two parameters: The id of the peripheral and the number of the command to execute. \n
		But also additional parameters can be added. */
	class PeripheralInstructionCommand final : public Command
	{
		public:
		static const int _ID = 27;
		static const std::string _NAME;

		PeripheralInstructionCommand ()
			: Command (_ID, _NAME)
							{ }

		virtual bool canBeExecuted () const override
							{ return (_parameters.size () >= 2); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};

	/** Command to asign a number to a joystick. \n
		E.g. When there is only one joystick the "system" assign it the number 0, but
		we might want it to act as joystick number 1 so an assigment from 0 to 1 is needed. */
	class AssignJoystickNameCommand final : public Command
	{
		public:
		static const int _ID = 28;
		static const std::string _NAME;

		AssignJoystickNameCommand ()
			: Command (_ID, _NAME)
							{ }

		/** The original josyctick number and the wanted number. */
		virtual bool canBeExecuted () const override
							{ return (_parameters.size () == 2); }

		private:
		virtual void executeImpl (CommandExecuter* cE, Computer* c, InfoStructure& rst) override;
	};
}

#endif

// End of the file
/*@}*/
