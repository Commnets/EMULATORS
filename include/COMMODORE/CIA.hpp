/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: CIA.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 14/01/2023 \n
 *	Description: The CIA Chip.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_CIA__
#define __COMMODORE_CIA__

#include <CORE/incs.hpp>
#include <COMMODORE/CIARegisters.hpp>
#include <COMMODORE/CIATimer.hpp>
#include <COMMODORE/CIAClock.hpp>

namespace COMMODORE
{
	/** The chip is a I/O Chip, with different uses. */
	class CIA : public MCHEmul::Chip
	{
		public:
		static const unsigned int _ID = 102;
		
		/** Receives the id of the chip in the computer, the id of the registers linked
			and the if og the interruption to launch when needed. \n
			In a computer there could be many of these with little differences. */
		CIA (int id, int rId, unsigned int intId);

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * The ones comming from the parent class. \n
		  * Registers	= InfoStructure: Value of the registers. \n
		  *	TimerA		= InfoStructure: Info about the timer A. \n
		  *	TimerB		= InfoStructure: Info about the timer B. \n
		  *	Clock		= InfoStructure: Info about the clock. \n
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		CIARegisters* _CIARegisters;
		const int _registersId;
		CIATimer _timerA, _timerB;
		CIAClock _clock;

		// Implementation
		unsigned int _lastClockCycles;
		unsigned int _timerAValueAtPortB;
		bool _pulseTimerASentToPortB;
		unsigned int _timerBValueAtPortB;
		bool _pulseTimerBSentToPortB;
	};
}

#endif
  
// End of the file
/*@}*/
