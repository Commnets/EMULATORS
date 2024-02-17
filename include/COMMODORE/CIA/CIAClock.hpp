/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: CIAClock.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 07/08/2021 \n
 *	Description: CIA Chip manages a day clock.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_CIACLOCK__
#define __COMMODORE_CIACLOCK__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	class CIAClock final : public MCHEmul::InfoClass
	{
		public:
		/** The constructor receives the id of the element,
			and the id of the type of interruptions to be launched when limits are reached. */
		CIAClock (int id /** unique in the CIA chip. */, unsigned int iID)
			: MCHEmul::InfoClass ("CIAClock"),
			  _id (id), _interruptId (iID)
							{ initialize (); }

		int id () const
							{ return (_id); }

		/** To point whether the timer reached the alarm set.
			This variable will be true until the clock moves to the next tenth of second. */
		bool reachesAlarm () const
							{ return (_reachesAlarm); }

		/** To define whether Clock related interruptions are allowed. */
		bool interruptEnabled () const
							{ return (_interruptEnabled); }
		void setInterruptEnabled (bool e)
							{ _interruptEnabled = e; }
		
		/** To know whether the interrupt condition for the Clock has or not been reached. \n
			When this variable is read, the value comes back to false. */
		bool interruptRequested () const
							{ bool r = _interruptRequested; _interruptRequested = false; return (r); }
		/** Same than the previous method but without modifying the value. */
		bool peekInterruptRequested () const
							{ return (_interruptRequested); }

		/** To know whether an interruption related with the Clock should be launched. \n
			That happen when there is a interruption requested and also they are allowed. */
		bool launchInterruption () const
							{ return (_interruptRequested && _interruptEnabled); }

		/** To set the alarm by pieces,
			because this is the way the CIA chip works. */
		void setAlarmHours (unsigned char h)
							{ _stopped = true, _alarmHours = h; }
		void setAlarmMinutes (unsigned char m)
							{ _alarmMinutes = m; }
		void setAlarmSeconds (unsigned char s)
							{ _alarmSeconds = s; }
		void setAlarmTenthSeconds (unsigned char ts)
							{ _alarmTenthsSecond = ts; _stopped = false;}

		void setHours (unsigned char h)
							{ _stopped = true, _hours = h; }
		void setMinutes (unsigned char m)
							{ _minutes = m; }
		void setSeconds (unsigned char s)
							{ _seconds = s; }
		void setTenthSeconds (unsigned char ts)
							{ _tenthsSecond = ts; _stopped = false;}

		/** To know the alarm, by pieces.
			This is the way the chips works. 
			Anytime hours are read, no new value can be read until tenths of second are also read. */
		unsigned char hours () const
							{ _timeLatched = true; return (_hours); }
		unsigned char minutes () const
							{ return (_timeLatched ? _minutesL : _minutes); }
		unsigned char seconds () const
							{ return (_timeLatched ? _secondsL : _seconds); }
		unsigned char tenthsSecond () const
							{ unsigned char r = _timeLatched ? _tenthsSecondL : _tenthsSecond; _timeLatched = false; return (r); }

		// Equivalent to the previous methods
		// ...but not affecting the variable.
		// Usually there are invoked in the peekValue method of the CIARegister...
		unsigned char peekHours () const
							{ return (_hours); }
		unsigned char peekMinutes () const
							{ return (_minutes); }
		unsigned char peekSeconds () const
							{ return (_seconds); }
		unsigned char peekTenthsSecond () const
							{ return (_tenthsSecond); }

		/** To initialize the timer. By default it is not enabled. */
		void initialize ();

		/** To simulate the behaviour of the timer. \n
			It invokes also some private methods.
			When an interruption is launches the variable _IRQRequested is set. 
			It becomes back to false when it is read. */
		void simulate (MCHEmul::CPU* cpu);

		/**
		  *	The name of the fields are: \n
		  *	IRQ				= Attribute with YES when IRQ are enabled and NO in other case.
		  *	HOURS			= Attribute with the current value of the hours.
		  *	MINUTES			= Attribute with the current value of the minutes.
		  *	SECONDS			= Attribute with the current value of the seconds.
		  *	TENTHSECONDS	= Attribute with the current value of the tenth of seconds.
		  *	AHOURS			= Attribute with the current value of the alarm for hours.
		  *	AMINUTES		= Attribute with the current value of the alarm for minutes.
		  *	ASECONDS		= Attribute with the current value of the alarm for seconds.
		  *	ATENTHSECONDS	= Attribute with the current value of the alarm for tenth of seconds.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		// Managing the timer...
		/** To actualize the time. */
		void actualizeTime ();

		private:
		const int _id;
		const unsigned int _interruptId;

		// The variables that define a timer...
		bool _interruptEnabled;
		unsigned char _alarmHours, _alarmMinutes, _alarmSeconds, _alarmTenthsSecond;

		// It is actualized continiously thorugh the method actualizeTime...
		// The time counts only hours in the day...
		MCHEmul::ClockDurationType _time;

		// Implementation
		/** The value of the clock cycles las time the timer counted down. */
		unsigned int _lastClockCycles;
		/** The current _time is divided always in: day, minutes, seconds and tenths of second. */
		unsigned char _hours, _minutes, _seconds, _tenthsSecond;
		/** If the hour variable is read (hours), the rest of the variables are latched (_timeLatched = true)
			until the tenths of second (tenthsSecond) are also read. \n
			However the time is always actualized. */
		mutable unsigned char _minutesL, _secondsL, _tenthsSecondL;
		mutable bool _timeLatched;
		/** The clock stops when hours are set either for alarm of for starting back purposes. */
		mutable bool _stopped;
		/** When the alarm time is reached, this variable becomes true, just for the time the clock starts back if any. */
		bool _reachesAlarm;
		/** When an IRQ interruption is requested this variable is set to true, until it is read. */
		mutable bool _interruptRequested;
	};
}

#endif
  
// End of the file
/*@}*/
