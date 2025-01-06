/** \ingroup ZX81 */
/*@{*/

/**	
 *	@file	
 *	File: OSIO.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 25/03/2024 \n
 *	Description: The OSIO events any ZX81 (like) Computer.
 *	Versions: 1.0 Initial
 */

#ifndef __ZX81_OSIO__
#define __ZX81_OSIO__

#include <SINCLAIR/incs.hpp>

namespace ZX81
{
	class ULA;

	/** The ZX81::InputOSSystem 
		it is very related with the port EF. \n
		The events that happen in the OS have to be transmitted to that port
		for them to be accesible for the rest of the computer elements. \n
		The matrx is the same for any king of ZX81 computer like. 
		The main difference is the way the ROM interprets any keystroke. */
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
			$7FFE (7)		| SPACE	| .		| M		| N		| B		| \n
							=========================================
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

