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
	class Computer;

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
		void setInExecution (bool i);

		/** The defult intialization of the interrupt leaves it inactive. */
		virtual void initialize ()
							{ _active = false; _lastClockCyclesExecuted = 0; }

		/** Receive the CPU the interrupts works for. \n
			It receives also a reference to a variable where to load the number of cycles that
			the execution tooks (when the return was ok), both total and reading cycles. \n
			It returns true if ok and false if not. */
		bool executeOver (CPU* c, unsigned int& nC, unsigned int& nCR);

		/**
		  *	The name of the fields are: \n
		  *	ID				= Id of the interrupt.
		  *	EXECUTION		= Attribute with YES when the interrupt is in execution and NO in other case.
		  */
		virtual InfoStructure getInfoStructure () const override;

		// To manage the debug for interruptions...
		static bool debug ()
							{ return (_debug); }
		/** To activate or desactivate the debug of the code for the interruptions (all). \n
			The debug for interruptions is incompatible with the deepDebug.
			When that one is activated this other one can not. \n 
			Returns true whether the action was possible. 
			Receives a reference to the environment (the computer), the name of the file where to trace,
			and also whether the info has to be added to that file or is new. */
		static bool activateDebug (Computer* c, const std::string& nF = ".\\INT.log", bool a = true /** meaning add info at the end. */);
		/** Same to desactivate. */
		static bool desactivateDebug (Computer* c);

		protected:
		// These methods are invoked by executeOver (defined above);
		/** To determine whether it is the time to execute the interruption. */
		virtual bool isTime (CPU* c) const = 0;
		/** To really execute the interrupt. \n 
			This must be overloaded by the real interrupt. \n
			Returns when everything ok. */
		virtual bool executeOverImpl (CPU* c, unsigned int& nC, unsigned int& nCR) = 0;

		protected:
		int _id;
		bool _active;
		bool _inExecution;
		
		// To debug the code in the interruption...
		static bool _debug;
		static std::string _debugFileName;
		static bool _addDebugInfo;
		static Computer* _computer;

		// Implementation
		mutable unsigned int _lastClockCyclesExecuted;
		static bool _debugOffWhenFinishes;

	};

	/** A map of interrupts. */
	using CPUInterrupts = std::map <int, CPUInterrupt*>;
}

#endif
  
// End of the file
/*@}*/


