/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: CPUInterrupt.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
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
	class Chip;
	class Computer;

	/** A CPU Interrupt is something that is able to stop the normal progress of the CPU execution. \n
		The different Interrupts of a system can have different levels of prority.
		By default, all have the same and equal 0. The smaller number the lower priority. */
	class CPUInterrupt : public InfoClass
	{
		public:
		// Different initial status of the interrupt
		// Many more can be added as the class is extended...
		static const unsigned int _EXECUTIONNOTALLOWED = 0;
		static const unsigned int _EXECUTIONTOWAIT = 1;
		static const unsigned int _EXECUTIONALLOWED = 2;

		CPUInterrupt () = delete;

		CPUInterrupt (int id, unsigned int cL, int pr = 0)
			: InfoClass ("Interrupt"),
			  _id (id),
			  _cyclesToLaunch (cL),
			  _priority (pr),
			  _cyclesAfterLaunch (0), // Usually set when executed, if needed!
			  _active (true /** by default. */),
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

		int priority () const
							{ return (_priority); }

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
							{ _lastClockCyclesExecuted = 0; } // Active or not is manage donly externally!

		// Managing the status of the enter line...
		/** Before any interrupt is admitted to the queue of the pending ones,
			it is needed to check whether this is possible. @see CPU::requestInterrupt. \n
			It could be said that this variable is aligned the meaning of how the CPU 
			considers the status of the interrupt PIN
			before considering to execute or not (admitting) a interrupt. \n
			By default a new interrupt request is always admitted (considered). \n
			However this behavior can be changed. */
		virtual bool admitNewInterruptRequest () const
							{ return (true); }
		virtual void setNewInterruptRequestAdmitted (bool)
							{ }

		// Managin the interrupt itself...
		/** To define whether the interrupt can or not be executed. 
			It might be overloaded later, but using the internal method isTime (), 
			that is invoked from this one. \n
			The method receives a reference to the CPU where it has to be executed, 
			and the CPU cycles when the interrupt happened. */
		inline unsigned int canBeExecutedOver (CPU* c, unsigned int cC);
		/** Receive the CPU the interrupts works for,
			and the number of cycles when the execution takes place. */
		bool executeOver (CPU* c, unsigned int cC);

		/** To know the cycles that the interrupt takes. 
			First variable is usually know before execution and the other, after execution. */
		unsigned int cyclesToLaunch () const
							{ return (_cyclesToLaunch); }
		unsigned int cycledAfterLaunch () const
							{ return (_cyclesAfterLaunch); }

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
		// These methods are invoked by canBeExecutedOver and executeOver (both defined above);
		/** To determine whether it is the time to execute the interruption. \n
			The method returns a possibility. See the reasons above although more can be defined. */
		virtual unsigned int isTime (CPU* c, unsigned int cC) const = 0;
		/** To really execute the interrupt. \n 
			This must be overloaded by the real interrupt. \n
			Returns when everything ok. */
		virtual bool executeOverImpl (CPU* c, unsigned int cC) = 0;

		private:
		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugStartingToDebugInterrupt ();
		void debugInterruptToStart (CPU* c);
		// -----

		protected:
		int _id;
		int _priority;
		unsigned int _cyclesToLaunch;
		unsigned int _cyclesAfterLaunch; // Rare but, used in some cases...
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

	// ---
	inline unsigned int CPUInterrupt::canBeExecutedOver (CPU* c, unsigned int cC)
	{ 
		assert (c != nullptr);
						
		return (!active () ? MCHEmul::CPUInterrupt::_EXECUTIONNOTALLOWED : isTime (c, cC)); 
	}

	/** A map of interrupts. */
	using CPUInterrupts = std::map <int, CPUInterrupt*>;
	/** A list of instructions. */
	using CPUListOfInterrupts = std::vector <CPUInterrupt*>;

	/** When a is requested from any point in the system, 
		this class must be used. \n
		Every request has an id that is assigned automatically every time a new object is created. */
	class CPUInterruptRequest final
	{
		public:
		/** This static const member is to refer to a "no" interrupt request. 
			It is usually used when the queue of pendings-requests is empty. */
		static const CPUInterruptRequest _NOINTREQUEST;

		CPUInterruptRequest ()
			: _number (_COUNTER++),
			  _type (-1) /** like _NOINTREQUEST. */, _cycles (0), _from (nullptr), _reason (0)
							{ }

		CPUInterruptRequest (int t, unsigned int c, Chip* f, int r)
			: _number (_COUNTER++),
			  _type (t), _cycles (c), _from (f), _reason (r)
							{ }

		unsigned long number () const
							{ return (_number); }
		int type () const
							{ return (_type); }
		unsigned int cycles () const
							{ return (_cycles); }
		const Chip* from () const
							{ return (_from); }
		Chip* from ()
							{ return (_from); }
		int reason () const
							{ return (_reason); }

		std::string toString () const;

		friend std::ostream& operator << (std::ostream& o, const CPUInterruptRequest& iR)
						{ return (o << iR.toString ()); }

		private:
		unsigned long _number; // Asigned automatically...
		/** Type. */
		int _type;
		/** The machine cycles (if possible to know) at the moment the interrupt was requested. */
		unsigned int _cycles;
		/** Who is launching the request? */
		Chip* _from;
		/** Reason to invoke it. */
		int _reason;

		// Implementation
		static unsigned long _COUNTER;
	};

	// To simplify the use of a list of requests...
	using CPUInterruptRequests = std::vector <CPUInterruptRequest>;

	/** This class is to manage how the interrupts are or not accepted by the CPU,
		and it is made up initially of a list of all possible type of Interrupts defined for a processor. */
	class CPUInterruptSystem : public InfoClass
	{
		public:
		CPUInterruptSystem (const CPUInterrupts& irr);

		~CPUInterruptSystem ();

		// Managing interrupts...
		/** Adding, removing or getting interrupts. */
		const CPUInterrupts& interrupts () const
							{ return (_interrupts); }
		bool existsInterrupt (int id) const
							{ return (_interrupts.find (id) != _interrupts.end ()); }
		const CPUInterrupt* interrupt (int id) const
							{ return ((*_interrupts.find (id)).second); }
		CPUInterrupt* interrupt (int id)
							{ return ((*_interrupts.find (id)).second); }
		void addInterrupt (CPUInterrupt* in);
		void removeInterrrupt (int id);

		// Managing the requests of interrutps...
		/** To request an interrupt. \n
			It receives the id of the interrupt requested, the clockCycle status when it happened,
			the sender (optional), and a code indicating the reason (also optional, -1 = not defined). */
		void requestInterrupt (int id, unsigned int nC, Chip* src = nullptr, int cR = -1)
							{ requestInterrupt (CPUInterruptRequest ({ id, nC, src, cR })); }
		/** Important method: \n
			Same than previous, but receiving a CPUInterruptRequest as parameter. \n
			This is the one that can be overloaded. \n
			To accept or not a interrupt to be processed will actually depend on many aspect but mainly on the type of CPU (@see CPU) */
		virtual bool requestInterrupt (const CPUInterruptRequest& iR) = 0;
		/** To know whether the interrupt requests pending. \n
			Empty list if none. */
		const CPUInterruptRequests& interruptsRequested () const
							{ return (_interruptsRequested); }
		/** Important method:\n
			To get the next interrupt in the list of those to be processed. \n 
			The way the interrupt is choosen can be changed also by the user. */
		virtual const CPUInterruptRequest& getNextInterruptRequest () const = 0;
		inline const CPUInterrupt* getNextInterrupt () const;
		CPUInterrupt* getNextInterrupt ()
							{ return (const_cast <CPUInterrupt*> 
								((const_cast <const CPUInterruptSystem*> (this)) -> getNextInterrupt ())); }
		// Be carefull with the two following method. No boundary check is done
		// so the consequences could be a crash!
		const CPUInterrupt* getInterruptForRequest (const CPUInterruptRequest& iR) const
							{ return (_rowInterrupts [iR.type ()]); }
		CPUInterrupt* getInterruptForRequest (const CPUInterruptRequest& iR)
							{ return (_rowInterrupts [iR.type ()]); }
		/** Once a interrupt is processed it has to be removed from the list of pendings ones. */
		void removeInterruptRequest (const CPUInterruptRequest& iR)
							{ _interruptsRequested.erase (std::find_if (_interruptsRequested.begin (), _interruptsRequested.end (), 
								[&](const CPUInterruptRequest& i) -> bool { return (i.number () == iR.number ()); })); }

		/** Invoked to initialize the system.
			By default the requests pending are cleared. */
		virtual void initialize () 
							{ _interruptsRequested = { }; }

		virtual InfoStructure getInfoStructure () const override;

		protected:
		/** To make a row data with the interruptions. \n
			This method is called from the constructor and instantiate the variable _rowInterrupts,
			and also at any time an interrupt is added or removed. */
		void makeInterruptionRowData ();

		protected:
		CPUInterrupts _interrupts;
		/** When the CPU is executing a transaction, and a interruption is requested,
			the current instruction has to finish first, and the the interruption is invoked. \n
			It the CPU were stopped, first of all the CPU would have to run back 
			and then finish the instruction that could be running when it was stopped. \n
			The parameter has the id of the interrupt requested. -1 means none. */
		CPUInterruptRequests _interruptsRequested;

		// Implementation
		/** Also a row vector with the interruptions. */
		CPUListOfInterrupts _rowInterrupts;
	};

	// ---
	inline const CPUInterrupt* CPUInterruptSystem::getNextInterrupt () const
	{
		const CPUInterruptRequest& iR = getNextInterruptRequest ();
		return ((iR.type () == CPUInterruptRequest::_NOINTREQUEST.type ())
			? nullptr 
			: _rowInterrupts [iR.type ()]);
	}

	/** The standard CPU Interrupt System. \n
		Valid for most of the CPU types implemented in the frameowrk. */
	class StandardCPUInterruptSystem : public CPUInterruptSystem
	{
		public:
		// The name of the class is not changed because there is no more additional 
		// info to show than the one that is shown at parent level
		StandardCPUInterruptSystem (const CPUInterrupts& irr)
			: CPUInterruptSystem (irr)
							{ /** Nothing else... */ }
		
		/** The pending-requests queue admits only one request at the same time with the same priority.
			If the requested one had majot priority it would be added at the top of the pending-requests list. */
		virtual bool requestInterrupt (const CPUInterruptRequest& iR) override;
		/** It returns the first one in the queue. */
		virtual const CPUInterruptRequest& getNextInterruptRequest () const override
							{ return (_interruptsRequested.empty () 
								? CPUInterruptRequest::_NOINTREQUEST : *_interruptsRequested.begin ()); }
	};
}

#endif
  
// End of the file
/*@}*/


