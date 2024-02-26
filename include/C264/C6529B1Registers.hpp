/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: C6529B1Registers.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 20/02/2024 \n
 *	Description: C6529B1 Registers Emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_C6529B1REGISTERS__
#define __C264_C6529B1REGISTERS__

#include <COMMODORE/incs.hpp>

namespace C264
{
	class C6529B1Registers final : public COMMODORE::C6529BRegisters
	{
		public:
		static const int _C6529B1REGS_SUBSET = 2000;

		C6529B1Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
			: COMMODORE::C6529BRegisters (_C6529B1REGS_SUBSET, ps, pp, a, s)
							{ }
	};
}

#endif
  
// End of the file
/*@}*/
