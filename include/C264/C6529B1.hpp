/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: C6529B1.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 25/02/2024 \n
 *	Description: Specific implementation of the 6529 chip in C264 (the one taking care of the keyboard).
 *	Versions: 1.0 Initial
 */

#ifndef __C264_C6529B1__
#define __C264_C6529B1__

#include <COMMODORE/incs.hpp>

namespace C264
{
	/** The important thing of this chip is just their registers (@see C6529Registers). */
	class C6529B1 final : public COMMODORE::C6529B
	{
		public:
		static const int _ID = 200;
		
		C6529B1 ()
			: COMMODORE::C6529B (_ID)
							{ }
	};
}

#endif
  
// End of the file
/*@}*/
