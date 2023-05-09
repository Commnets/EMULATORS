/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: CPUInterrupt.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/04/2021 \n
 *	Description: Defines a generic way to manage interrupts in a CPU.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_CPUINTERRUPT__
#define __MCHEMUL_CPUINTERRUPT__

#include <CORE/global.hpp>
#include <CORE/InfoClass.hpp>

namespace MCHEmul
{
	class CPU;

	/** A CPU Interrupt is something that is able to stop the normal progress of the CPU execution. */
	class CPUInterrupt : public InfoClass
	{
		public:
		CPUInterrupt () = delete;

		CPUInterrupt (int id)
			: InfoClass ("Interrupt"),
			  _id (id), _active (false /** by default. */),
			  _inExecution (false),
			  _lastClockCyclesExecuted (0)
							{ }

		CPUInterrupt (const CPUInterrupt&) = delete;

		CPUInterrupt& operator = (const CPUInterrupt&) = delete;

		virtual ~CPUInterrupt ()
							{ /** Nothing by default. */ }

		CPUInterrupt (CPUInterrupt&&) = delete;

		CPUInterrupt& operator = (CPUInterrupt&&) = delete;

		int id () const
							{ return (_id); }

		/** When the interrupt is active. */
		bool active () const
							{ return (_active); }
		void setActive (bool a)
							{ _active = a; }

		/** When the interrupt in under execution. 
			The switch is set on when start the execution, but the finalization 
			has to been set back off depending on the CPU being implemented. \n
			In com CPU when the interruption start to run can be distinguish from other. */
		bool inExecution () const
							{ return (_inExecution); }
		void setInExecution (bool i)
							{ _inExecution = i; }

		/** The defult intialization of the interrupt leaves it inactive. */
		virtual void initialize ()
							{ _active = false; _lastClockCyclesExecuted = 0; }

		/** Receive the CPU the interrupts works for. \n
			It receives also a reference to a variable where to load the number of cycles it 
			took the execution (when the return was ok). \n
			It returns true if ok and false if not. */
		bool executeOver (CPU* c, unsigned int& nC);

		virtual InfoStructure getInfoStructure () const override;

		protected:
		// These methods are invoked by executeOver (defined above);
		/** To determine whether it is the time to execute the interruption. */
		virtual bool isTime (CPU* c) const = 0;
		/** To really execute the interrupt. \n 
			This must be overloaded by the real interrupt. \n
			Returns when everything ok. */
		virtual bool executeOverImpl (CPU* c, unsigned int& nC) = 0;

		protected:
		int _id;
		bool _active;
		bool _inExecution;

		// Implementation
		mutable unsigned int _lastClockCyclesExecuted;
	};

	/** A map of interrupts. */
	using CPUInterrupts = std::map <int, CPUInterrupt*>;
}

#endif
  
// End of the file
/*@}*/


