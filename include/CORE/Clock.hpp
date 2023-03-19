/** \ingroup CORE */
/*@{*/

/**	
*	@file	
*	File: Clock.hpp \n
*	Framework: CPU Emulators library \n
*	Author: Ignacio Cea Forniés (EMULATORS library) \n
*	Creation Date: 21/12/2021 \n
*	Description: To regulate or synchonize the speed of different things.
*	Versions: 1.0 Initial
*/

#ifndef __MCHEMUL_CLOCK__
#define __MCHEMUL_CLOCK__

#include <chrono>
#include <assert.h>

namespace MCHEmul
{
	/** Used internally to align the speeds. 
		This clock counts in nanoseconds!. */
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
		Clock (unsigned int cS, double f = 1.01526)
			: _cyclesPerSecond (cS),
			  _realCyclesPerSecond (0),
			  _factor (f),
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

		private:
		unsigned int _cyclesPerSecond;
		unsigned int _realCyclesPerSecond;
		double _factor; // A factor for the speed of the clock...
		bool _tooQuick;

		// Implementation
		unsigned int _realCyclesPerSecondTmp;
		bool _realCyclesPerSecondCalculated;
		std::chrono::time_point <std::chrono::steady_clock> _iClock;
	};
}

#endif

// End of the file
/*@}*/
