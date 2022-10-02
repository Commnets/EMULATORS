/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: CIAClock.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 07/08/2021 \n
 *	Description: Any CIA Chip manages a day clock
 *	Versions: 1.0 Initial
 */

#ifndef __C64_CIACLOCK__
#define __C64_CIACLOCK__

#include <CORE/incs.hpp>
#include <C64/global.hpp>

namespace C64
{
	class CIAClock
	{
		public:
		CIAClock (int id /** unique in the CIA chip. */)
			: _id (id) 
							{ initialize (); }

		/** To initialize the timer. By default it is not enabled. */
		virtual void initialize ();

		int id () const
							{ return (_id); }

		/** To point whether the timer reached the alarm set.
			This variable will be true until the clock moves to the next tenth of second. */
		bool reachesAlarm () const
							{ return (_reachesAlarm); }

		bool IRQEnabled () const
							{ return (_IRQEnabled); }
		void setIRQEnabled (bool e)
							{ _IRQEnabled = e; }
		bool IRQRequested () const
							{ bool r = _IRQRequested; _IRQRequested = false; return (r); }

		/** To set the alarm by pieces,
			because this is the way the CIA chip works. */
		void setAlarmHours (unsigned char h)
							{ _stopped = true, _alarmHours = h; }
		void setAlarmMinutes (unsigned char m)
							{ _alarmMinutes = m; }
		void setAlarmSeconds (unsigned char s)
							{ _alarmSeconds = s; }
		void setAlarmTenthSeconds (unsigned char ts)
							{ _alarmTenthsSecond = ts; _stopped = true;}

		void setHours (unsigned char h)
							{ _stopped = true, _hours = h; }
		void setMinutes (unsigned char m)
							{ _minutes = m; }
		void setSeconds (unsigned char s)
							{ _seconds = s; }
		void setTenthSeconds (unsigned char ts)
							{ _tenthsSecond = ts; _stopped = true;}

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

		/** To simulate the behaviour of the timer. \n
			It invokes also some private methods.
			When an interruption is launches the variable _IRQRequested is set. 
			It becomes back to false when it is read. */
		void simulate (MCHEmul::CPU* cpu);

		friend std::ostream& operator << (std::ostream& o, const CIAClock& cc);

		private:
		// Managing the timer...
		/** To actualize the time. */
		void actualizeTime ();

		private:
		int _id;

		// The variables that define a timer...
		bool _IRQEnabled;
		unsigned char _alarmHours, _alarmMinutes, _alarmSeconds, _alarmTenthsSecond;

		// It is actualized continiously thorugh the method actualizeTime...
		// The time counts only hours in the day...
		Time _time;

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
		mutable bool _IRQRequested;
	};
}

#endif
  
// End of the file
/*@}*/
