/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: CIATimer.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 05/08/2021 \n
 *	Description: CIA Chip manages Timers.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_CIATIMER__
#define __COMMODORE_CIATIMER__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	class CIATimer : public MCHEmul::InfoClass
	{
		public:
		/** To determine the behaviour of the Timer onces it reaches the 0. */
		enum class RunMode
		{
			_RESTART = 0,			// The timer restarts once it reaches 0
			_ONETIME				// The timer stops when it reaches 0 (becomes not enabled)
		};                        

		/** To determine the signal that moves a timer to count down... */
		enum class CountMode
		{
			// The names are related directly with numbers to simplify later and quicker conversions...
			_PROCESSORCYCLES = 0,		// Processor cycle 
			_SIGNALSONCNTLINE = 1,		// Signal present on the CNT line at pin 4 of User Port 
			_TIMERCOUNTSDOWNTO0 = 2,	// When other Timer reaches 0
			_0ONCNTPULSES = 3			// When other Timer reaches 0 and _SIGNALSONCNTLINE in this timer is also on
			// These two last types of timers helps to generate long time timers...
		};

		/**
		  *	When the timer is created, the value of the internal variables will be as follows:
		  *	RUNMODE = RESTART
		  * COUNTMODE = PROCESSORCYCLES
		  * ENABLED = FALSE
		  * IRQENABLED = FALSE
		  * INITIALVALUE = 0
		  * TIME = 0 (in thenth of second)
		  * The constructor receivesan id to identify it 
		  * and the id of the interrupts to launch when the the max, min values are reached!
		  */
		CIATimer (int id /** unique in the CIA chip. */, unsigned int iID)
			: MCHEmul::InfoClass ("Clock"),
			  _id (id),
			  _interruptId (iID)
							{ initialize (); }

		/** To initialize the timer. By default it is not enabled. */
		virtual void initialize ();

		int id () const
							{ return (_id); }

		// Managing the type...
		RunMode runMode () const
							{ return (_runMode); }
		void setRunMode (RunMode rm)
							{ _runMode = rm; }
		CountMode countMode () const
							{ return (_countMode); }
		void setCountMode (CountMode cm)
							{ _countMode = cm; }

		/** To point whether the timer reached 0. 
			This variable will be true until the timer starts back. */
		bool reaches0 () const
							{ return (_reaches0); }

		// Managing the status...
		bool enabled () const
							{ return (_enabled); }
		void setEnabled (bool e)
							{ _enabled = e; }

		bool InterruptEnabled () const
							{ return (_InterruptEnabled); }
		void setInterruptEnabled (bool e)
							{ _InterruptEnabled = e; }
		bool InterruptRequested () const
							{ bool r = _InterruptRequested; _InterruptRequested = false; return (r); }

		// Managing the values...
		/** The value used as the starting point for the count down. */
		unsigned short initialValue () const
							{ return (_initialValue); }
		void setInitialValue (unsigned short iV)
							{ _initialValue = iV; _currentValue = _initialValue; }
		/** The current value of the timer. */
		unsigned short currentValue () const
							{ return (_currentValue); }
		/** The timer is forced to start back. */
		void reset ()
							{ _currentValue = _initialValue; }

		/** To simulate the behaviour of the timer. \n
			It invokes also some private methods. */
		void simulate (MCHEmul::CPU* cpu, CIATimer* t = nullptr);

		/**
		  *	The name of the fields are: \n
		  * RUN				= Attribute with the run mode of the timer. \n
		  *	COUNT			= Attribute with the count mode of the timer. \n
		  * STATUS			= Attribute with YES if active and NO if other case. \n
		  *	IRQ				= Attribute with YES when IRQ are enabled and NO in other cas.
		  *	VALUE			= Attribute with the current value of the timer.
		  *	INITIALVALUE	= Attribute with the initial value of the timer.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		// Managing the timer...
		/** 
		  * To count down to 0. \n
		  * The method will return true when it reaches 0. \n
		  * The way of counting down will depend on the counting mode,
		  * and according to this, additional info will be needed: The CPU and other timer. 
		  * If the timer reaches 0, the varible _reaches0 will be set to true. \n
		  * If the timer launches an exception, the variable _IRQRequested is set to true. \n
		  *	When this variable is read, then becomes false back.
		  */
		bool countDown (MCHEmul::CPU* cpu, CIATimer* t = nullptr);

		private:
		const int _id = 0;
		const unsigned int _interruptId = 0;

		// The variables that define a timer...
		RunMode _runMode;
		CountMode _countMode;
		bool _enabled;
		bool _InterruptEnabled;
		unsigned short _initialValue;

		// It is actualized continiously through the method actualizeTime...
		// The time counts only hours in the day...
		MCHEmul::Time _time;

		// Implementation
		/** The value the timer has per loop. */
		unsigned short _currentValue; 
		/** The value of the clock cycles las time the timer counted down. */
		unsigned int _lastClockCycles;
		/** When 0 is reached, this variable becomes true, just for the time the timer starts back if any. */
		bool _reaches0;
		/** When an IRQ interruption is requested this variable is set to true, until it is read. */
		mutable bool _InterruptRequested;
	};
}

#endif
  
// End of the file
/*@}*/
