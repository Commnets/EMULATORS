/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: CIA.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forni�s (EMULATORS library) \n
 *	Creation Date: 22/05/2021 \n
 *	Description: The CIA Chips.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_CIA2__
#define __C64_CIA2__

#include <CORE/incs.hpp>
#include <COMMODORE/incs.hpp>
#include <C64/CIA2Registers.hpp>

namespace C64
{
	class VICII;
	class Commodore64;

	/** The chip is to communicate the C64 with the environment. */
	class CIA2 final : public COMMODORE::CIA
	{
		public:
		friend Commodore64;

		static const unsigned int _ID = 1030;

		CIA2 ()
			: CIA (_ID, CIA2Registers::_CIA2_SUBSET, F6500::NMIInterrupt::_ID),
			  _CIA2Registers (nullptr),
			  _VICIIRef (nullptr)
							{ setClassName ("CIA2"); }

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		private:
		C64::CIA2Registers* _CIA2Registers;
		/** Set directly by Commodore 64. */
		COMMODORE::VICII* _VICIIRef;
	};
}

#endif
  
// End of the file
/*@}*/
