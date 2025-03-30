/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: OSIO.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/08/2024 \n
 *	Description: The OSIO events any MSX (like) Computer.
 *	Versions: 1.0 Initial
 */

#ifndef __MSX_OSIO__
#define __MSX_OSIO__

#include <SINCLAIR/incs.hpp>

namespace MSX
{
	/** The MSX::InputOSSystem 
		It is very related with the por EF. \n
		The events that happen in the OS have to be transmitted to that port
		for them to be accesible for the rest of the computer elements. */
	class InputOSSystem final : public MCHEmul::InputOSSystem
	{
		public:
		static const unsigned int _ID = 201;

		using Keystroke = 
			std::pair <unsigned short /** bit at port A. */, unsigned short /** bit at port B. */>;
		using Keystrokes = std::vector <Keystroke>;

		InputOSSystem ();

		virtual void linkToChips (const MCHEmul::Chips& c) override;

		inline const Keystrokes& keystrokesFor (SDL_Scancode sc) const;

		private:
		// TODO: Describe the map of keystrokes
		using KeystrockesMap = std::map <SDL_Scancode, Keystrokes>;
		static const KeystrockesMap _MSXKEYS;
		static const Keystrokes _NOKEYSTROKES;
	};

	// ---
	inline const InputOSSystem::Keystrokes& InputOSSystem::keystrokesFor (SDL_Scancode sc) const
	{
		KeystrockesMap::const_iterator i = _MSXKEYS.find (sc);
		return ((i == _MSXKEYS.end ()) ? InputOSSystem::_NOKEYSTROKES : (*i).second);
	}
}

#endif
  
// End of the file
/*@}*/
