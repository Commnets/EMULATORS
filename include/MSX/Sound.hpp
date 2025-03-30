/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: Sound.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 08/03/2025 \n
 *	Description: The sound system linked to MSX	.
 *	Versions: 1.0 Initial
 */

#ifndef __MSX_SOUND__
#define __MSX_SOUND__

#include <CORE/incs.hpp>

namespace MSX
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
