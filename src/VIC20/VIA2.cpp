#include <VIC20/VIA2.hpp>
#include <VIC20/Memory.hpp>
#include <VIC20/OSIO.hpp>

// ---
bool VIC20::VIA2::initialize ()
{
	assert (memoryRef () != nullptr);

	// Gets the memory block dedicated to the VIA2
	// That has special uses with the keyboard and joysticks...
	if (!(_VIA2Registers = 
		dynamic_cast <VIC20::VIA2Registers*> (memoryRef () -> subset (_registersId))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	return (COMMODORE::VIA::initialize ());
}

// ---
bool VIC20::VIA2::simulate (MCHEmul::CPU* cpu)
{
	if (deepDebugActive ())
		*_deepDebugFile
			// Where
			<< "VIA2\t" 
			// When
			<< std::to_string (_lastClockCycles) << "\t" // clock cycles at that point
			// What
			<< "Info cycle\t\t"
			// Data
			<< "PortA:["
			<< std::to_string (_VIA2Registers -> outputRegisterA ()) << "," 
			<< std::to_string (_VIA2Registers -> dataPortADir ()) << ","
			<< _VIA2Registers -> portA ().asString (MCHEmul::UByte::OutputFormat::_HEXA, 0)
			<< "], PortB:["
			<< std::to_string (_VIA2Registers -> outputRegisterB ()) << "," 
			<< std::to_string (_VIA2Registers -> dataPortBDir ()) << ","
			<< _VIA2Registers -> portB ().asString (MCHEmul::UByte::OutputFormat::_HEXA, 0) << "]\n";

	return (COMMODORE::VIA::simulate (cpu));
}

// ---
void VIC20::VIA2::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	COMMODORE::VIA::processEvent (evnt, n);

	switch (evnt.id ())
	{
		case MCHEmul::InputOSSystem::_KEYBOARDKEYPRESSED:
			{
				const VIC20::InputOSSystem::Keystrokes& ks = ((VIC20::InputOSSystem*) n) -> keystrokesFor
					(std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> (evnt.data ()) -> _key);
				if (!ks.empty ()) // The key has to be defined...
					for (const auto& j : ks)
						_VIA2Registers -> setKeyboardStatusMatrix (j.first, j.second, false);
			}

			break;

		case MCHEmul::InputOSSystem::_KEYBOARDKEYRELEASED:
			{
				const VIC20::InputOSSystem::Keystrokes& ks = ((VIC20::InputOSSystem*) n) -> keystrokesFor
				(std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> (evnt.data ()) -> _key);
				if (!ks.empty ()) // The key has to be defined...
					for (const auto& j : ks)
						_VIA2Registers -> setKeyboardStatusMatrix (j.first, j.second, true);
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKMOVED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickMovementEvent> jm = 
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickMovementEvent> (evnt.data ());
				if ((jm -> _joystickId != 0) || jm -> _axisValues.size () > 2)
					break; // Only joysticks 0 is allowed and never more than 2 axis!

				unsigned char dr = 0;
				size_t ct = 0;
				for (auto i : jm -> _axisValues)
				{
					dr |= (ct == 0) // The x axis first...
						? ((i > 0) ? 8 /** right. */ : ((i < 0) ? 4 /** left. */ : 0))
						: ((i > 0) ? 2 /** down. */ : ((i < 0) ? 1 /** up. */ : 0));

					ct++;
				}

				_VIA2Registers -> setJoystick1Status 
					((dr == 0x00) ? 0xff /** none connected. */ : _VIA2Registers -> joystick1Status () & ~dr);
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKBUTTONPRESSED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickButtonEvent> jb = 
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickButtonEvent> (evnt.data ());
				if (jb -> _joystickId != 0)
					break; // Only joystick 0 is allowed!

				_VIA2Registers -> setJoystick1Status (_VIA2Registers -> joystick1Status () & ~0x10);
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKBUTTONRELEASED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickButtonEvent> jb = 
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickButtonEvent> (evnt.data ());
				if (jb -> _joystickId != 0)
					break; // Only joystick 0 is allowed!
	
				_VIA2Registers -> setJoystick1Status (_VIA2Registers -> joystick1Status () | 0x10);
			}

			break;
			
		// TODO

		default:
			break;
	}
}
