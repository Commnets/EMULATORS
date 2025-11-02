#include <C264/TED.hpp>
#include <C264/TEDRegisters.hpp>
#include <C264/OSIO.hpp>

// ---
C264::TED::TED (int intId, const MCHEmul::RasterData& vd, const MCHEmul::RasterData& hd,
		int vV, MCHEmul::SoundLibWrapper* sW, const MCHEmul::Attributes& attrs)
	: COMMODORE::TED (intId, vd, hd, vV, sW, attrs),
	  _TEDRegisters (nullptr)
{
	// Nothing else to do...
}

// ---
bool C264::TED::initialize ()
{
	if (!COMMODORE::TED::initialize ())
		return (false);

	if (!(_TEDRegisters = 
		dynamic_cast <C264::TEDRegisters*> (memoryRef () -> subset (COMMODORE::TEDRegisters::_TEDREGS_SUBSET))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	return (true);
}

// ---
void C264::TED::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	switch (evnt.id ())
	{
		// Whn a change in the port is detected, it is just kept under the TED info...
		// ...it will taken into account later, when the TED info was reached!
		case C264::C6529B1::_PORTVALUECHANGED:
			{
				_TEDRegisters -> setKeyboardEntry (MCHEmul::UByte ((unsigned char) evnt.value ()));
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKMOVED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickMovementEvent> jm = 
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickMovementEvent> (evnt.data ());

				size_t ct = 0; // Counts the axis...
				MCHEmul::UByte dr = MCHEmul::UByte::_0;
				for (size_t ct = 0; ct < jm -> _axisValues.size (); ct++)
					dr.setBit (((C264::InputOSSystem*) n) -> bitForJoystickAxis 
						(jm -> _joystickId, (int) ct, jm -> _axisValues [ct]), true);
				_TEDRegisters -> setJoystickStatus (jm -> _joystickId, dr); // If there is no axis pressed, then a 0 is set...
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKBUTTONPRESSED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickButtonEvent> jb = 
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickButtonEvent> (evnt.data ()); // Cannot be null...
				_TEDRegisters -> setJoystickStatusBit (jb -> _joystickId, 
					((C264::InputOSSystem*) n) -> bitForJoystickButton (jb -> _joystickId, jb -> _buttonId), true);
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKBUTTONRELEASED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickButtonEvent> jb = 
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickButtonEvent> (evnt.data ()); // Cannot be null
				_TEDRegisters -> setJoystickStatusBit (jb -> _joystickId, 
					((C264::InputOSSystem*) n) -> bitForJoystickButton (jb -> _joystickId, jb -> _buttonId), false);
			}

			break;

		default:
			break;
	}
}

// ---
C264::TED_PAL::TED_PAL (int intId, int vV, MCHEmul::SoundLibWrapper* wS)
	: C264::TED (intId,
		 COMMODORE::TED_PAL::_VRASTERDATA, COMMODORE::TED_PAL::_HRASTERDATA, vV, wS,
		{ { "Name", "TED" },
		  { "Code", "7360/8360 for PAL" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1984" } })
{
	// Nothing else to do...
}

// ---
C264::TED_NTSC::TED_NTSC (int intId, int vV, MCHEmul::SoundLibWrapper* wS)
	: C264::TED (intId,
		 COMMODORE::TED_NTSC::_VRASTERDATA, COMMODORE::TED_NTSC::_HRASTERDATA, vV, wS,
		{ { "Name", "TED" },
		  { "Code", "7360/8360 for NTSC" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1984" } })
{
	// Nothing else to do...
}
