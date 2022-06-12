/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: Screen.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/06/2021 \n
 *	Description: The screen linked to the C64.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_OSIO__
#define __C64_OSIO__

#include <CPU/incs.hpp>

namespace C64
{
	class InputOSSystem final : public MCHEmul::InputOSSystem
	{
		public:
		static const unsigned int _ID = 2;

		InputOSSystem (const MCHEmul::Attributes& attrs = { })
			: MCHEmul::InputOSSystem (_ID, attrs)
							{ }

		protected:
		virtual void whenKeyPressed (SDL_Keycode) override;
		virtual void whenKeyReleased (SDL_Keycode) override;
	};
}

#endif
  
// End of the file
/*@}*/

