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

	/** A countdown timer with a selected count source and timeout policy. \n
		Zero reached by a decrement, timeout and automatic reload are distinct phases. */
	class VIATimer : public MCHEmul::InfoClass
	{
		public:
		friend VIA;

		/** Events generated while the timer simulates one PHI2 cycle. */
		struct CycleResult final
		{
			CycleResult ()
				: _timeout (false),
				  _lowByteReached0 (false)
							{ }

			bool _timeout;
			bool _lowByteReached0;
		};

		/** Timeout and reload policy. \n
			Not all modes are applicable to the two timers. */
		enum class RunMode
		{
			// Timer 1 and Timer 2; T2 accepts PHI2 or PB6 falling-edge counting.
			_ONESHOOT			= 0,	// One interrupt per explicit counter load; counting continues.
										// The current one-shot model requests at the first decrement to zero.
										// No "reflection" of the status in the bit 7 at the port linked (@see VIAPort).
			// This free-running mode is only applicable to Timer 1.
			_CONTINUOUS			= 1,	// Underflows after 0 and reloads on the following cycle.
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
			_PULSERECEIVED		= 1		// On every negative-going edge sampled on PB6.
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
							{ _initialValue = iV; }
		void setLatchLow (const MCHEmul::UByte& v)
							{ _initialValue = (_initialValue & 0xff00) | v.value (); }
		void setLatchHigh (const MCHEmul::UByte& v)
							{ _initialValue = (_initialValue & 0x00ff) | (v.value () << 8); }
		void clearInterrupt ()
							{ _interruptRequested = false; }
		void loadCounter ()
							{ reset (); start (); }
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

		/** Simulates one PHI2 cycle and returns event flags, not a success code. \n
			_lowByteReached0 reports a consumed decrement whose low byte became zero. \n
			_timeout follows the selected run policy: the current one-shot model uses
			a decrement to zero; continuous mode uses zero-to-$ffff underflow. \n
			Continuous reload consumes the next cycle without decrement or event,
			giving a steady timeout period of latch + 2 cycles. */
		virtual CycleResult simulate (MCHEmul::CPU* cpu);

		/**
		  *	The name of the fields are: \n
		  * The ones comming from the parent class and: \n
		  * RUN				= Attribute: Run mode of the timer. \n
		  *	COUNT			= Attribute: Count mode of the timer. \n
		  *	IRQ				= Attribute: YES (1) when interrupts are enabled; this is the IER mask, not the pending flag.
		  *	VALUE			= Attribute: Current value of the timer.
		  *	INITIALVALUE	= Attribute: Initial value of the timer.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		// Managing the "Timer"...
		/** 
		  * To count down to 0. \n
		  * The method returns true when a decrement is consumed. \n
		  * The way of counting down will depend on the counting mode,
		  * If the "Timer" reaches 0, the varible _reaches0 will be set to true. \n
		  * The zero flags describe this decrement and remain valid for this cycle.
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
		/** A continuous-mode underflow reloads the latch on the next cycle. */
		bool _reloadPending;
		/** The value the "Timer" has per loop. */
		unsigned short _currentValue; 
		
		/** True only in the simulated cycle whose consumed decrement reaches zero. */
		bool _reaches0;
		/** The same with this varible when LSB reaches 0. */
		bool _reaches0LSB;
		/** Remembers that the first one-shot zero event has been consumed until reset/load. */
		bool _firstTimeReaches0;

		/** Pending flag: cleared by acknowledgement; interruptRequested() consumes it,
			whereas peekInterruptRequested() and IFR reads preserve it. */
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

		/** Timer 2 is always one-shot, independently of its count source. \n
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
