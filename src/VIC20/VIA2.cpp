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

	// Just to observe the write actions aganst the datasette port...
	observe (&_PB);

	return (COMMODORE::VIA::initialize ());
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

		// A change in the datasette port input line (CA1) has been detected...
		// Notice that the value of the event is not taken into account
		// because just the change in the signal is detected!
		case MCHEmul::DatasetteIOPort::_READ:
			{
				_CA1.setValue (false); // The datasette is reading!
			}

			break;

		// When writting if becase the datasette port output line (PB3) has changed...
		case COMMODORE::VIAPortB::_VIAPORTN3ACTUALIZED:
			{
				notify (MCHEmul::Event (MCHEmul::DatasetteIOPort::_WRITE, evnt.value ())); // Write to the datasette...
			}

			break;

		default:
			break;
	}
}
