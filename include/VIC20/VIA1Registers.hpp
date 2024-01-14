/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: VIA1Registers.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 17/12/2023 \n
 *	Description: VIC20 VIA 1 Registers Emulation.
 *	Versions: 1.0 Initial.
 */

#ifndef __VIC20_VIA1REGISTERS__
#define __VIC20_VIA1REGISTERS__

#include <COMMODORE/incs.hpp>

namespace VIC20
{
	class VIA1;

	/** Just to identify this space of the memory in a different section. */
	class VIA1Registers final : public COMMODORE::VIARegisters
	{
		public:
		friend VIA1;

		static const int _VIA1_SUBSET = 2100;

		VIA1Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);
	};
}

#endif
  
// End of the file
/*@}*/
