/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: Sound.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/12/2023 \n
 *	Description: The sound system linked to C264 series computer.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_SOUND__
#define __C264_SOUND__

#include <CORE/incs.hpp>
#include <COMMODORE/incs.hpp>

namespace C264
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
