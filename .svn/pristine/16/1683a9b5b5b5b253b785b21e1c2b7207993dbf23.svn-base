/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: CIA.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 22/05/2021 \n
 *	Description: The CIA Chips.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_CIA2__
#define __C64_CIA2__

#include <CORE/incs.hpp>
#include <C64/CIA2Registers.hpp>
#include <C64/CIATimer.hpp>
#include <C64/CIAClock.hpp>

namespace C64
{
	class VICII;
	class Commodore64;

	/** The chip is to communicate the C64 with the environment. */
	class CIA2 : public MCHEmul::Chip
	{
		public:
		friend Commodore64;

		static const unsigned int _ID = 3;

		CIA2 ()
			: MCHEmul::Chip (_ID,
				{ { "Name", "CIA2" },
				  { "Code", "6526/6526A/8521" },
				  { "Manufacturer", "Commodore Business Machines CBM" },
				  { "Year", "1980" } }),
			  _CIA2Registers (nullptr),
			  _timerA (0), _timerB (1), _clock (0)
							{ }

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		private:
		C64::CIA2Registers* _CIA2Registers;
		/** Set directly by Commodore 64. */
		VICII* _VICIIRef;
		CIATimer _timerA, _timerB;
		CIAClock _clock;

		// Implementation
		unsigned int _lastClockCycles;
	};
}

#endif
  
// End of the file
/*@}*/
