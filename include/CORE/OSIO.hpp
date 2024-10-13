/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: OSIO.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
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
		// Different events.
		/** Key pressed. \n
			The parameter of the event is the scand_code, 
			this is the position of the key within the keyboard not necessary the key code that depends on
			the configuration of the keyboard (country). */
		static const unsigned int _KEYBOARDKEYPRESSED			= 150;
		/** Key released. */
		static const unsigned int _KEYBOARDKEYRELEASED			= 151;
		/** Joystick moved. \n
			The parametes are the joystick id and the the axis id moved. */
		static const unsigned int _JOYSTICKMOVED				= 152;
		/** Joystick button pressed. \n
			The parametes are the joystick id and the the axis id moved. */
		static const unsigned int _JOYSTICKBUTTONPRESSED		= 153;
		static const unsigned int _JOYSTICKBUTTONRELEASED		= 154;
		/** Mouse moved. \n
			The parametes are the mouse id and the the axis id moved. */
		static const unsigned int _MOUSEMOVED					= 155;
		/** Mouse button pressed. \n
			The parametes are the mouse id and the the axis id moved. */
		static const unsigned int _MOUSEBUTTONPRESSED			= 156;
		static const unsigned int _MOUSEBUTTONRELEASED			= 157;

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
			std::vector <int> _axisValues; // Really there will be usually two, 
										   // but this structure is easir to manipulate!
		};

		/** To identify an event in the joystick related with the buttons. */
		struct JoystickButtonEvent : public Event::Data
		{
			JoystickButtonEvent (int jId, int bId, bool o)
				: _joystickId (jId), _buttonId (bId), _on (o)
							{ }

			int _joystickId;
			int _buttonId;
			bool _on; // true when pressed, false when released...
		};

		/** To identify an event in the mouse related with the movement. */
		struct MouseMovementEvent : public Event::Data
		{
			MouseMovementEvent (int id, int x, int y)
				: _mouseId (id), 
				  _x (x), _y (y)
							{ }

			int _mouseId;
			int _x, _y;
		};

		/** To identify an event in the mouse related with the buttons. */
		struct MouseButtonEvent : public Event::Data
		{
			MouseButtonEvent (int jId, int bId, bool o)
				: _mouseId (jId), _buttonId (bId), _on (o)
							{ }

			int _mouseId;
			int _buttonId;
			bool _on; // true when pressed, false when released...
		};

		InputOSSystem (int id, const Attributes& attrs = { });

		/** Assign a number to a joystick id. */
		void addConversionJoystick (int id, int n)
							{ _conversionJoystickMap [id] = n; _joyMovementMap = { }; }
		void setConversionJoystickMap (std::map <int, int>&& cJM)
							{ _conversionJoystickMap = std::move (cJM); _joyMovementMap = { }; }

		bool quitRequested () const
							{ return (_quitRequested); }

		/** Not possible to connect any peripheral. */
		virtual bool connectPeripheral (IOPeripheral* p) override
							{ return (false); }

		virtual bool initialize () override;

		/** Algorithm, invoking the protected methods defined. */
		virtual bool simulate (CPU* cpu) override;

		/**
		  *	Adding to parent fields:
		  * JOYSTICK	= Joystick id connected with the name in parenthesis. \n
		  */
		virtual InfoStructure getInfoStructure () const override;

		/** To know which is the name of a scan code. 
			It can be usefull in debug functions. */
		static std::string nameOfkey (SDL_Scancode sc)
							{ return (std::string (SDL_GetScancodeName (sc))); }

		protected:
		// To manage events related with the keyboard...
		/** What to do when a key has been pressed. */
		void whenKeyPressed (SDL_Scancode kc) // No reference is needed as SDL_Scancode is a enum (basic type)
							{ notify (Event (_KEYBOARDKEYPRESSED, 0,
								std::shared_ptr <Event::Data> ((Event::Data*) new KeyboardEvent (kc, true)))); }
		/** What to do when a key has been released. */
		void whenKeyReleased (SDL_Scancode kc)
							{ notify (Event (_KEYBOARDKEYRELEASED, 0,
								std::shared_ptr <Event::Data> ((Event::Data*) new KeyboardEvent (kc, false)))); }

		// To manage events related with the joystick...
		/** The JoystickMovementMap is a map with different entries per joystick id,
			and with a vector with the values of different axis per entry. */
		using JoystickMovementMap = std::map <int /** joystick id. */, std::vector <int> /** vector of axis. */>;
		/** The situation of the different axis in the different joystick loaded in the system are tracked 
			in the "simulate" method. When they changed this method is then invoked. */
		void whenJoystickMoved (const JoystickMovementMap& jm)
							{ for (auto& i : jm) 
								notify (Event (_JOYSTICKMOVED, 0,
									std::shared_ptr <Event::Data> ((Event::Data*) 
										new JoystickMovementEvent (joystickEquivalentId (i.first), i.second)))); }
		/** What to do when the joystick button is pressed. */
		void whenJoystickButtonPressed (const SDL_JoyButtonEvent& jb)
							{ notify (Event (_JOYSTICKBUTTONPRESSED, 0,
								std::shared_ptr <Event::Data> ((Event::Data*) 
									new JoystickButtonEvent (joystickEquivalentId (jb.which), jb.button, true)))); }
		/** What to do when the joystick button is released. */
		void whenJoystickButtonReleased (const SDL_JoyButtonEvent& jb)
							{ notify (Event (_JOYSTICKBUTTONRELEASED, 0,
								std::shared_ptr <Event::Data> ((Event::Data*) 
									new JoystickButtonEvent (joystickEquivalentId (jb.which), jb.button, false)))); }

		// To manage the events related with the mouse...
		/** The situation of the different axis in the different mouses loaded in the system are tracked 
			in the "simulate" method. When they changed this method is then invoked. */
		void whenMouseMoved (const SDL_MouseMotionEvent& mm)
							{ notify (Event (_MOUSEMOVED, 0,
									std::shared_ptr <Event::Data> ((Event::Data*) 
										new MouseMovementEvent (mm.which, mm.x, mm.y)))); }
		/** What to do when the mouse button is pressed. */
		void whenMouseButtonPressed (const SDL_MouseButtonEvent& mb)
							{ notify (Event (_MOUSEBUTTONPRESSED, 0,
								std::shared_ptr <Event::Data> ((Event::Data*) 
									new MouseButtonEvent (mb.which, mb.button, true)))); }
		/** What to do when the joystick button is released. */
		void whenMouseButtonReleased (const SDL_MouseButtonEvent& mb)
							{ notify (Event (_MOUSEBUTTONRELEASED, 0,
								std::shared_ptr <Event::Data> ((Event::Data*) 
									new MouseButtonEvent (mb.which, mb.button, false)))); }

		private:
		// Joystick movements...
		/** Just to control all the joystick events at the same time. */
		using SDL_JoyAxisEvents = std::vector <SDL_JoyAxisEvent>;
		void treatJoystickMovementEvents (SDL_JoyAxisEvents&& js);

		// Implementation
		/** To find the joystick equivalent id after passing through the conversion. */
		int joystickEquivalentId (int a) const
							{ std::map <int, int>::const_iterator i = _conversionJoystickMap.find (a);
							  return ((i == _conversionJoystickMap.end ()) ? a : (*i).second); }

		protected:
		bool _quitRequested;
		using SDLJoysticks = std::vector <SDL_Joystick*>;
		SDLJoysticks _joysticks;
		std::map <int, int> _conversionJoystickMap; // between the internal id and the one to be notified...
		/** The clock to control the frenquency to read the keyboard. */
		Clock _clock;

		// Implementation
		/** To track the movement of the josyticks in the different directions. */
		JoystickMovementMap _joyMovementMap;
	};
}

#endif
  
// End of the file
/*@}*/
