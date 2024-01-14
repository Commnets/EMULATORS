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
	class VIAPort;

	/** Every "Timer" always count down, 
		and it is defined basically by an id, the way it runs (what to do once it reaches 0) 
		and the input signal used to count down.*/
	class VIATimer final : public MCHEmul::InfoClass
	{
		public:
		friend VIA;

		/** What to do when reaching 0? */
		enum class RunMode
		{
			_ONESHOOT			= 0,	// One single count - down. 
										// Interrupt flag sets when reaches 0, just once.
										// No "reflection" of the status in the bit 7 at the port linked (@see VIAPort).
			_CONTINUOUS			= 1,	// Starts back once 0 is reached.
										// Interrupt flag sets everytime it runs out.
										// No "reflection" in the bit 7 at port linked.
			_ONESHOOTSIGNAL		= 2,	// Idem that _ONESHOOT but "reflecting" the situation in the bit at the port linked.
			_CONTINUOUSSIGNAL	= 3		// Idem than _CONTINUOUS but "reflecting" at the port linked the situation (bit 7).
		};                        

		/** Which signal flags a count - down? */
		enum class CountMode
		{
			_PROCESSORCYCLES	= 0,	// With every clock cycle.
			_PULSERECEIVED		= 1		// Every time bit 6 at port linked changes its status.
		};

		VIATimer (int id);

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

		/** To point whether the "Timer" reached 0. \n
			When true, this variable will be true just for 1 cycle. */
		bool reaches0 () const
							{ return (_reaches0); }
		/** To point whether the LSB part of the counter reached 0.
			When true, this variable will be true just for 1 cycle. */
		bool reaches0LSB () const
							{ return (_reaches0LSB); }
		/** To point whether the "Timer" reaches half of its initial value. \n
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

		// Managing the values of the "Timer"...
		unsigned short initialValue () const
							{ return (_initialValue); }
		void setInitialValue (unsigned short iV)
							{ _initialValue = iV; _currentValue = _initialValue; }
		unsigned short currentValue () const
							{ return (_currentValue); }
		/** The "Timer" is "forced" to start back. */
		void reset ();

		void initialize ();

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
		void lookAtPort (VIAPort* p)
							{ _P = p; }

		private:
		// Managing the "Timer"...
		/** 
		  * To count down to 0. \n
		  * The method will return true when it reaches 0. \n
		  * The way of counting down will depend on the counting mode,
		  * If the "Timer" reaches 0, the varible _reaches0 will be set to true. \n
		  *	When this variable is read, then becomes false back.
		  */
		bool countDown (MCHEmul::CPU* cpu);

		private:
		const int _id;

		// The variables that define a timer...
		RunMode _runMode;
		CountMode _countMode;
		unsigned short _initialValue;
		bool _interruptEnabled;

		// Elements the "Timer" has relation with...
		VIAPort* _P;

		// Implementation
		/** To indicate whether the "Timer" is or not counting down. */
		bool _counting;
		/** The value the "Timer" has per loop. */
		unsigned short _currentValue; 
		
		/** When 0 is reached, 
			this variable becomes true, just for the time the "Timer" starts back if any. */
		bool _reaches0;
		/** The same with this varible when LSB reaches 0. */
		bool _reaches0LSB;
		/** True when it is the first time 0 is reached. */
		bool _firstTimeReaches0;
		/** When half of the value is reached. */
		bool _reachesHalf;
		/** If already reached half... */
		bool _alreadyReachedHalf;

		/** When an interrupt is requested this variable is set to true, until it is read. */
		MCHEmul::OBool _interruptRequested;
	};
}

#endif
  
// End of the file
/*@}*/
