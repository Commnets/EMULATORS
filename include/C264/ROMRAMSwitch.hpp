/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: ROMRAMSwitch.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 10/03/2024 \n
 *	Description: In the C264 series memory, 
 *				 there is a couple of registers ($ff3e and $ff3f) that are used
 *				 to select ROM or RAM in the address space $8000 - $ffff. \n
 *				 We have tried to emulate that behaviour with this virtual chip.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_ROMRAMSWITCH__
#define __C264_ROMRAMSWITCH__

#include <COMMODORE/incs.hpp>

namespace C264
{
	class ROMRAMSwitchRegisters;

	class ROMRAMSwitch final : public MCHEmul::Chip
	{
		public:
		static const int _ID = 202;
		
		ROMRAMSwitch ();

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugROMRAMSwitchCycle (MCHEmul::CPU* cpu);
		// -----

		private:
		// Implementation
		/** The memory is used also as the set of registers of the chip. */
		ROMRAMSwitchRegisters* _ROMRAMSwitchRegisters;
	};
}

#endif
  
// End of the file
/*@}*/
