#include <C64/VICII.hpp>

// ---
void C64::VICII_JoystickEvent::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* nfier)
{
	switch (evnt.id ())
	{
		// The light pen is also active when the fire button is pressed in the joystick "1"!
		// The LP signal (one of the pins of the VICII) is connected to the game port 1 pin 4 also...
		case MCHEmul::InputOSSystem::_JOYSTICKBUTTONPRESSED:
			{
				if (std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickButtonEvent> 
						(evnt.data ()) -> _joystickId == 0) // Any button!!
					_VICIIRegisters -> setLigthPenActive (true);
			}

			break;

		// ...and disactive in the opposite situation!
		case MCHEmul::InputOSSystem::_JOYSTICKBUTTONRELEASED:
			{
				if (std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickButtonEvent> 
						(evnt.data ()) -> _joystickId == 0) // Any button!
					_VICIIRegisters -> setLigthPenActive (false);
			}

			break;

		default:
			break;
	}
}

// ---
bool C64::VICII_PAL::initialize ()
{ 
	bool result = COMMODORE::VICII_PAL::initialize ();
	if (result)
		VICII_JoystickEvent::linkToVICIIRegisters (COMMODORE::VICII_PAL::_VICIIRegisters);

	return (result);
}

// ---
void C64::VICII_PAL::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* nfier)
{ 
	COMMODORE::VICII_PAL::processEvent (evnt, nfier); 
							  
	// Whatever the result of the previous management was...
	VICII_JoystickEvent::processEvent (evnt, nfier);
}

// ---
bool C64::VICII_NTSC::initialize ()
{ 
	bool result = COMMODORE::VICII_NTSC::initialize ();
	if (result)
		VICII_JoystickEvent::linkToVICIIRegisters (COMMODORE::VICII_NTSC::_VICIIRegisters);

	return (result);
}		

// ---
void C64::VICII_NTSC::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* nfier)
{ 
	COMMODORE::VICII_NTSC::processEvent (evnt, nfier); 
							  
	// Whatever the result of the previous management was...
	VICII_JoystickEvent::processEvent (evnt, nfier);
}
