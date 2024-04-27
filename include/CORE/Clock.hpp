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
		Clock (unsigned int cS, double f = 1.0)
			: _cyclesPerSecond (cS),
			  _realCyclesPerSecond (0),
			  _factor (f),
			  _ticks ({ TicksCounterDelayed (1) }),
			  _tooQuick (false),
			  _realCyclesPerSecondTmp (0), 
			  _realCyclesPerSecondCalculated (false),
			  _iClock ()
							{ assert (_cyclesPerSecond > 0); }

		/** To change the factor. */
		double factor () const
							{ return (_factor); }
		void setFactor (double f)
							{ _factor = f; }

		/** To manage the ticks. */
		const TicksCountersDelayed& ticksCounters () const
							{ return (_ticks); }
		void setTicksCounters (const TicksCountersDelayed& tC)
							{ _ticks = tC; }
		void setTicksCounters (TicksCountersDelayed&& tC)
							{ _ticks = std::move (tC); }
		const TicksCounterDelayed& ticksCounter (size_t t = 0) const
							{ return (_ticks [t]); }
		TicksCounterDelayed& ticksCounter (size_t t = 0)
							{ return (_ticks [t]); }

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
		unsigned int _cyclesPerSecond;
		unsigned int _realCyclesPerSecond;
		TicksCountersDelayed _ticks;
		double _factor; // A factor for the speed of the clock...
		bool _tooQuick;

		// Implementation
		unsigned int _realCyclesPerSecondTmp;
		bool _realCyclesPerSecondCalculated;
		std::chrono::time_point <std::chrono::steady_clock> _iClock;
	};

	// ---
	inline void MCHEmul::Clock::start ()
	{
		_tooQuick = false;

		_realCyclesPerSecond = 0;

		_realCyclesPerSecondTmp = 0;

		_realCyclesPerSecondCalculated = false;

		_iClock = std::chrono::steady_clock ().now ();
	}

	// ---
	inline void Clock::countCycles (unsigned int cC)
	{
		// The time lasted from the last calculation (in nanoseconds) of the speed.
		// The variable iClock keeps always the last moment when the speed was calculated...
		long long elapsed = (std::chrono::steady_clock::now () - _iClock).count ();

		// If the real speed has already been calculated
		// Then it is latched in a variable accesible from outside
		// and starts back again the counter.
		// Otherwise keep counting cycles until one second has lasted.
		if (_realCyclesPerSecondCalculated)
		{
			_realCyclesPerSecond = _realCyclesPerSecondTmp;

			_iClock = std::chrono::steady_clock::now ();

			_realCyclesPerSecondTmp = 0;

			_realCyclesPerSecondCalculated = false;
		}
		else
		{
			if (elapsed < _NANOSECOND)
			{ 
				_realCyclesPerSecondTmp += cC;

				for (auto& i : _ticks)
					i.count (cC); // Affect the delayed ticks!
			}
			else 
				_realCyclesPerSecondCalculated = true;
		}

		// To stimate whether the clock goes or not too quick attending the last data...
		// Some tolerance is taking into account (2,5% estimated)
		_tooQuick = (_realCyclesPerSecondTmp * _NANOSECOND) > (_cyclesPerSecond * (elapsed * _factor));
	}
}

#endif

// End of the file
/*@}*/
