/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: OSIO.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/06/2021 \n
 *	Description: The OSIO events in the C64 are linked to the Chip CIA1 (@see C64::CIA1).
 *	Versions: 1.0 Initial
 */

#ifndef __C64_OSIO__
#define __C64_OSIO__

#include <COMMODORE/incs.hpp>

namespace C64
{
	class CIA1;

	/** The C64::InputOSSystem is very related with the chip CIA1. \n
		The events that happen in the OS have to be transmitted to that chip 
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
			Important to know about how C64 keyboard layout matches PC Keyboard: \n
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
		CIA1* _cia1;
		COMMODORE::VICII* _vicII;

		/** 
			How SDL keys are mapped into the chars map of the C64. \n
			That key will have an image in the status zone. \n
			$DC01 (read)	Bit 7		 Bit 6	 Bit 5		 Bit 4			 Bit 3	 Bit 2			Bit 1		 Bit 0 \n
			$DC00 (select)	================================================================================================= \n
			Byte 7			| RUNSTOP	| Q		| CLOGO		| SPACE			| 2		| CTRL			| LEFT		| 1			| \n
			Byte 6			| /			| UP	| =			| RIGHTSHIFT	| HOME	| ;				| *			| POUND		| \n
			Byte 5			| ,			| @		| :			| .				| -		| L				| P			| +			| \n
			Byte 4			| N			| O		| K			| M				| 0		| J				| I			| 9			| \n
			Byte 3			| V			| U		| H			| B				| 8		| G				| Y			| 7			| \n
			Byte 2			| X			| T		| F			| C				| 6		| D				| R			| 5			| \n
			Byte 1			| LEFTSHIFT	| E		| S			| Z				| 4		| A				| W			| 3			| \n
			Byte 0			| UP/DOWN	| F5	| F3		| F1			| F7	| LEFT/RIGHT	| RETURN	| DELETE	| \n
							================================================================================================= \n
			Take into account that the activity can be done in th eother way around...\n
			The important C64 keys in a Win32 standard keyboard are: \n
			LEFT CONTROL				= CTRL \n
			RIGHT CONTROL				= CTRL \n
			LEFT ALT					= STOP \n
			SHIFT + LEFT ALT			= RUN  \n
			TAB							= COMMODORE KEY \n
			LEFT BRACKET				= + \n
			RIGHT BRACKET				= *	\n
			APOSTROPHE					= :	\n
			BACKSLASH					= @	\n
			ESCAPE						= ARROW BACK \n
			GRAVE						= POUND \n
			ACCENT						= MINUS	\n
			OPEN EXCLAMATION			= EQUAL	\n
			OPEN BRACKET				= PLUS \n
			CLOSE BRACKET				= ASTERISC \n
			OPEN PARENTHESIS			= TWO POINTS \n
			CLOSE PARENTEHEIS			= ADDRESS \n
			COMMA						= COMMA \n
			PERIOD						= PERIOD \n
			MINUS						= SLASH \n
			SHIFT COMMA					= LESSER THAN \n
			SHIFT PERIOD				= GREATER THAN \n
			SHIFT MINUS					= QUESTION MARK \n
			Ñ							= POINT AND COMMA \n
		*/
		using KeystrockesMap = std::map <SDL_Scancode, Keystrokes>;
		static const KeystrockesMap _C64KEYS;
		static const Keystrokes _NOKEYSTROKES;
	};

	// ---
	inline const InputOSSystem::Keystrokes& InputOSSystem::keystrokesFor (SDL_Scancode sc) const
	{
		KeystrockesMap::const_iterator i = _C64KEYS.find (sc);
		return ((i == _C64KEYS.end ()) ? InputOSSystem::_NOKEYSTROKES : (*i).second);
	}
}

#endif
  
// End of the file
/*@}*/

