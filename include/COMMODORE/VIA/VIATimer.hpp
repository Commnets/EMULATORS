/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VIATimer.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
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
	class VIAPortB;

	/** Every "Timer" always count down, 
		and it is defined basically by an id, the way it runs (what to do once it reaches 0) 
		and the input signal used to count down.*/
	class VIATimer : public MCHEmul::InfoClass
	{
		public:
		friend VIA;

		/** What to do when reaching 0? \n
			Not all modes are applicable to the two timers. */
		enum class RunMode
		{
			// In Timer2 this mode applies just only when CountMode = _PROCESSORCYCLES
			_ONESHOOT			= 0,	// One single count - down. 
										// Interrupt flag sets when reaches 0, just once.
										// No "reflection" of the status in the bit 7 at the port linked (@see VIAPort).
			// In Timer2 this mode applies just only when CountMode = _PULSERECEIVED
			_CONTINUOUS			= 1,	// Starts back once 0 is reached.
										// Interrupt flag sets everytime it runs out.
										// No "reflection" in the bit 7 at port linked.
			// This two modes are only applicable to Timer1
			_ONESHOOTSIGNAL		= 2,	// Idem that _ONESHOOT but "reflecting" the situation in the bit at the port linked.
			_CONTINUOUSSIGNAL	= 3		// Idem than _CONTINUOUS but "reflecting" at the port linked the situation (bit 7).
		};                        

		/** Which signal flags a count - down? \n
			Not all modes are applicable to the two types of timers. 
			@see simulation y count down methods. */
		enum class CountMode
		{
			_PROCESSORCYCLES	= 0,	// With every clock cycle.
			// This counting mode is only applicable to Timer2....
			_PULSERECEIVED		= 1		// Every time bit 6 at port linked changes its status.
		};

		VIATimer (int id);

		int id () const
							{ return (_id); }

		// To control the way of working....
		RunMode runMode () const
							{ return (_runMode); }
		CountMode countMode () const
							{ return (_countMode); }
		/** Specific verifications can be done per type of Timer. */
		virtual void setCountAndRunMode (CountMode cM, RunMode rM)
							{ _countMode = cM; _runMode = rM; }

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

		/** The "Timer" is "forced" to start back, but still stopped. */
		virtual void reset ();

		/** To start/stop the counting process. */
		void start () 
							{ _counting = true; }
		void stop ()
							{ _counting = false; }

		virtual void initialize ();

		/** To simulate the behaviour of the timer. \n
			It invokes also some private methods. \n
			Returns true when everything was ok, and false in other circunstance. */
		virtual bool simulate (MCHEmul::CPU* cpu);

		/**
		  *	The name of the fields are: \n
		  * The ones comming from the parent class and: \n
		  * RUN				= Attribute: Run mode of the timer. \n
		  *	COUNT			= Attribute: Count mode of the timer. \n
		  *	IRQ				= Attribute: YES (1) when IRQ are enabled and NO in other cas.
		  *	VALUE			= Attribute: Current value of the timer.
		  *	INITIALVALUE	= Attribute: Initial value of the timer.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		// Managing the "Timer"...
		/** 
		  * To count down to 0. \n
		  * The method will return true when it reaches 0. \n
		  * The way of counting down will depend on the counting mode,
		  * If the "Timer" reaches 0, the varible _reaches0 will be set to true. \n
		  *	When this variable is read, then becomes false back.
		  */
		virtual bool countDown (MCHEmul::CPU* cpu);

		void lookAtPort (VIAPortB* p)
							{ _P = p; }

		protected:
		const int _id;

		// Elements the "Timer" has relation with...
		VIAPortB* _P;

		// The variables that define a timer...
		RunMode _runMode;
		CountMode _countMode;
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
		/** True when it is the first time 0 is reached. */
		bool _firstTimeReaches0;
		/** When half of the value is reached. */
		bool _reachesHalf;
		/** If already reached half... */
		bool _alreadyReachedHalf;

		/** When an interrupt is requested this variable is set to true, until it is read. */
		MCHEmul::OBool _interruptRequested;
	};

	/** The way this Timer works is specific. */
	class VIATimer1 final : public VIATimer
	{
		public:
		VIATimer1 (int id)
			: VIATimer (id)
							{ }

		/** Only _PROCESSORCYCLES counting mode is allowed. \n
			The system does an assert just to be sure under debugging modes. */
		virtual void setCountAndRunMode (CountMode cM, RunMode rM) override;

		virtual void reset ();

		virtual void initialize ();
	};

	/** The way this Timer works is specific. */
	class VIATimer2 final : public VIATimer
	{
		public:
		VIATimer2 (int id)
			: VIATimer (id)
							{ }

		/** Only _PROCESSORCYCLES counting mode is allowed with the running mode _ONESHOOT. \n
			And Only _PULSERECEIVED counting mode is allowed with the running mode _CONTINUOUS. \n
			The system does an assert just to be sure under debugging modes. */
		virtual void setCountAndRunMode (CountMode cM, RunMode rM) override;

		private:
		/** The Timer 2 is able to count down when it receives a negative edge in the P6. */
		virtual bool countDown (MCHEmul::CPU* cpu);
	};
}

#endif
  
// End of the file
/*@}*/
