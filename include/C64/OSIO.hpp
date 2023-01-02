/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: Screen.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/06/2021 \n
 *	Description: The OSIO events in the C64 are linked to the Chip CIA1 (@see C64::CIA1).
 *	Versions: 1.0 Initial
 */

#ifndef __C64_OSIO__
#define __C64_OSIO__

#include <CORE/incs.hpp>

namespace C64
{
	class CIA1;

	/** The C64::InputOSSystem is very related with the chip CIA1. \n
		The events that happen in the OS have to be transmitted to that chip 
		for them to be accesible for the rest of the computer elements. */
	class InputOSSystem final : public MCHEmul::InputOSSystem
	{
		public:
		static const unsigned int _ID = 2;

		InputOSSystem (const MCHEmul::Attributes& attrs = { })
			: MCHEmul::InputOSSystem (_ID, attrs),
			  _cia1 (nullptr)
							{ }

		virtual void linkToChips (const MCHEmul::Chips& c) override;

		private:
		virtual void whenKeyPressed (SDL_Scancode k) override;
		virtual void whenKeyReleased (SDL_Scancode k) override;
		virtual void whenJoystickMoved (const MCHEmul::InputOSSystem::JoystickMovementMap& jm) override;
		virtual void whenJoystickButtonPressed (SDL_JoyButtonEvent jb) override;
		virtual void whenJoystickButtonReleased (SDL_JoyButtonEvent jb) override;

		private:
		CIA1* _cia1;

		/** 
			How SDL keys are mapped into the chars map of the C64.
			That key will have an image in the status zone. 
			$DC01 (read)	Bit 7		 Bit 6	 Bit 5		 Bit 4			 Bit 3	 Bit 2	 Bit 1		 Bit 0
			$DC00 (select)	=========================================================================================
			Byte 7			| STOP		| Q		| CLOGO		| SPACE			| 2		| CTRL	| LEFT		| 1			|
			Byte 6			| /			| UP	| =			| RIGHTSHIFT	| HOME	| ;		| *			| POUND		|
			Byte 5			| ,			| @		| :			| .				| -		| L		| P			| +			|
			Byte 4			| N			| O		| K			| M				| 0		| J		| I			| 9			|
			Byte 3			| V			| U		| H			| B				| 8		| G		| Y			| 7			|
			Byte 2			| X			| T		| F			| C				| 6		| D		| R			| 5			|
			Byte 1			| LEFTSHIFT	| E		| S			| Z				| 4		| A		| W			| 3			|
			Byte 0			| DOWN		| F5	| F3		| F1			| F7	| RIGHT	| RETURN	| DELETE	|
							=========================================================================================
		*/
		using KeyMPos = std::pair <unsigned short /** The byte. */, unsigned short /** The bit. */>;
		static const std::map <SDL_Scancode, const std::vector <KeyMPos>> _C64KEYS;
	};
}

#endif
  
// End of the file
/*@}*/

