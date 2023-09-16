
#include <C64/CIA1.hpp>
#include <C64/Memory.hpp>
#include <C64/OSIO.hpp>
#include <C64/DatasettePort.hpp>

// ---
bool C64::CIA1::initialize ()
{
	assert (memoryRef () != nullptr);

	// Gets the memory block dedicated to the CIA1
	// That has special uses with the keyboard and joysticks...
	if (!(_CIA1Registers = 
		dynamic_cast <C64::CIA1Registers*> (memoryRef () -> subset (_registersId))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	return (COMMODORE::CIA::initialize ());
}

// ---
void C64::CIA1::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	COMMODORE::CIA::processEvent (evnt, n);

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
					_CIA1Registers -> setJoystick1Status 
						((dr == 0x00) ? 0xff /** none connected. */ : _CIA1Registers -> joystick1Status () & ~dr);
				else
					_CIA1Registers -> setJoystick2Status 
						((dr == 0x00) ? 0xff : _CIA1Registers -> joystick2Status () & ~dr);
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKBUTTONPRESSED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickButtonEvent> jb = 
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickButtonEvent> (evnt.data ());
				if (jb -> _joystickId != 0 && jb -> _joystickId != 1)
					break; // Only joysticks 0 y 1 are allowed!
	
				if (jb -> _joystickId == 0)
					_CIA1Registers -> setJoystick1Status (_CIA1Registers -> joystick1Status () & ~0x10);
				else
					_CIA1Registers -> setJoystick2Status (_CIA1Registers -> joystick2Status () & ~0x10);
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKBUTTONRELEASED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickButtonEvent> jb = 
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickButtonEvent> (evnt.data ());
				if (jb -> _joystickId != 0 && jb -> _joystickId != 1)
					break; // Only joysticks 0 y 1 are allowed!
	
				// Only two joysticks are allowed...
				if (jb -> _joystickId == 0)
					_CIA1Registers -> setJoystick1Status (_CIA1Registers -> joystick1Status () | 0x10);
				else
					_CIA1Registers -> setJoystick2Status (_CIA1Registers -> joystick2Status () | 0x10);
			}

			break;

		case C64::DatasetteIOPort::_READ:
			// Activate the interrupt as a value has been received...
			// It doesn't matter whether the value is 1 or 0, the interrupt is always activated...
			_CIA1Registers -> setValue (0x0d, 
				_CIA1Registers -> readValue (0x0d) & 0xef | 0x10 /** Activate bit 4 = FLAG interrupt. */);
			break;

		default:
			break;
	}
}
