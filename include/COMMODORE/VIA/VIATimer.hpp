/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VIATimer.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 05/01/2023 \n
 *	Description: The "Timer" of the VIA Chip (@see VIA for better understanding).
 *				 @see https://web.archive.org/web/20160108173129if_/http://archive.6502.org/datasheets/mos_6522_preliminary_nov_1977.pdf
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_VIATIMER__
#define __COMMODORE_VIATIMER__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	class VIA;

	/** Every "Timer" always count down, 
		and it is defined basically by an id, the way it runs (what to do once it reaches 0) 
		and the input signal to count down.*/
	class VIATimer final : public MCHEmul::InfoClass
	{
		public:
		friend VIA;

		/** What to do when reaching 0? */
		enum class RunMode
		{
			_ONESHOOT			= 0,	// One single count - down. 
										// INT flag on when reaches 0. 
										// No "reflaction" of the status in the bit 7 at portB (@see VIA Chip).
			_CONTINUOUS			= 1,	// Starts back once 0 is reached.
										// INT flag on.
										// No "reflection" in the bit 7 at portB.
			_ONESHOOTSIGNAL		= 2,	// Idem that _ONESHOOT but "reflecting" the situation in the bit at portB.
			_CONTINUOUSSIGNAL	= 3		// Idem than _CONTINUOUS but "reflecting" at portB the situation (bit 7).
		};                        

		/** Which signal flags a count - down? */
		enum class CountMode
		{
			_PROCESSORCYCLES	= 0,	// With every clock cycle.
			_PULSERECEIVED		= 1		// Every time bit 6 at portB changes its status.
		};

		VIATimer (int id /** unique in the CIA chip. */, unsigned int iID)
			: MCHEmul::InfoClass ("VIATimer"),
			  _id (id),
			  _interruptId (iID),
			  _PB6Situation (false),
			  _interruptRequested (false)
							{ initialize (); /** To initialize the rest of the values. */ }

		int id () const
							{ return (_id); }

		RunMode runMode () const
							{ return (_runMode); }
		void setRunMode (RunMode rm)
							{ _runMode = rm; }
		CountMode countMode () const
							{ return (_countMode); }
		void setCountMode (CountMode cm)
							{ _countMode = cm; }

		/** To point whether the timer reached 0. \n
			When true, this variable will be true just for 1 cycle. */
		bool reaches0 () const
							{ return (_reaches0); }
		/** To point whether the LSB part of the counter reached 0.
			When true, this variable will be true just for 1 cycle. */
		bool reaches0LSB () const
							{ return (_reaches0LSB); }
		/** To point whether the timer reaches half of its initial value. \n
			When true, this variable will be true just for 1 cycle. */
		bool reachesHalf () const
							{ return (_reachesHalf); }

		// Managing interrupt related data...
		bool interruptEnabled () const
							{ return (_interruptEnabled); }
		void setInterruptEnabled (bool e)
							{ _interruptEnabled = e; }
		bool interruptRequested () const // Bear in mind that this variable becomes false, after executing the method.
							{ return (_interruptRequested); }
		bool peekInterruptRequested () const
							{ return (_interruptRequested.peekValue ()); }
		bool launchInterrupt () const
							{ return (_interruptRequested.peekValue () && _interruptEnabled); }

		/** The situation of the PB6 is as an "entry" value to count down. */
		void setPB6Situation (bool p)
							{ _PB6Situation = true; }

		/** In some modes (@see above), when the timer reaches0 = true, 
			a signal has to be transmited to the bit 7 at portB. \n
			This variable let's know whether this is the situation. 
			The PB7 is an "exit" signal. */
		bool PB7Situation () const
						{ return (_PB7Situation); }

		// Managing the values of the timer...
		unsigned short initialValue () const
							{ return (_initialValue); }
		void setInitialValue (unsigned short iV)
							{ _initialValue = iV; _currentValue = _initialValue; }
		unsigned short currentValue () const
							{ return (_currentValue); }
		/** The timer is "forced" to start back. */
		inline void reset ();

		virtual void initialize ();

		/** To simulate the behaviour of the timer. \n
			It invokes also some private methods. \n
			Returns true when everything was ok, and false in other circunstance. */
		bool simulate (MCHEmul::CPU* cpu);

		/**
		  *	The name of the fields are: \n
		  * The ones comming from the parent class and: \n
		  * RUN				= Attribute: Run mode of the timer. \n
		  *	COUNT			= Attribute: Count mode of the timer. \n
		  * STATUS			= Attribute: YES (1) if active and NO (0) in any other case. \n
		  *	IRQ				= Attribute: YES (1) when IRQ are enabled and NO in other cas.
		  *	VALUE			= Attribute: Current value of the timer.
		  *	INITIALVALUE	= Attribute: Initial value of the timer.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		// Managing the timer...
		/** 
		  * To count down to 0. \n
		  * The method will return true when it reaches 0. \n
		  * The way of counting down will depend on the counting mode,
		  * If the timer reaches 0, the varible _reaches0 will be set to true. \n
		  *	When this variable is read, then becomes false back.
		  */
		bool countDown (MCHEmul::CPU* cpu);

		private:
		const int _id;
		const unsigned int _interruptId;

		// The variables that define a timer...
		RunMode _runMode;
		CountMode _countMode;
		bool _interruptEnabled;
		unsigned short _initialValue;

		// Implementation
		/** To indicate whether the timer is or not counting down. */
		bool _counting;
		/** The value the timer has per loop. */
		unsigned short _currentValue; 
		/** The value of the clock cycles last time the timer counted down. */
		unsigned int _lastClockCycles;
		
		/** When 0 is reached, 
			this variable becomes true, just for the time the timer starts back if any. */
		bool _reaches0;
		/** The same with this varible when LSB reaches 0. */
		bool _reaches0LSB;
		/** True when it is the first time 0 is reached. */
		bool _firstTimeReaches0;
		/** When half of the value is reached. */
		bool _reachesHalf;
		/** If already reached half... */
		bool _alreadyReachedHalf;

		/** What would the status of the bit 7 at 
			portB be after "simulate" the timer? */
		mutable bool _PB7Situation;
		/** Which is the situation of the bit 6 at portB? */
		MCHEmul::OBool _PB6Situation;

		/** When an interrupt is requested this variable is set to true, until it is read. */
		MCHEmul::OBool _interruptRequested;
	};

	// ---
	inline void VIATimer::reset ()
	{ 
		_currentValue = _initialValue; 
		
		_alreadyReachedHalf = false; 
							  
		_firstTimeReaches0 = false;

		_PB7Situation = false;

		_PB6Situation = false;
		
		_counting = true; 
	}
}

#endif
  
// End of the file
/*@}*/
