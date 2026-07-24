/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: CIATimer.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 05/08/2021 \n
 *	Description: CIA Chip manages Timers.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_CIATIMER__
#define __COMMODORE_CIATIMER__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	class CIATimer final : public MCHEmul::InfoClass
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


		/** To determine how the timer output is reflected at PB6 or PB7. */
		enum class PortOutputMode
		{
			_PULSE = 0,
			_TOGGLE
		};

		/** Results generated while the timer simulates a single clock cycle. */
		struct CycleResult final
		{
			CycleResult ()
				: _underflow (false),
				  _portOutputChanged (false),
				  _portOutput (false)
							{ }

			bool _underflow;
			bool _portOutputChanged;
			bool _portOutput;
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
			: MCHEmul::InfoClass ("CIATimer"),
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

		// What to do with the data port
		/** When the timer reaches 0 it can or not affect the port data. */
		bool affectPortDataB () const
							{ return (_affectPortDataB); }
		void setAffectPortDataB (bool a)
							{ _affectPortDataB = a; }
		PortOutputMode portOutputMode () const
							{ return (_portOutputMode); }
		void setPortOutputMode (PortOutputMode m)
							{ _portOutputMode = m; }
		bool portOutput () const
							{ return (_portOutput); }

		// Managing the status...
		bool enabled () const
							{ return (_enabled); }
		void setEnabled (bool e);

		/** To define whether Timer related interruptions are allowed. */
		bool interruptEnabled () const
							{ return (_interruptEnabled); }
		void setInterruptEnabled (bool e)
							{ _interruptEnabled = e; }

		/** To know whether the interrupt condition for the Timer has or not been reached. \n
			When this variable is read, the value comes back to false. */
		bool interruptRequested () const
							{ bool r = _interruptRequested; _interruptRequested = false; return (r); }
		/** Same than the previous method but without modifying the value. */
		bool peekInterruptRequested () const
							{ return (_interruptRequested); }

		/** To know whether an interruption related with the Timer should be launched. \n
			That happen when there is a interruption requested and also they are allowed. */
		bool launchInterruption () const
							{ return (_interruptRequested && _interruptEnabled); }

		// Managing the values...
		/** The value used as the starting point for the count down. */
		unsigned short initialValue () const
							{ return (_initialValue); }
		void setLatchLow (const MCHEmul::UByte& v);
		void setLatchHigh (const MCHEmul::UByte& v);
		/** The current value of the timer. */
		unsigned short currentValue () const
							{ return (_currentValue); }
		/** The timer is forced to start back. */
		void forceLoad ()
							{ _currentValue = _initialValue; }

		/** To simulate the behaviour of the timer. \n
			It invokes also some private methods. */
		CycleResult simulate (bool processorCycle, bool CNTRisingEdge,
			bool linkedTimerUnderflow, bool CNTHigh);

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
		bool hasToCount (bool processorCycle, bool CNTRisingEdge,
			bool linkedTimerUnderflow, bool CNTHigh) const;
		void triggerUnderflow (CycleResult& result);

		private:
		const int _id;
		const unsigned int _interruptId;

		// The variables that define a timer...
		RunMode _runMode;
		CountMode _countMode;
		bool _affectPortDataB;
		PortOutputMode _portOutputMode;
		bool _enabled;
		bool _interruptEnabled;
		unsigned short _initialValue;

		// Implementation
		/** The value the timer has per loop. */
		unsigned short _currentValue;
		bool _portOutput;
		bool _pulseOutputActive;
		/** When an IRQ interruption is requested this variable is set to true, until it is read. */
		mutable bool _interruptRequested;
	};
}

#endif
  
// End of the file
/*@}*/
