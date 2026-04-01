/** \ingroup SINCLAIR */
/*@{*/

/**	
 *	@file	
 *	File: Printer.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 30/03/2026 \n
 *	Description: To convert a ZX Code into an ASCII one
 *				 There are differences between computers in the SINCLAIR series.
 *	Versions: 1.0 Initial
 */

#ifndef __SINCLAIR_ZXCODETOASCII__
#define __SINCLAIR_ZXCODETOASCII__

#include <CORE/incs.hpp>

namespace SINCLAIR
{
		/** Very simple class that muct be overloaded, 
			for a specific implementation of the computer. */
		class ZXCodeToASCII
		{
			public:
			virtual unsigned char convert (unsigned char) = 0;
		};
}

#endif
  
// End of the file
/*@}*/

