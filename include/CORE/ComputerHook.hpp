/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: ComputerHook.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 28/06/2024 \n
 *	Description: When a computer cycle is executed,
 *				 some considerations around might be taken into account and then some actions executed as a consequence. \n
 *				 e.g. Imagin that we want to print out a message (trace/debug e.g.) each time 
 *				 the CPU access a certain group of memory position... \n
 *				 This is what a ComputerHook is for!
 *				 We could even say that a ComputerHook is a kind of breakpoint but more generic. \n
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_COMPUTERHOOK__
#define __MCHEMUL_COMPUTERHOOK__

#include <CORE/global.hpp>
#include <CORE/NotifyObserver.hpp>
#include <CORE/InfoClass.hpp>
#include <CORE/ComputerAction.hpp>
#include <CORE/Address.hpp>

namespace MCHEmul
{
	/** A ComputerHook might be linked to a one ComputerAction (@see ComputerAction) and only one. 
		But this link is not mandatory at all. */
	class ComputerHook : public Notifier, public Observer, public InfoClass
	{
		public:
		/** The constructor receives the computer action to be executed when a condition is reached. \n
			The condition is something that must be calculated from the information relying in a computer object. \n
			Every hook is also defined by an id. \n
			IMPORTANT: The hook is never the owner of the ComputerAction, so it can reuse what creates in other places. */
		ComputerHook (unsigned int id, int type, ComputerAction* cA = nullptr);

			/** This constructor is similar to the previous one but receiving the id of the action to take.\n
			Sometimes the hook could execute a internal action defined in the computer and the id is requiered. */
		ComputerHook (unsigned int id, int type, unsigned int cAId );

		/** Just in case. \n
			By default it does nothing. */
		virtual ~ComputerHook ()
							{ }

		// Accesing members...
		/** To get the id. */
		unsigned int id () const
							{ return (_id); }
		/** To get the type. */
		int type () const
							{ return (_type); }
		/** Receives a reference to the Computer and returns true when the condition is reached
			and false when it is not.  Must be overloaded. */
		virtual bool conditionReached (Computer* cpu) = 0;
		/** Gets the computer action (if any) associated to the hook. */
		const ComputerAction* action () const
							{ return (_action); }
		ComputerAction* action ()
							{ return (_action); }
		/** THe action by id. */
		unsigned int actionId () const
							{ return (_actionId); }

		/** Verifies the condition an execute the action if reached and the condition is valid. \n
			It returns the result of the action if executed (can be true or false), and false in other case. 
			The method can be overloaded, by cases (e.g.) no ComputerAction is directly assigned. \n
			The priority is first _action and then _actionId. */
		virtual bool verifyAndExecuteAction (Computer* c);

		/** To print out info of the hook if needed (for debugging reasons mainly). */
		virtual std::string asString () const = 0;

		friend std::ostream& operator << (std::ostream& o, const ComputerHook& h)
							{ return (o << h.asString ()); }

		virtual InfoStructure getInfoStructure () const override;

		protected:
		unsigned int _id;
		int _type;
		/** Two possibilities to define what to execute. */
		ComputerAction* _action;
		unsigned int _actionId;
	};

	/** To simplify a reference to a list of hooks.
		An object of this type is defined in the Computer class. */
	using ComputerHooks = std::vector <ComputerHook*>;
	/** A map of them using the id as index. */
	using ComputerHooksMap = std::map <unsigned int, ComputerHook*>;

	/** Defines a computer hook for a specific zone of the memory 
		where the CPU is accessing in a particular cycle. */
	class ComputerMemoryAccessHook final : public ComputerHook
	{
		public:
		static const int _TYPE = 0;
		static const Strings _INFO;

		ComputerMemoryAccessHook 
				(unsigned int id, const Address& start, const Address& end, 
					ComputerAction* cA = nullptr);

		ComputerMemoryAccessHook
				(unsigned int id, const Address& start, const Address& end,
					unsigned int cAId);
		
		/** The condition is reached when the PC of the CPU is in the defined zone. */
		virtual bool conditionReached (Computer* c) override;
	
		/** To print out info of the hook if needed (Debugging). */
		virtual std::string asString () const override;

		private:
		/** The addresses with the limit. \n
			End has to be bigger or equal than start. */
		Address _start, _end;

		// Implementation
		/** The last address reached by the hook, 
			unless the address verified by the condition is different when previously reached,
			the condition is validated as false. */
		Address _lastAddress;
	};

	/** Sometimes the hooks can be grouped in pools. \n
		The pool controls every individual hook, and creates new hooks. \n
		Every simulation and computer can need their own hooks, 
		so this class must be overloaded. \n
		The pool is the owner of the hooks it creates. */
	class ComputerHooksPool : public InfoClass
	{
		public:
		ComputerHooksPool ();

		virtual ~ComputerHooksPool ();

		// Managing the hooks within the pool
		/** To verify whether a hook exists in the pool in advance. */
		bool existsHook (unsigned int id) const
							{ return (_hooks.find (id) != _hooks.end ()); }
		/** The method to invoke to get a hook. \n
			If a hook with the same id already existed it would be returned,
			whatever the rest of the parameters were!. \n
			In ither case the method createHook will be invoked using 
			the parameters received. \n
			Notice that to objects are returned: The hook requested
			and an error. If hook == nullptr, the error will have sense, otherwise wont). */
		std::tuple <ComputerHook*, std::string> 
			hook (unsigned int id, int t, const Strings& strs) const;
		/** To remove the hook from the pool. \n
			The hook requested, if existed, is also deleted. \n
			NOTE: Take care using this method!:
			If the hook is being used in the computer (e.g.), would become invalid
			and will generate a crash in the main loop of the computer. \n
			If the hook didn't exist, nothing will happen. */
		void removeHook (unsigned int id);

		/** This method returns the list of the possible hooks to be created with information about. 
			It can be overloaded if new hooks could be created. */
		virtual Strings help () const;

		virtual InfoStructure getInfoStructure () const override;

		protected:
		/** The function to create a new hook. \n
			Just to create it. This function is very internal. \n
			The only ones created by default are the ones deines above. */
		virtual std::tuple <ComputerHook*, std::string> 
			createHook (unsigned int id, int t, const Strings& prms) const;

		protected:
		mutable ComputerHooksMap _hooks;
	};
}

#endif
  
// End of the file
/*@}*/

