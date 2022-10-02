/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: ChipRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 02/10/2022 \n
 *	Description: All Registers emulated inherit from this one.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_CHIPREGISTERS__
#define __C64_CHIPREGISTERS__

#include <CORE/incs.hpp>

namespace C64
{
	/** Just to print basically any register. */
	class ChipRegisters : public MCHEmul::PhisicalStorageSubset
	{
		public:
		ChipRegisters (int id, MCHEmul::PhisicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
			: MCHEmul::PhisicalStorageSubset (id, ps, pp, a, s)
							{ }

		/** To be redefined later to return the specific number of registers of the chip. */
		virtual size_t numberRegisters () const = 0;

		friend std::ostream& operator << (std::ostream& o, const ChipRegisters& vr);
	};
}

#endif
  
// End of the file
/*@}*/
