/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: C1C2Selector.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 28/09/2025 \n
 *	Description: In the C264 series memory, 
 *				 there is a couple of registers (from $fdd0 to fddf) that are used
 *				 to manages externals data than can connect a cartridge (or external memory in general)
 *				 between the position $8000 - $ffff. \n
 *				 We have tried to emulate that behaviour with this virtual Chip.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_C1C2SELECTOR__
#define __C264_C1C2SELECTOR__

#include <COMMODORE/incs.hpp>

namespace C264
{
	class C1C2SelectorRegisters;

	class C1C2Selector final : public MCHEmul::Chip
	{
		public:
		static const int _ID = 203;
		
		C1C2Selector ();

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugC1C2SelectorCycle (MCHEmul::CPU* cpu);
		// -----

		private:
		// Implementation
		/** The memory is used also as the set of registers of the chip. */
		C1C2SelectorRegisters* _C1C2SelectorRegisters;
	};
}

#endif
  
// End of the file
/*@}*/
