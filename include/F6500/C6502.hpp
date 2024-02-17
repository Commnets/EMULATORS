/** \ingroup F6500 */
/*@{*/

/**	
 *	@file	
 *	File: C6502.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 05/11/2023 \n
 *	Description: The main CPU of the VIC20.
 *	Versions: 1.0 Initial
 */

#ifndef __F6500_C6502__
#define __F6500_C6502__

#include <F6500/C6500.hpp>

namespace F6500
{
	/** The Chip CPU 6502 */
	class C6502 : public C6500
	{
		public:
		C6502 (int id);
	};
}

#endif
  
// End of the file
/*@}*/
