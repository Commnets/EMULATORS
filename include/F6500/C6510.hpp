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

namespace F6500
{
	/** The Chip CPU 6510 */
	class C6510 : public C6500
	{
		public:
		C6510 (int id);
	};
}

#endif
  
// End of the file
/*@}*/
