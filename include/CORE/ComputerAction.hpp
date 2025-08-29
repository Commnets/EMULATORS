/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: ComputerAction.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 26/08/2025 \n
 *	Description: When a computer is running code at a certain position (PC) or 
 *				 a hook condition is reached, certain actions might be executed. \n
 *				 This is what a ComputerAction is for! \n
 *				 The actions can be almost be anything, 
 *				 but usually they will be related to the execution flow of the computer. \n
 *	Versions: 1.0 Initial
 */

#ifndef __MCHMUL_COMPUTERACTION__
#define __MCHMUL_COMPUTERACTION__

#include <vector>
#include <map>

namespace MCHEmul
{
	class Computer;

	/** The computer might execute different actions when reaching a specific position,
		or when a hook condition is detected. */
	class ComputerAction
	{
		public:
		// No default constructors are needed...
		ComputerAction (unsigned int id)
			: _id (id)
						{ }

		virtual ~ComputerAction () 
						{ /** Just in case an extension is needed. */ }

		/** Get the id. */
		unsigned int id () const
						{ return (_id); }

		/** Returns true when the next CPU cycle has to be executed after this action,
			and false in any other circusntance. */
		virtual bool execute (Computer*) = 0;

		protected:
		unsigned int _id;
	};

	/** To manage a set of them and simplify the code. */
	using ComputerActions = std::vector <ComputerAction*>;
	/** TO manage a map of actions. */
	using ComputerActionsMap = std::map <unsigned int, ComputerAction*>;

	/** An action made up of a set of actions. */
	class CompositeComputerAction : public ComputerAction
	{
		public:
		CompositeComputerAction (unsigned int id, const ComputerActions& a)
			: ComputerAction (id),
				_actions (a)
						{ }

		/** It will return true when all elements return true, 
			and false in other circunstance. */
		virtual bool execute (Computer* c) override;

		protected:
		ComputerActions _actions;
	};
}

#endif
  
// End of the file
/*@}*/
