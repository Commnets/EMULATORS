/** \ingroup F6500 */
/*@{*/

/**	
 *	@file	
 *	File: IO7501PortRegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 01/03/2024 \n
 *	Description: The 7501 chip uses a port. \n
 *				 That port is located at address 0 (data direction) and 1 (port value) of the RAM. \n
 *	Versions: 1.0 Initial
 */

#ifndef __F6500_IO7501PORTREGISTERS__
#define __F6500_IO7501PORTREGISTERS__

#include <F6500/IO6510PortRegisters.hpp>

namespace F6500
{
	/** The port is like 6510's one, but with 8 bits used insted 6. */
	class IO7501PortRegisters : public IO6510PortRegisters
	{
		public:
		IO7501PortRegisters (int id, MCHEmul::PhysicalStorage* ps)
			: IO6510PortRegisters (id, ps, 0b11111111 /** the 6 bits are used. */)
							{ setClassName ("IO7501PortRegisters"); }
	};
}

#endif
  
// End of the file
/*@}*/
