/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: Command.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 04/08/2022 \n
 *	Description: Representing any command that can be sent to the emulation of a computer.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_COMMAND__
#define __MCHEMUL_COMMAND__

#include <CORE/global.hpp>
#include <CORE/InfoStructure.hpp>

namespace MCHEmul
{
	class Computer;

	/** A command is and instruction to be executed over the computer.
		Any command is made of an id and a set of attributes. */
	class Command 
	{
		public:
		Command () = delete;

		Command (int id, const std::string& n, const Attributes& prms = { })
			: _id (id), _name (n), _parameters (prms)
							{ }

		Command (const Command&) = default;

		Command& operator = (const Command&) = default;

		virtual ~Command ()
							{ }

		int id () const
							{ return (_id); }
		const std::string& name () const 
							{ return (_name); }
		const Attributes& parameters () const
							{ return (_parameters); }
		void setParameters (const Attributes& prms)
							{ _parameters = prms; }

		// Manage the execution...
		/** To determine whether that are part of the commnand are or not valid to 
			execute the command. The method return true if they are, and false in other case. */ 
		virtual bool canBeExecuted () const = 0;
		/** To execute the command. \n
			It returns true when the command has been executed properly and false when not. */
		virtual bool execute (Computer* c, InfoStructure& rst);

		protected:
		/** This method executes the command over the computer received. \n
			This methid is inoked from "execute" after verified that it is possible. 
			This is ther eal one to be overloaded. */
		virtual void executeImpl (Computer* c, InfoStructure& rst) = 0;

		protected:
		int _id;
		std::string _name;
		Attributes _parameters;
	};

	using Commands = std::vector <Command*>;

	/** A command made up of others. */
	class ComplexCommand : public Command
	{
		public:
		ComplexCommand (int id, const std::string& n, const Commands& cmds)
			: Command (id, n), _commands (cmds)
							{ }

		~ComplexCommand ()
							{ for (auto i : _commands) delete (i); }

		/** By default a complex command could be executed just when all commans it is made up of can also be executed. */
		virtual bool canBeExecuted () const override;
		/** By default execute all commands inside and returns true if all also returns true, and false in other case. */
		virtual bool execute (Computer* c, InfoStructure& rst) override;

		private:
		Commands _commands;
	};
}

#endif
  
// End of the file
/*@}*/
