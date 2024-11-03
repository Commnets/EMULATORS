#include <VIC20/VIA1.hpp>
#include <VIC20/Memory.hpp>
#include <VIC20/OSIO.hpp>

// ---
bool VIC20::VIA1::initialize ()
{
	assert (memoryRef () != nullptr);

	// Gets the memory block dedicated to the CIA2
	if (!(_VIA1Registers = 
		dynamic_cast <VIC20::VIA1Registers*> (memoryRef () -> subset (_registersId))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	return (COMMODORE::VIA::initialize ());
}

// ---
void VIC20::VIA1::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	COMMODORE::VIA::processEvent (evnt, n);

	switch (evnt.id ())
	{
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
						? ((i > 0) ? 0x80 /** right/east = bit 7 */ : ((i < 0) ? 0x10 /** left/west = bit 4 */ : 0x00))
						: ((i > 0) ? 0x08 /** down/south = bit 3 */ : ((i < 0) ? 0x04 /** up/north = bit 2 */ : 0x00));

					ct++;
				}

				/** The full joystick status is saved,
					but bear in mind that VIA1 only takes care of every movement except right/east 
					that is managed through out VIA 2. */
				_VIA1Registers -> setJoystickStatus 
					((dr == 0x00) ? 0xff /** none connected. */ : _VIA1Registers -> joystickStatus () & ~dr);
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKBUTTONPRESSED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickButtonEvent> jb = 
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickButtonEvent> (evnt.data ());
				if (jb -> _joystickId != 0)
					break; // Only joystick 0 is allowed!

				_VIA1Registers -> setJoystickStatus (_VIA1Registers -> joystickStatus () & ~0x20 /** bit 5. */);
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKBUTTONRELEASED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickButtonEvent> jb = 
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickButtonEvent> (evnt.data ());
				if (jb -> _joystickId != 0)
					break; // Only joystick 0 is allowed!
	
				_VIA1Registers -> setJoystickStatus (_VIA1Registers -> joystickStatus () & ~0x20 /** bit 5. */);
			}

			break;

		default:
			break;
	}
}
