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
#include <CORE/Clock.hpp>
#include <SDL_events.h>

namespace MCHEmul
{
	/** Represents the generation of inputs from the OS. */
	class InputOSSystem : public IODevice
	{
		public:
		/** Different events. */
		static const unsigned int _KEYBOARDKEYPRESSED			= 101;
		static const unsigned int _KEYBOARDKEYRELEASED			= 102;
		static const unsigned int _JOYSTICKMOVED				= 103;
		static const unsigned int _JOYSTICKBUTTONPRESSED		= 104;
		static const unsigned int _JOYSTICKBUTTONRELEASED		= 105;

		/** To identify an event in the keyboard. */
		struct KeyboardEvent : public Event::Data
		{
			KeyboardEvent (SDL_Scancode k, bool o)
				: _key (k), _on (o)
							{ }

			SDL_Scancode _key;
			bool _on; // true when pressed, false when released...
		};

		/** To identify an event in the joystick related with the movement. */
		struct JoystickMovementEvent : public Event::Data
		{
			JoystickMovementEvent (int id, const std::vector <int>& av)
				: _joystickId (id), _axisValues (av)
							{ }

			int _joystickId;
			std::vector <int> _axisValues;
		};

		/** To identify an event in the joystick related with the buttons. */
		struct JoystickButtonEvent : public Event::Data
		{
			JoystickButtonEvent (int id, bool o)
				: _joystickId (id), _on (o)
							{ }

			int _joystickId;
			bool _on; // true when pressed, false when released...
		};

		InputOSSystem (int id, const Attributes& attrs = { });

		bool quitRequested () const
							{ return (_quitRequested); }

		virtual bool initialize () override;

		/** Algorithm, invoking the protected methods defined. */
		virtual bool simulate () override;

		protected:
		// To manage events related with the keyboard...
		/** What to do when a key has been pressed. */
		void whenKeyPressed (SDL_Scancode kc) // No reference is needed as SDL_Scancode is a enum (basic type)
							{ notify (Event (_KEYBOARDKEYPRESSED, 
								std::shared_ptr <Event::Data> ((Event::Data*) new KeyboardEvent (kc, true)))); }
		/** What to do when a key has been released. */
		void whenKeyReleased (SDL_Scancode kc)
							{ notify (Event (_KEYBOARDKEYRELEASED, 
								std::shared_ptr <Event::Data> ((Event::Data*) new KeyboardEvent (kc, false)))); }

		// To manage events related with the joystick...
		/** The JoystickMovementMap is a map with different entries per joystick id,
			and with a vector with the values of different axis per entry. */
		using JoystickMovementMap = std::map <int /** joystick id. */, std::vector <int> /** vector of axis. */>;
		/** The situation of the different axis in the different joystick loaded in the system are tracked 
			in the "simulate" method. When they changed this method is then invoked. */
		void whenJoystickMoved (const JoystickMovementMap& jm)
							{ for (auto& i : jm ) 
								notify (Event (_JOYSTICKMOVED,
									std::shared_ptr <Event::Data> ((Event::Data*) 
										new JoystickMovementEvent (i.first, i.second)))); }
		/** What to do when the joystick button is pressed. */
		void whenJoystickButtonPressed (const SDL_JoyButtonEvent& jb)
							{ notify (Event (_JOYSTICKBUTTONPRESSED, 
								std::shared_ptr <Event::Data> ((Event::Data*) new JoystickButtonEvent (jb.button, true)))); }
		/** What to do when the joystick button is released. */
		void whenJoystickButtonReleased (const SDL_JoyButtonEvent& jb)
							{ notify (Event (_JOYSTICKBUTTONRELEASED, 
								std::shared_ptr <Event::Data> ((Event::Data*) new JoystickButtonEvent (jb.button, false)))); }

		private:
		/** Just to control all the joystick events at the same time. */
		using SDL_JoyAxisEvents = std::vector <SDL_JoyAxisEvent>;
		void treatJoystickEvents (SDL_JoyAxisEvents&& js);

		protected:
		bool _quitRequested;
		using SDLJoysticks = std::vector <SDL_Joystick*>;
		SDLJoysticks _joysticks;
		/** The clock to control the frenquency to read the keyboard. */
		Clock _clock;

		// Implementation
		/** To track the movement of the josytick in the different directions. */
		JoystickMovementMap _movementMap;
	};
}

#endif
  
// End of the file
/*@}*/
