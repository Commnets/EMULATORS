/** \ingroup CORE */
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
			  _quitRequested (false),
			  _joysticks (),
			  _movementMap ()
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
		/** The JoystickMovementMap is a map with different entries per joystick id,
			and with a vector with the values of different axis per entry. */
		using JoystickMovementMap = std::map <int /** joystick id. */, std::vector <int> /** vector of axis. */>;
		/** The situation of the different axis in the different joystick loaded in the system are tracked 
			in the "simulate" method. When they changed this method is then invoked. */
		virtual void whenJoystickMoved (const JoystickMovementMap&) = 0;
		/** What to do when the joystick button is pressed. */
		virtual void whenJoystickButtonPressed (SDL_JoyButtonEvent) = 0;
		/** What to do when the joystick button is released. */
		virtual void whenJoystickButtonReleased (SDL_JoyButtonEvent) = 0;

		private:
		/** Just to control all the joystick events at the same time. */
		using SDL_JoyAxisEvents = std::vector <SDL_JoyAxisEvent>;
		void treatJoystickEvents (const SDL_JoyAxisEvents&);

		protected:
		bool _quitRequested;

		using SDLJoysticks = std::vector <SDL_Joystick*>;
		SDLJoysticks _joysticks;

		// Implementation
		/** To track the movement of the josytick in the different directions. */
		JoystickMovementMap _movementMap;
	};
}

#endif
  
// End of the file
/*@}*/
