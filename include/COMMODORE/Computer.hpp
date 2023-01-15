/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: Computer.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 14/01/2023 \n
 *	Description: Generic calss for all Commodore Computers.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_COMPUTER__
#define __COMMODORE_COMPUTER__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	/** Just to recognize a Commdore Computer. */
	class Computer : public MCHEmul::Computer
	{
		public:
		Computer (MCHEmul::CPU* cpu, const MCHEmul::Chips& c, 
			MCHEmul::Memory* m, const MCHEmul::IODevices& d, unsigned int cs, const MCHEmul::Attributes& attrs)
			: MCHEmul::Computer (cpu, c, m, d, cs, attrs)
							{ }
	};
}

#endif
  
// End of the file
/*@}*/
