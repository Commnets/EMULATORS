/** \ingroup F6500 */
/*@{*/

/**	
 *	@file	
 *	File: C7501.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 02/03/2024 \n
 *	Description: The main CPU of the C264 series.
 *				 It is quite similar to the C6510 but it has no the NMI possibility!
 *				 and the port has 8 bits instead 6.
 *	Versions: 1.0 Initial
 */

#ifndef __F6500_C7501__
#define __F6500_C7501__

#include <F6500/C6510.hpp>
#include <F6500/IO7501PortRegisters.hpp>

namespace F6500
{
	/** The Chip CPU 7501. Similar to 6510. */
	class C7501 : public C6510
	{
		public:
		C7501 (int id);

		virtual MCHEmul::PhysicalStorageSubset* createPortRegistersOn (int id, MCHEmul::PhysicalStorage* ps) override
							{ return (new F6500::IO7501PortRegisters (id, ps)); }
	};
}

#endif
  
// End of the file
/*@}*/
