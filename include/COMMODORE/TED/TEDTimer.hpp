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
			_FROMRELOADVALUE	= 0,		// Meaning that when reaches 0,
											// starts back from the reload value.
			_FROMFFFF						// Meaning that when reaches 0,
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
		bool interruptRequested () const
							{ return (_interruptRequested); }
		void clearInterruptRequested ()
							{ _interruptRequested = false; }
		bool launchInterrupt () const
							{ return (_interruptRequested && _interruptEnabled); }

		// Managing the values of the "Timer"...
		unsigned short reloadValue () const
							{ return (_reloadValue); }
		unsigned short currentValue () const
							{ return (_currentValue); }
		/** Loading the low byte inhibits counting until the high byte is written. */
		void writeLowByte (unsigned char v);
		/** Loading the high byte completes the value and starts the timer. */
		void writeHighByte (unsigned char v);
		/** The "Timer" is "forced" to start back, but still stopped. */
		void reset ();

		void initialize ();

		/** To simulate one single-clock pulse received by the timer. */
		void clock ();

		/**
		  *	The name of the fields are: \n
		  * The ones coming from the parent class and: \n
		  * RUN				= Attribute: 0 = _FROMRELOADVALUE, 1 = _FROMFFFF. \n
		  *	IRQ				= Attribute: YES when interrupts are enabled and NO otherwise. \n
		  *	VALUE			= Attribute: Current timer counter value. \n
		  *	RELOADVALUE		= Attribute: Value programmed through the timer registers.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		// Managing the "Timer"...
		/** 
		  * To count down to 0. \n
		  * The method will return true when it reaches 0. \n
		  * If the "Timer" reaches 0, the varible _reaches0 will be set to true. \n
		  */
		bool countDown ();

		private:
		const int _id;
		RunMode _runMode;

		// The variables that define a timer...
		/** The value programmed through the timer registers. */
		unsigned short _reloadValue;
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

		/** When an interrupt is requested this variable is set to true, until it is acknowledged. */
		bool _interruptRequested;
	};
}

#endif
  
// End of the file
/*@}*/
