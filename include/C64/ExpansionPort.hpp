/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: ExpansionPort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 22/01/2023 \n
 *	Description:	Just to give name to the different PINS of the ExpansionPort.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_EXPANSIONPORT__
#define __C64_EXPANSIONPORT__

#include <COMMODORE/incs.hpp>

namespace C64
{
	class ExpansionIOPort : public COMMODORE::ExpansionIOPort
	{
		public:
		/** The name of the different PINS in the C64 implementation. */
		static const unsigned char _GAME = 8;
		static const unsigned char _EXROM = 9;

		ExpansionIOPort ()
			: COMMODORE::ExpansionIOPort ()
							{ }
	};
}

#endif
  
// End of the file
/*@}*/
