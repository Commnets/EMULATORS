/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: Sound.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 13/01/2021 \n
 *	Description: The sound system linked to C64.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_SOUND__
#define __C64_SOUND__

#include <CORE/incs.hpp>
#include <COMMODORE/incs.hpp>

namespace C64
{
	class SoundSystem : public MCHEmul::SoundSystem
	{
		public:
		static const int _ID = 202;

		SoundSystem ();
	};
}

#endif
  
// End of the file
/*@}*/
