/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: C6529B.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 09/12/2023 \n
 *	Description: Very simple chip that preset the info to read the keyboard (@see OSIO classes).
 *	Versions: 1.0 Initial
 */

#ifndef __C264_C6529B__
#define __C264_C6529B__

#include <CORE/incs.hpp>
#include <COMMODORE/incs.hpp>

namespace C264
{
	class C6529B : public MCHEmul::Chip
	{
		public:
		static const int _ID = 200;
		
		C6529B (int id);

		// TODO
	};
}

#endif
  
// End of the file
/*@}*/
