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
bool VIC20::VIA1::simulate (MCHEmul::CPU* cpu)
{
	if (deepDebugActive ())
		*_deepDebugFile
			// Where
			<< "VIA1\t" 
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
						? ((i > 0) ? 0x80 /** right = bit 7 */ : ((i < 0) ? 0x10 /** left = bit 4 */ : 0x00))
						: ((i > 0) ? 0x08 /** down = bit 3 */ : ((i < 0) ? 0x04 /** up = bit 2 */ : 0x00));

					ct++;
				}

				/** The full joystick status is saved, but VIA1 will oly take care of the right switch (bit 7). */
				_VIA1Registers -> setJoystickStatus 
					((dr == 0x00) ? 0xff /** none connected. */ : _VIA1Registers -> joystickStatus () & ~dr);
			}

			break;

		default:
			break;
	}
}
