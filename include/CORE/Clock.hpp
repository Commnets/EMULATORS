/** \ingroup CORE */
/*@{*/

/**	
*	@file	
*	File: Clock.hpp \n
*	Framework: CPU Emulators library \n
*	Author: Ignacio Cea Fornies (EMULATORS library) \n
*	Creation Date: 21/12/2021 \n
*	Description: To regulate or synchonize the speed of different things.
*	Versions: 1.0 Initial
*/

#ifndef __MCHEMUL_CLOCK__
#define __MCHEMUL_CLOCK__

#include <chrono>
#include <vector>
#include <assert.h>
#include <CORE/Ticks.hpp>

namespace MCHEmul
{
	/** Used internally to align the speeds. 
		This clock counts in nanoseconds!. 
		The clock cn be linked with other, but it is not the owner of those! */
	class Clock final
	{
		public:
		static const long long _NANOSECOND = (long long) 1.0e9;

		Clock () = delete;

		/** 
		  * Constructor.
		  * @param cS	The number of clocks per second. \n
		  * @param f	A factor to affect the number of clocks. \n
		  *				It can be used to temporaly affcet the speed of the clock. *
		  */
		Clock (unsigned int cS, double f = 1.0);

		/** To change the factor. */
		double factor () const
							{ return (_factor); }
		void setFactor (double f);

		/** To manage the ticks. \n
			Replacing the collection invalidates references previously obtained from it. \n
			The collection has to be configured before assigning its counters to motherboard elements. */
		const TicksCountersDelayed& ticksCounters () const
							{ return (_ticks); }
		void setTicksCounters (const TicksCountersDelayed& tC)
							{ _ticks = tC; }
		void setTicksCounters (TicksCountersDelayed&& tC)
							{ _ticks = std::move (tC); }
		inline const TicksCounterDelayed& ticksCounter (size_t t = 0) const;
		inline TicksCounterDelayed& ticksCounter (size_t t = 0);

		unsigned int cyclesPerSecond () const
							{ return (_cyclesPerSecond); }
		unsigned int realCyclesPerSecond () const
							{ return (_realCyclesPerSecond); }
		bool tooQuick () const
							{ return (_tooQuick); }

		/** Just to put everything at the beginning. */
		void start ();
		/** Just to calculate the speed. */
		void countCycles (unsigned int cC);

		/** To get the clock info as an string. */
		std::string asString () const;

		private:
		static const unsigned int _ADJUSTMENTSPERSECOND = 1000;
		static const unsigned short _REQUESTSBETWEENTIMECHECKS = 32;

		void actualizeCyclesPerAdjustment ();
		void actualizeMeasurement
			(const std::chrono::time_point <std::chrono::steady_clock>& n);
		/** Completes the current speed adjustment. \n
			Ordinary host-side latency preserves the scheduled clock phase. \n
			A delay of at least one complete adjustment period resets that phase,
			preventing an uncontrolled attempt to recover elapsed host time. */
		void finishAdjustment
			(const std::chrono::time_point <std::chrono::steady_clock>& n);

		private:
		unsigned int _cyclesPerSecond;
		unsigned int _realCyclesPerSecond;
		TicksCountersDelayed _ticks;
		double _factor; // A factor for the speed of the clock...
		bool _tooQuick;

		// Implementation
		unsigned long long _realCyclesPerSecondTmp;
		unsigned int _cyclesPerAdjustment;
		unsigned long long _cyclesSinceLastAdjustment;
		unsigned short _requestsSinceLastTimeCheck;
		std::chrono::time_point <std::chrono::steady_clock> _iClock;
		std::chrono::time_point <std::chrono::steady_clock> _adjustmentClock;
		std::chrono::time_point <std::chrono::steady_clock> _resumeClock;
	};

	// ---
	inline const MCHEmul::TicksCounterDelayed& MCHEmul::Clock::ticksCounter (size_t t) const
	{
		assert (t < _ticks.size ());

		return (_ticks [t]);
	}

	// ---
	inline MCHEmul::TicksCounterDelayed& MCHEmul::Clock::ticksCounter (size_t t)
	{
		assert (t < _ticks.size ());

		return (_ticks [t]);
	}
}

#endif

// End of the file
/*@}*/
