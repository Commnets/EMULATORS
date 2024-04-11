/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: INTInterrupt.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 11/11/2023 \n
 *	Description: The INT type of interrupt in FZ80.
 *	Versions: 1.0 Initial
 */

#ifndef __FZ80_INTINTERRUPT__
#define __FZ80_INTINTERRUPT__

#include <CORE/incs.hpp>
#include <FZ80/Interrupt.hpp>

namespace FZ80
{
	/** A Maskarable Interrupt. It could be avoided using the bit I of the status flag. */
	class INTInterrupt final : public Interrupt
	{
		public:
		static const unsigned int _ID = 0;

		INTInterrupt ()
			: Interrupt (_ID, 0 /** It will decided any time the INTMode is fixed. (@see below) */),
			  _INTMode (0) // The basic one by default...
							{ setClassName ("INTInterrupt"); }

		/** To manage the mode of the interruption. */
		unsigned char INTMode () const
							{ return (_INTMode); }
		inline void setINTMode (unsigned char iM); // To set also the number of cycles to lunch...

		private:
		virtual bool isTime (MCHEmul::CPU* c, unsigned int cC) const override;
		virtual bool executeOverImpl (MCHEmul::CPU* c, unsigned int cC) override;

		private:
		/** The type of interruption:
			0:	The interruption executes as instruction the next value in the data bus.
			1:	The interruption executes the routine in a specific memory address. 
			2:	The interruption executes the routine through out a table. */
		unsigned char _INTMode;
	};

	// ---
	inline void INTInterrupt::setINTMode (unsigned char iM)
	{
		_INTMode = iM;
		if (_INTMode == 0)
			_cyclesToLaunch = 2; // + the ones that the instruction will take...
								 // This mode is very dynamic! (@see _cyclesAfterLaunch)
		else
		if (_INTMode == 1)
			_cyclesToLaunch = 13; // like restart (11) + 2
		else
		if (_INTMode == 2)
			_cyclesToLaunch = 19; // 7 to fetch the lower address + 6 to save the program counter + 6 to make the jump
		else
			assert (false); // If should be here...
	}
}

#endif
  
// End of the file
/*@}*/