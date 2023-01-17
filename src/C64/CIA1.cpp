#include <C64/CIA1.hpp>
#include <C64/Memory.hpp>
#include <C64/OSIO.hpp>

// ---
bool C64::CIA1::initialize ()
{
	assert (memoryRef () != nullptr);

	// Gets the memory block dedicated to the CIA1
	// That has special uses with the keyboard and joysticks...
	if (!(_CIA1Registers = 
		dynamic_cast <C64::CIA1Registers*> (memoryRef () -> subset (C64::CIA1Registers::_CIA1_SUBSET))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	return (COMMODORE::CIA::initialize ());
}

// ---
void C64::CIA1::processEvent (MCHEmul::Event&& evnt, MCHEmul::Notifier* n)
{
	switch (evnt.id ())
	{
		case MCHEmul::InputOSSystem::_KEYBOARDKEYPRESSED:
			{
				const C64::InputOSSystem::Keystrokes& ks = ((C64::InputOSSystem*) n) -> keystrokesFor
					(std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> (evnt.data ()) -> _key);
				if (!ks.empty ()) // The key has to be defined...
					for (const auto& j : ks)
						_CIA1Registers -> setKeyboardStatusMatrix (j.first, j.second, false);
			}

			break;

		case MCHEmul::InputOSSystem::_KEYBOARDKEYRELEASED:
			{
				const C64::InputOSSystem::Keystrokes& ks = ((C64::InputOSSystem*) n) -> keystrokesFor
				(std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> (evnt.data ()) -> _key);
				if (!ks.empty ()) // The key has to be defined...
					for (const auto& j : ks)
						_CIA1Registers -> setKeyboardStatusMatrix (j.first, j.second, true);
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKMOVED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickMovementEvent> jm = 
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickMovementEvent> (evnt.data ());
				if ((jm -> _joystickId != 0 && jm -> _joystickId != 1) || jm -> _axisValues.size () > 2)
					break; // Only joysticks 0 y 1 are allowed and never more than 2 axis each!

				unsigned char dr = 0;
				size_t ct = 0;
				for (auto i : jm -> _axisValues)
				{
					dr |= (ct == 0) // The x axis first...
						? ((i > 0) ? 8 /** right. */ : ((i < 0) ? 4 /** left. */ : 0))
						: ((i > 0) ? 2 /** down. */ : ((i < 0) ? 1 /** up. */ : 0));

					ct++;
				}

				if (jm -> _joystickId == 0)
				{ 
					// The events on the joystick 1 are set on the same place than the keyboard...
					_CIA1Registers -> setJoystick1InputPending (dr != 0x00);
					for (size_t i = 0; i < 8; i++)
						_CIA1Registers -> setKeyboardStatusMatrix (i,
							(dr == 0) ? 0xff : _CIA1Registers -> keyboardStatusMatrix (i) & (0xff - dr));
				}
				else
				{
					// The movement of the joystick 2...
					// it is simplier!
					_CIA1Registers -> setJoystick2Status 
						((dr == 0) ? 0xff : _CIA1Registers -> joystick2Status () & (0xff - dr));
				}
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKBUTTONPRESSED:
			{
				// Only two buttons are allowed...
				if (std::static_pointer_cast 
						<MCHEmul::InputOSSystem::JoystickButtonEvent> (evnt.data ()) -> _joystickId == 0)
				{
					// The events on the joystick 1 are set on the same place than the keyboard...
					for (size_t i = 0; i < 8; i++)
						_CIA1Registers -> setKeyboardStatusMatrix (i, _CIA1Registers -> keyboardStatusMatrix (i) & 
							(0xff - 0x10 /** bit 4 clear when on. */));
				}
				else
				if (std::static_pointer_cast 
						<MCHEmul::InputOSSystem::JoystickButtonEvent> (evnt.data ()) -> _joystickId == 1)
					_CIA1Registers -> setJoystick2Status (_CIA1Registers -> joystick2Status () & 
						(0xff /** 0 means switch on. */ - 0x10));
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKBUTTONRELEASED:
			{
				// Only two joysticks are allowed...
				if (std::static_pointer_cast 
					<MCHEmul::InputOSSystem::JoystickButtonEvent> (evnt.data ()) -> _joystickId == 0)
				{
					// The events on the joystick 1 are set on the same place than the keyboard...
					for (size_t i = 0; i < 8; i++)
						_CIA1Registers -> setKeyboardStatusMatrix (i, _CIA1Registers -> keyboardStatusMatrix (i) | 0x10);
				}
				else
					if (std::static_pointer_cast 
						<MCHEmul::InputOSSystem::JoystickButtonEvent> (evnt.data ()) -> _joystickId == 1)
							_CIA1Registers -> setJoystick2Status (_CIA1Registers -> joystick2Status () | 0x10);
			}

			break;

		default:
			break;
	}
}
