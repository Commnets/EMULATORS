#include <VIC20/VIA2.hpp>
#include <VIC20/Memory.hpp>
#include <VIC20/OSIO.hpp>

// ---
bool VIC20::VIA2::initialize ()
{
	assert (memoryRef () != nullptr);

	// Gets the memory block dedicated to the CIA2
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
			<< _PA.asString ()
			<< "], PortB:["
			<< _PB.asString () << "]\n";

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
						? ((i > 0) ? 0x80 /** right/east = bit 7 */ : ((i < 0) ? 0x10 /** left/west = bit 4 */ : 0))
						: ((i > 0) ? 0x08 /** down/south = bit 3 */ : ((i < 0) ? 0x04 /** up/north = bit 2 */ : 0));

					ct++;
				}

				/** Saves the full status of the joystick, 
					but bear in mind that VIA2 only takes care of the right/east movement. */
				_VIA2Registers -> setJoystickStatus 
					((dr == 0x00) ? 0xff /** none connected. */ : _VIA2Registers -> joystickStatus () & ~dr);
			}

			break;

			// The rest of the movements of the joystick are managed throught out VIA1...

		default:
			break;
	}
}
