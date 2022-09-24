/** \ingroup CPU */
/*@{*/

/**	
 *	@file	
 *	File: OSIO.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/06/2021 \n
 *	Description: To gather and manage the events in the windows system where the emulation is happening.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_OSIO__
#define __MCHEMUL_OSIO__

#include <CORE/IO.hpp>
#include <SDL_events.h>

namespace MCHEmul
{
	/** Represents the generation of inputs from the OS. */
	class InputOSSystem : public IODevice
	{
		public:
		InputOSSystem (int id, const Attributes& attrs = { })
			: IODevice (Type::_OUTPUT, id, attrs),
			  _quitRequested (false)
							{ }

		bool quitRequested () const
							{ return (_quitRequested); }

		virtual bool initialize () override;

		/** Algorithm, invoking the protected methods defined. */
		virtual bool simulate () override;

		protected:
		// To manage events related with the keyboard...
		/** What to do when a key has been pressed. */
		virtual void whenKeyPressed (SDL_Scancode) = 0;
		/** What to do when a key has been released. */
		virtual void whenKeyReleased (SDL_Scancode) = 0;

		// To manage events related with the joystick...
		using SDL_JoyAxisEvents = std::vector <SDL_JoyAxisEvent>;
		/** What to do when the joystick is moved.
			Take care that the events in the joystick have to be managed as a hole, because may of them
			can happen at the same time: e.g. when the joystick is moved in two axes simultaneosly. */
		virtual void whenJoystickMoved (const SDL_JoyAxisEvents&) = 0;
		/** What to do when the joystick button is pressed. */
		virtual void whenJoystickButtonPressed (SDL_JoyButtonEvent) = 0;
		/** What to do when the joystick button is released. */
		virtual void whenJoystickButtonReleased (SDL_JoyButtonEvent) = 0;

		protected:
		bool _quitRequested;
	};
}

#endif
  
// End of the file
/*@}*/
