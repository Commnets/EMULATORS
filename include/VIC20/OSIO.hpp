/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: OSIO.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 16/12/2023 \n
 *	Description: The OSIO events in the VIC20 are linked to the Chip VIA (@see VIC20::VIA1).
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_OSIO__
#define __VIC20_OSIO__

#include <COMMODORE/incs.hpp>

namespace VIC20
{
	class VIA1;
	class VIA2;

	/** The VIC20::InputOSSystem 
		is very related with the two VIA chips: VIA1 (@see VIA1) & VIA2 (@see VIA2). \n
		The events that happen in the OS have to be transmitted to those chips 
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

		/** The ScanCode is based en where the key is localed in a standard US PC Keyboard. \n
			So e.g, the ; is where the Ñ is in a spanish layout. \n
			Important to know about how VIC20 keyboard layout matches PC Keyboard: \n
			LEFT CONTROL = CTRL \n
			RIGHT CONTROL = CTRL \n
			LEFT ALT = STOP \n
			SHIFT + LEFT ALT = RUN \n
			TAB = COMMODORE KEY \n
			LEFT BRACKET = + \n
			RIGHT BRACKET = * \n
			APOSTROPHE = : \n
			BACKSLASH = @ \n
			ESCAPE = ARROW BACK \n
			GRAVE = POUND \n
			ACCENT = MINUS \n
			OPEN EXCLAMATION = EQUAL \n
			OPEN BRACKET = PLUS \n
			CLOSE BRACKET = ASTERIC \n
			OPEN PARENTHESIS = TWO POINTS \n
			CLOSE PARENTEHEIS = ADDRESS \n
			COMMA = COMMA \n
			PERIOD = PERIOD \n
			MINUS = SLASH \n
			SHIFT COMMA = LESSER THAN \n
			SHIFT PERIOD = GREATER THAN \n
			SHIFT MINUS = QUESTION MARK \n
			SUPR = UP ARROW \n
			Ñ = POINT AND COMMA. */
		inline const Keystrokes& keystrokesFor (SDL_Scancode sc) const;

		private:
		VIA1* _via1;
		VIA2* _via2;
		COMMODORE::VICI* _vicI;

		/** 
			How SDL keys are mapped into the chars map of the VIC20. \n
			That key will have an image in the status zone. \n
			$9121 (read)	Bit 7		 Bit 6	 Bit 5		 Bit 4			 Bit 3	 Bit 2	 Bit 1		 Bit 0 \n
			$9120 (select)	========================================================================================= \n
			Byte 7			| F7		| HOME		| -			| 0			| 8		| 6		| 4			| 2			| \n
			Byte 6			| F5		| UP		| @			| O			| U		| T		| E			| Q			| \n
			Byte 5			| F3		| =			| :			| K			| H		| F		| S			| CLOGO		| \n
			Byte 4			| F1		| RIGHTSHIFT| .			| M			| B		| C		| Z			| SPACE		| \n
			Byte 3			| LEFT/RIGHT| /			| ,			| N			| V		| X		| LEFTSHIFT	| RUNSTOP	| \n
			Byte 2			| UP/DOWN	| ;			| L			| J			| G		| D		| A			| CTRL		| \n
			Byte 1			| RETURN	| *			| P			| I			| Y		| R		| W			| LEFT		| \n
			Byte 0			| DELETE	| POUND		| +			| 9			| 7		| 5		| 3			| 1			| \n
							========================================================================================= \n
			Take into account that the activity can be done in the other way around...\n
		*/
		using KeystrockesMap = std::map <SDL_Scancode, Keystrokes>;
		static const KeystrockesMap _VIC20KEYS;
		static const Keystrokes _NOKEYSTROKES;
	};

	// ---
	inline const InputOSSystem::Keystrokes& InputOSSystem::keystrokesFor (SDL_Scancode sc) const
	{
		KeystrockesMap::const_iterator i = _VIC20KEYS.find (sc);
		return ((i == _VIC20KEYS.end ()) ? InputOSSystem::_NOKEYSTROKES : (*i).second);
	}
}

#endif
  
// End of the file
/*@}*/

