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
			  _lastTicks (0),
			  _countingTemporal (false),
			  _latchedTicks (0)
							{ }
		
		unsigned int ticks () const
							{ return (_ticks); }
		unsigned int lastTicks () const
							{ return (_lastTicks); }
		unsigned int elapsedTicks () const
							{ return ((_lastTicks > _ticks) // Above the limit!
								? (unsigned int) ((unsigned long long) _ticks + 
									std::numeric_limits <unsigned long long>::max () - (unsigned long long) _lastTicks)
								: _ticks - _lastTicks); }

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
		
		return ((_latchedTicks > _ticks) 
			? (unsigned int) ((unsigned long long) _ticks + 
				std::numeric_limits <unsigned long long>::max () - (unsigned long long) _latchedTicks)
			: _ticks - _latchedTicks);
	}
	
	// ---
	inline void TicksCounter::reset ()
	{ 
		_ticks = _lastTicks = _latchedTicks = 0;

		_countingTemporal = false;
	}
	
	/** This a special type of tick counter. \n
		A tick is only counted after X number of requests. */
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

		/** To count requests, that it is not the same than ticks. \n
			The number of ticks equivalent will actually depend on the number of requests 
			to wait before counting 1 single tick. */
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
		mutable unsigned short _requestsLeft;
	};
	
	using TicksCountersDelayed = std::vector <TicksCounterDelayed>;
	
	// ---
	inline void TicksCounterDelayed::setNumberRequestsToWait (unsigned short nC)
	{
		_numberRequestsToWait = nC;
		
		// But the number of current ticks is not reset...

		_requestsLeft = 0;
	}
	
	// ---
	inline unsigned int TicksCounterDelayed::count (unsigned int nC)
	{
		unsigned int result = 
			_ticks.count ((unsigned int) (nC / _numberRequestsToWait));
		if ((_requestsLeft += (nC % _numberRequestsToWait)) >= _numberRequestsToWait)
		{
			result = _ticks.count (1);
			
			_requestsLeft -= _numberRequestsToWait;
		}
		
		return (result);
	}
	
	// ---
	void TicksCounterDelayed::reset ()
	{
		_ticks.reset ();
		
		_requestsLeft = 0;
	}
}

#endif

// End of the file
/*@}*/
