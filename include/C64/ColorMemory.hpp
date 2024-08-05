/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: ColorMemory.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 15/05/2021 \n
 *	Description: C64 Color Memory. The MSB nibble is not connect and always return a random number
 *	Versions: 1.0 Initial
 */

#ifndef __C64_COLORMEMORY__
#define __C64_COLORMEMORY__

#include <CORE/incs.hpp>

namespace C64
{
	/** The color RAM type Memory manage only LSB nibbles. */
	class ColorRAMMemory final : public MCHEmul::PhysicalStorageSubset
	{
		public:
		static const int _COLOR_SUBSET = 1100;

		ColorRAMMemory (MCHEmul::PhysicalStorage* ps,
				size_t pp, const MCHEmul::Address& a, size_t s)
			: MCHEmul::PhysicalStorageSubset (_COLOR_SUBSET, ps, pp, a, s),
			  _lastValue (MCHEmul::PhysicalStorage::_DEFAULTVALUE)
							{ }

		private:
		/** The high nibble of the color RAM Memory bytes are not connected, 
			so when a value is requested a random value is returned in them. */
		virtual const MCHEmul::UByte& readValue (size_t p) const override
							{ return (_lastValue = MCHEmul::PhysicalStorageSubset::readValue (p) & 0x0f | 
								(unsigned char) ((rand () % 0x10) << 4)); }

		private:
		// Implementation
		mutable MCHEmul::UByte _lastValue;
	};
}

#endif
  
// End of the file
/*@}*/
