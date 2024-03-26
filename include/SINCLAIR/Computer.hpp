/** \ingroup SINCLAIR */
/*@{*/

/**	
 *	@file	
 *	File: Computer.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 24/03/2024 \n
 *	Description: Generic calss for all Sinclair Computers.
 *	Versions: 1.0 Initial
 */

#ifndef __SINCLAIR_COMPUTER__
#define __SINCLAIR_COMPUTER__

#include <CORE/incs.hpp>

namespace SINCLAIR
{
	/** Just to recognize a Sinclair Computer. */
	class Computer : public MCHEmul::Computer
	{
		public:
		Computer (MCHEmul::CPU* cpu, 
				  const MCHEmul::Chips& c, 
				  MCHEmul::Memory* m, 
				  const MCHEmul::IODevices& d, 
				  unsigned int cs, 
				  const MCHEmul::Buses& bs,
				  const MCHEmul::Wires& ws,
				  const MCHEmul::Attributes& attrs)
			: MCHEmul::Computer (cpu, c, m, d, cs, bs, ws, attrs)
							{ }
							
		// TODO
	};
}

#endif
  
// End of the file
/*@}*/
