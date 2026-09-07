/** \ingroup CORE */
/*@{*/

/**	
*	@file	
*	File: Ticks.hpp \n
*	Framework: CPU Emulators library \n
*	Author: Ignacio Cea Fornies (EMULATORS library) \n
*	Creation Date: 18/03/2023 \n
*	Description: To count ticks. \n
*				 It is the very basic expression of a clock
*	Versions: 1.0 Initial
*/

#ifndef __MCHEMUL_TICKS__
#define __MCHEMUL_TICKS__

#include <assert.h>
#include <string>
#include <vector>
#include <CORE/InfoStructure.hpp>

namespace MCHEmul
{
	/** Just to count ticks (@see method count). \n
		Partial counts can be also considerered. */
	class TicksCounter final
	{
		public:
		/** The number of initial tick is optional. */
		TicksCounter (unsigned int nT = 0)
			: _ticks (nT),
			  _lastTicks (nT),
			  _countingTemporal (false),
			  _latchedTicks (nT)
							{ }
		
		unsigned int ticks () const
							{ return (_ticks); }
		unsigned int lastTicks () const
							{ return (_lastTicks); }
		unsigned int elapsedTicks () const
							{ return (_ticks - _lastTicks); }

		inline unsigned int count (unsigned int nT);
							
		/** To start to count partial ticks. */
		inline void startPartialCounter ();
		bool partialCounterOn () const
							{ return (_countingTemporal); }
		/** End the partial counting. \n
			Returns the number of ticks counted. */
		inline unsigned int endPartialCounter ();
		
		/** To restart the counter. */
		inline void reset ();
		
		operator unsigned int () const
							{ return (_ticks); }

		/** To get the info of the ticks counter as a string. */
		std::string asString () const
							{ return (std::to_string (_ticks) + 
								(_countingTemporal ? ("(" + std::to_string (_latchedTicks) + ")") : "")); }
		
		private:
		unsigned int _ticks;
		unsigned int _lastTicks;
		
		// Implementation...
		bool _countingTemporal;
		unsigned int _latchedTicks;
	};
	
	using TicksCounters = std::vector <TicksCounter>;

	// ---
	inline unsigned int TicksCounter::count (unsigned int nT)
	{ 
		_lastTicks = _ticks; 
		
		return (_ticks += nT); 
	}

	// ---
	inline void TicksCounter::startPartialCounter ()
	{ 
		if (!_countingTemporal) 
		{ 
			_countingTemporal = true;
	
			_latchedTicks = _ticks;
		} 
	}
	
	// ---
	inline unsigned int TicksCounter::endPartialCounter ()
	{
		if (!_countingTemporal)
			return (0);
		
		_countingTemporal = false;
		
		return (_ticks - _latchedTicks);
	}
	
	// ---
	inline void TicksCounter::reset ()
	{ 
		_ticks = _lastTicks = _latchedTicks = 0;

		_countingTemporal = false;
	}
	
	/** This is a special type of tick counter. \n
		A derived tick is counted after receiving a configured number of reference ticks. \n
		Requests not completing a derived tick are kept for the following count invocation. */
	class TicksCounterDelayed final
	{
		public:
		/** nR positive. */
		TicksCounterDelayed (unsigned short nR)
			: _numberRequestsToWait (nR),
			  _ticks (),
			  _requestsLeft (0)
							{ assert (_numberRequestsToWait != 0); }
		
		unsigned short numberRequestsToWait () const
							{ return (_numberRequestsToWait); }
		/** The pending requests to be counted are also reset. */
		inline void setNumberRequestsToWait (unsigned short nC);
		
		unsigned int ticks () const
							{ return (_ticks); }
		unsigned int lastTicks () const
							{ return (_ticks.lastTicks ()); }
		unsigned int elapsedTicks () const
							{ return (_ticks.elapsedTicks ()); }

		/** Counts ticks received from the reference clock. \n
			The returned value is the total number of derived ticks. \n
			The method elapsedTicks returns the derived ticks produced by this invocation. */
		inline unsigned int count (unsigned int nR);
							
		/** To start to count partial ticks. */
		inline void startPartialCounter ()
							{ _ticks.startPartialCounter (); }
		/** To end the partial counter. \n
			It will return the number of ticks counted. */
		unsigned int endPartialCounter ()
							{ return (_ticks.endPartialCounter ()); }
		
		inline void reset ();
		
		operator unsigned int () const
							{ return (_ticks); }

		std::string asString () const
							{ return (_ticks.asString () + "," + 
								std::to_string (_numberRequestsToWait)); }
							
		private:
		unsigned short _numberRequestsToWait;
		TicksCounter _ticks;
		
		// Implementation
		unsigned short _requestsLeft;
	};
	
	using TicksCountersDelayed = std::vector <TicksCounterDelayed>;
	
	// ---
	inline void TicksCounterDelayed::setNumberRequestsToWait (unsigned short nC)
	{
		assert (nC != 0);

		_numberRequestsToWait = nC;
		
		// Changing the divisor starts a new reference-clock phase,
		// but it does not reset the derived ticks already counted.
		_requestsLeft = 0;
	}
	
	// ---
	inline unsigned int TicksCounterDelayed::count (unsigned int nC)
	{
		unsigned long long requests =
			(unsigned long long) _requestsLeft + (unsigned long long) nC;

		_requestsLeft = (unsigned short)
			(requests % (unsigned long long) _numberRequestsToWait);

		return (_ticks.count ((unsigned int)
			(requests / (unsigned long long) _numberRequestsToWait)));
	}
	
	// ---
	inline void TicksCounterDelayed::reset ()
	{
		_ticks.reset ();
		
		_requestsLeft = 0;
	}
}

#endif

// End of the file
/*@}*/
