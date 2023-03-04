/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: SFChip.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 25/07/2022 \n
 *	Description: Chip to regulate special behaviours.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_SFCHIP__
#define __C64_SFCHIP__

#include <CORE/incs.hpp>
#include <C64/SFChipRegisters.hpp>

namespace C64
{
	/** Very critical chip. \n
		The direction of the IO Channels are configurated here at the position 0x00. \n
		The configuration of the memory is controlled with this chip, accesing the possition 0x01 of the page 0. */
	class SpecialFunctionsChip final : public MCHEmul::Chip
	{
		public:
		static const unsigned int _ID = 1040;

		/** Event when the C64 Port IO bits are actualized. */
		static const unsigned int _C64PORTIOBITSACTUALIZED = 300;

		SpecialFunctionsChip ();

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* ntier) override;

		private:
		C64::SpecialFunctionsChipRegisters* _SFRegisters;
	};
}

#endif
  
// End of the file
/*@}*/
