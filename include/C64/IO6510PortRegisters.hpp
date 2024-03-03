/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: IO6510PortRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 01/03/2024 \n
 *	Description: The 6510 port with some details. \n
 *	Versions: 1.0 Initial
 */

#ifndef __C64_IO6150PORTREGISTERS__
#define __C64_IO6150PORTREGISTERS__

#include <F6500/incs.hpp>

namespace C64
{
	class SpecialFunctionsChip;

	class IO6510PortRegisters final : public F6500::IO6510PortRegisters
	{
		public:
		friend SpecialFunctionsChip;

		static const int _IO6510REGISTERS_SUBSET = 2040;

		IO6510PortRegisters (MCHEmul::PhysicalStorage* ps)
			: F6500::IO6510PortRegisters (_IO6510REGISTERS_SUBSET, ps)
							{ }
	};
}

#endif
  
// End of the file
/*@}*/
