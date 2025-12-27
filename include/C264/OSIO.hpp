/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: OSIO.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 04/02/2024 \n
 *	Description: The OSIO events in the C264 series computer are linked to two chips: 
 *				 TED (@see C264::TED) and C6529B (@see C264::C6529B) with no special nickname.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_OSIO__
#define __C264_OSIO__

#include <COMMODORE/incs.hpp>

namespace C264
{
	class C6529B1; 
	class TED;

	/** The C264::InputOSSystem  \n
		is very related with the 6529B1 chip (@see C6529B1). */
	class InputOSSystem final : public MCHEmul::InputOSSystem
	{
		public:
		using Keystroke = 
			std::pair <unsigned short /** bit at port A. */, unsigned short /** bit at port B. */>;
		using Keystrokes = std::vector <Keystroke>;

		static const unsigned int _ID = 201;

		InputOSSystem ()
			: MCHEmul::InputOSSystem (_ID),
			  _C6529B1 (nullptr), _TED (nullptr)
							{ }

		/** link to C6529B1 chip. */
		virtual void linkToChips (const MCHEmul::Chips& c) override;

		/** The ScanCode is based en where the key is localed in a standard US PC Keyboard. \n
			So e.g, the ; is where the Ñ is in a spanish layout. \n
			Important to know about how C264 series keyboard layout matches PC keyboard layout (@see doc): \n
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
		size_t bitForJoystickAxis (int jId, int aId, int aV) const
							{ return ((aId == 0) // x axis
								? ((aV > 0) ? 3 : 2) /** 3 = right, 2 = left */
								: ((aV > 0) ? 1 : 0)); /** 1 = down, 0 = up */ }
		size_t bitForJoystickButton (int jId, int bId) const
							{ return (jId == 0 ? 6 : 7); /** The bit 6 in joystick 0 and 7 in the joystick 1. */ }

		private:
		/** 
			How SDL keys are mapped into the chars map of the C264. \n
			That key will have an image in the status zone. \n
			C6529B $FF30 (select)	Bit 7		 Bit 6		Bit 5	Bit 4	Bit 3	Bit 2	Bit 1	Bit 0		   \n
			TED: $FD08 (read)		============================================================================== \n
			Byte 7					| RUN/STOP	| /			| ,		| N		| V		| X		| SHIFT	| @			 | \n
			Byte 6					| Q			| +			| -		| O		| U		| T		| E		| F3		 | \n
			Byte 5					| CLOGO		| =			| :		| K		| H		| F		| S		| F2		 | \n
			Byte 4					| SPACE		| ESCAPE	| .		| M		| B		| C		| Z		| F1		 | \n
			Byte 3					| 2			| RIGHT		| UP	| 0		| 8		| 6		| 4		| HELP		 | \n
			Byte 2					| CTRLS		| ;			| L		| J		| G		| D		| A		| POUND		 | \n
			Byte 1					| CLR/HOME	| *			| P		| I		| Y		| R		| W		| RETURN	 | \n
			Byte 0					| 1			| LEFT		| DOWN	| 9		| 7		| 5		| 3		| INS/DEL	 | \n
									============================================================================== \n
			How the keyboard motherboard pins are connected both to TED and to C6529 is different.
		*/
		using KeystrockesMap = std::map <SDL_Scancode, Keystrokes>;
		static const KeystrockesMap _C264KEYS;
		static const Keystrokes _NOKEYSTROKES;

		// Implementation
		/** The linked chip. */
		C6529B1* _C6529B1;
		TED* _TED;
	};

	// ---
	inline const InputOSSystem::Keystrokes& InputOSSystem::keystrokesFor (SDL_Scancode sc) const
	{ 
		KeystrockesMap::const_iterator i = _C264KEYS.find (sc);
		
		return ((i == _C264KEYS.end ())
			? InputOSSystem::_NOKEYSTROKES : (*i).second); 
	}
}

#endif
  
// End of the file
/*@}*/

