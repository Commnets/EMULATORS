/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: TEDTimer.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 10/02/2024 \n
 *	Description: The "Timer" of the TED Chip (@see TED for better understanding).
 *				 It is a simplier version of VIA's one (VICI)
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_TEDTIMER__
#define __COMMODORE_TEDTIMER__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	class TED;

	/** Every "Timer" always count down, 
		and it is defined basically by an id, and a way of running. \n
		The signal used to count down is the ted clock signal. */
	class TEDTimer final : public MCHEmul::InfoClass
	{
		public:

		/** The way of running is defined at construction time. \n
			Can not be changed during the life of the timer. */
		enum class RunMode
		{
			_FROMINITIALVALUE	= 0,		// Meaning that when reaches 0,
											// starts back from the initial value loaded.
			_CONTINUOUS						// Meaning that when reaches 0,
											// starts back from 0xffff.
		};

		friend TED;

		TEDTimer (int id, RunMode rM);

		int id () const
							{ return (_id); }
		RunMode runMode () const
							{ return (_runMode); }

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
		bool peekInterruptRequested () const // To avoid the behavious described avoid...
							{ return (_interruptRequested.peekValue ()); }
		bool launchInterrupt () const
							{ return (_interruptRequested.peekValue () && _interruptEnabled); }

		// Managing the values of the "Timer"...
		unsigned short initialValue () const
							{ return (_initialValue); }
		/** The actual value is also set. \n
			This "initialValue" will be only used when the runMode = _CONTINUOUS. */
		void setInitialValue (unsigned short iV)
							{ _initialValue = iV; _currentValue = _initialValue; }
		unsigned short currentValue () const
							{ return (_currentValue); }
		/** The "Timer" is "forced" to start back, but still stopped. */
		void reset ();

		/** To start/stop the counting process. */
		void start () 
							{ _counting = true; }
		void stop ()
							{ _counting = false; }

		void initialize ();

		/** To simulate the behaviour of the timer. \n
			It invokes also some private methods. \n
			Returns true when everything was ok, and false in other circunstance. */
		bool simulate (MCHEmul::CPU* cpu);

		/**
		  *	The name of the fields are: \n
		  * The ones comming from the parent class and: \n
		  * STATUS			= Attribute: YES (1) if active and NO (0) in any other case. \n
		  * RUNMODE			= Attribute: 0 = _FROMINITIALVALUE, 1 = _CONTINUOUS. \n
		  *	IRQ				= Attribute: YES (1) when IRQ are enabled and NO in other cas.
		  *	VALUE			= Attribute: Current value of the timer.
		  *	INITIALVALUE	= Attribute: Initial value of the timer.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		// Managing the "Timer"...
		/** 
		  * To count down to 0. \n
		  * The method will return true when it reaches 0. \n
		  * If the "Timer" reaches 0, the varible _reaches0 will be set to true. \n
		  *	When this variable is read, then becomes false back.
		  */
		bool countDown (MCHEmul::CPU* cpu);

		private:
		const int _id;
		RunMode _runMode;

		// The variables that define a timer...
		unsigned short _initialValue;
		bool _interruptEnabled;

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
