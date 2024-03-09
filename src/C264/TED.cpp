#include <C264/TED.hpp>
#include <C264/TEDRegisters.hpp>

// ---
const MCHEmul::RasterData C264::TED_PAL::_VRASTERDATA (260, 274, 3, 202, 245, 259, 312, 4, 4);
const MCHEmul::RasterData C264::TED_PAL::_HRASTERDATA 
	(376, 384, 432, 295, 343, 375, 456 /** For everyting to run, it has to be divisible by 8. */, 7, 9);
const MCHEmul::RasterData C264::TED_NTSC::_VRASTERDATA (235, 249, 3, 202, 220, 234, 262, 4, 4);
const MCHEmul::RasterData C264::TED_NTSC::_HRASTERDATA 
	(376, 384, 432, 295, 343, 375, 456 /** For everyting to run, it has to be divisible by 8. */, 7, 9);

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
bool C264::TED::simulate (MCHEmul::CPU* cpu)
{
	COMMODORE::TED::simulate (cpu);

	if (_TEDRegisters -> romAccessChanged ()) // Changes the value...
		notify (MCHEmul::Event (_ROMACCESSCHANGED, 
			(unsigned int) _TEDRegisters -> hiromSelected () ? 1 : 0));

	return (true);
}

// ---
void C264::TED::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	switch (evnt.id ())
	{
		// When there is a value latched for the keyboard...
		case COMMODORE::C6529B::_LACTCHEDVALUECHANGED:
			{ 
				_TEDRegisters -> 
					setKeyboardPins (MCHEmul::UByte ((unsigned char) evnt.value ()));
			}

			break;

		// When the joystick 1 have changed...
		case C264::C6529B1::_JOYSTICK1VALUECHANGED:
			{ 
				_TEDRegisters -> 
					setJoystickPins (0, MCHEmul::UByte ((unsigned char) evnt.value ()));
			}

			break;

		// When the joystick 2 have changed...
		case C264::C6529B1::_JOYSTICK2VALUECHANGED:
			{ 
				_TEDRegisters -> 
					setJoystickPins (1, MCHEmul::UByte ((unsigned char) evnt.value ()));
			}

			break;

		default:
			break;
	}
}

// ---
C264::TED_PAL::TED_PAL (int vV, MCHEmul::SoundLibWrapper* wS)
	: C264::TED (
		 _VRASTERDATA, _HRASTERDATA, vV, wS,
		{ { "Name", "TED" },
		  { "Code", "7360/8360 for PAL" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1984" } })
{
	// Nothing else to do...
}

// ---
C264::TED_NTSC::TED_NTSC (int vV, MCHEmul::SoundLibWrapper* wS)
	: C264::TED (
		 _VRASTERDATA, _HRASTERDATA, vV, wS,
		{ { "Name", "TED" },
		  { "Code", "7360/8360 for NTSC" },
		  { "Manufacturer", "Commodore Business Machines CBM" },
		  { "Year", "1984" } })
{
	// Nothing else to do...
}
