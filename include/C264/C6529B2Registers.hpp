/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: C6529B2Registers.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 20/02/2024 \n
 *	Description: C6529B2 Registers Emulation.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_C6529B2REGISTERS__
#define __C264_C6529B2REGISTERS__

namespace C264
{
	class C6529B2;

	class C6529B2Registers final : public COMMODORE::C6529BRegisters
	{
		public:
		friend C6529B2;

		static const int _C6529B2REGS_SUBSET = 2010;

		C6529B2Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
			: COMMODORE::C6529BRegisters (_C6529B2REGS_SUBSET, ps, pp, a, s)
							{ }
	};
}

#endif
  
// End of the file
/*@}*/
