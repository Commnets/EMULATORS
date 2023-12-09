/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: Sound.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 09/12/2023 \n
 *	Description: The sound system linked to VIC20.
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_SOUND__
#define __VIC20_SOUND__

#include <CORE/incs.hpp>
#include <COMMODORE/incs.hpp>

namespace VIC20
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
