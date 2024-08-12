/** \ingroup ZXSPECTRUM */
/*@{*/

/**	
 *	@file	
 *	File: OSIO.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/08/2024 \n
 *	Description: The OSIO events any ZXSpectrum (like) Computer.
 *	Versions: 1.0 Initial
 *	based on: http://www.breakintoprogram.co.uk/hardware/computers/zx-spectrum/keyboard
 */

#ifndef __ZXSPECTRUM_OSIO__
#define __ZXSPECTRUM_OSIO__

#include <SINCLAIR/incs.hpp>

namespace ZXSPECTRUM
{
	class ULA;

	/** The ZXSPECTRUM::InputOSSystem 
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
		ULA* _ula;

		/** 
			How SDL keys are mapped into the chars map of the SINCLAIR keyboard. \n
			Bit				  D0	  D1	  D2	  D3	  D4 \n
			Port			========================================= \n
			$FEFE (0)		| SHIFT	| Z		| X		| C		| V		| \n
			$FDFE (1)		| A		| S		| D		| F		| G		| \n
			$FBFE (2)		| Q		| W		| E		| R		| T		| \n
			$F7FE (3)		| 1		| 2		| 3		| 4		| 5		| \n
			$EFFE (4)		| 0		| 9		| 8		| 7		| 6		| \n
			$DFFE (5)		| P		| O		| I		| U		| Y		| \n
			$BFFE (6)		| ENTER	| L		| K		| J		| H		| \n
			$7FFE (7)		| SPACE	| Sym	| M		| N		| B		| \n
							=========================================
			Sym is emulated using the Left ALT key...
		*/
		using KeystrockesMap = std::map <SDL_Scancode, Keystrokes>;
		static const KeystrockesMap _ZX81KEYS;
		static const Keystrokes _NOKEYSTROKES;
	};

	// ---
	inline const InputOSSystem::Keystrokes& InputOSSystem::keystrokesFor (SDL_Scancode sc) const
	{
		KeystrockesMap::const_iterator i = _ZX81KEYS.find (sc);
		return ((i == _ZX81KEYS.end ()) ? InputOSSystem::_NOKEYSTROKES : (*i).second);
	}
}

#endif
  
// End of the file
/*@}*/

