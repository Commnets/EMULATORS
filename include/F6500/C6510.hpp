/** \ingroup F6500 */
/*@{*/

/**	
 *	@file	
 *	File: C6510.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 11/04/2021 \n
 *	Description: The main CPU of the C64.
 *	Versions: 1.0 Initial
 */

#ifndef __F6500_C6510__
#define __F6500_C6510__

#include <F6500/C6500.hpp>
#include <F6500/IO6510PortRegisters.hpp>

namespace F6500
{
	/** The Chip CPU 6510 */
	class C6510 : public C6500
	{
		public:
		C6510 (int id, 
			const MCHEmul::Attributes& attr = // Can be changed...
				{ { "Code", "6510/6510-1/6510-2" },
				  { "Manufacturer", "MOS Technology INC/Commodore Semiconductor Group (CBM)"},
				  { "Year", "1980" },
				  { "Speed Range", "0.985(PAL) - 1.023(NTSC) MHz" } });

		/** Create the port registers associated over a physical storage. \n
			Can be overloaded as other CPU similar to this one are slightly different. */
		virtual MCHEmul::PhysicalStorageSubset* createPortRegistersOn (int id, MCHEmul::PhysicalStorage* ps)
							{ return (new F6500::IO6510PortRegisters (id, ps)); }
	};
}

#endif
  
// End of the file
/*@}*/
