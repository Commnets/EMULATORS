/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: Memory.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 15/05/2021 \n
 *	Description: C64 Memory
 *	Versions: 1.0 Initial
 */

#ifndef __C64_MEMORY__
#define __C64_MEMORY__

#include <CORE/incs.hpp>

namespace C64
{
	/** The color RAM type Memory manage only nibbles. */
	class ColorRAMMemory : public MCHEmul::Memory
	{
		public:
		ColorRAMMemory ()
			: MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0xd8 }, false), 0x0400)
							{ }

		protected:
		/** The high nibble of the color RAM Memory bytes are not connected, 
			so when a value is requested a random value is returned. */
		virtual MCHEmul::UByte readValue (size_t p) const override
							{ return (_values [p] & 0x0f | (unsigned char) ((rand () % 0x10) << 4)); }
	};
}

#endif
  
// End of the file
/*@}*/
