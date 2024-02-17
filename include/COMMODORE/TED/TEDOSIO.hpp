/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: TEDOSIO.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 04/02/2024 \n
 *	Description: In a TED system there is a core relation between TED and the input system. \n
 *				 The keyboard/joystick status is read in the register 8 ($ff08) of the TED (@see TED).
 *				 So there must be at least a base implementation of the IOSystem.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_TEDOSIO__
#define __COMMODORE_TEDOSIO__

#include <COMMODORE/incs.hpp>

namespace COMMODORE
{
	class TED;

	/** The only think not defined is which row/coulm of the keyboard matrix 
		is in every position. It will depend on the implementation. */
	class TEDInputOSSystem : public MCHEmul::InputOSSystem
	{
		public:
		using Keystroke = 
			std::pair <unsigned short /** bit at port A. */, unsigned short /** bit at port B. */>;
		using Keystrokes = std::vector <Keystroke>;

		TEDInputOSSystem (int id);

		/** link to TED. */
		virtual void linkToChips (const MCHEmul::Chips& c) override;

		/** It will depend on the implementation. \n
			It returns the location in the keyboard matrix of the scan code selected. */
		virtual const Keystrokes& keystrokesFor (SDL_Scancode sc) const = 0;
		/** It will also depend on the implementation. \n
			Receives the joystickId, the axisId and the data for the movement. \n
			It returns the bit affected by a specific movement in a joystick axis. */
		virtual size_t bitForJoystickAxis (int jId, int aId, int aV) const = 0;
		/** Same for a button. \n
			Receives the joystickId. */
		virtual size_t bitForJoystickButton (int jId, int bId) const = 0;

		protected:
		TED* _ted;
	};
}

#endif
  
// End of the file
/*@}*/

