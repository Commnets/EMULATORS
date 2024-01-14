/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: VIA2Registers.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 17/12/2023 \n
 *	Description: VCI20 VIA 1 Registers Emulation
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_VIA2REGISTERS__
#define __VIC20_VIA2REGISTERS__

#include <COMMODORE/incs.hpp>

namespace VIC20
{
	class VIA2;

	class VIA2Registers final : public COMMODORE::VIARegisters
	{
		public:
		friend VIA2;

		/** A name when this register is a subset in the main memory. */
		static const int _VIA2_SUBSET = 2110;

		VIA2Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);
	};
}

#endif
  
// End of the file
/*@}*/
