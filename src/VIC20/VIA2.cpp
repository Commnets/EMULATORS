#include <VIC20/VIA2.hpp>
#include <VIC20/Memory.hpp>
#include <VIC20/OSIO.hpp>

// ---
VIC20::VIA2::VIA2 ()
	: VIA (_ID, VIA2Registers::_VIA2_SUBSET, F6500::IRQInterrupt::_ID),
	  _VIA2Registers (nullptr)
{ 
	setClassName ("VIA2"); 

	_joystickStatus = 0xff; // No switches clicked, no fire buttons pressed...
	for (size_t i = 0; i < 8; i++)
		_keyboardStatusMatrix [i] = _rev_keyboardStatusMatrix [i] = MCHEmul::UByte::_FF; // No keys pressed...
}

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

	// The standard simulation fails for VIA 2
	// becaus ethe status of the keyboard and joystick are not taken into account...
	bool result = COMMODORE::VIA::simulate (cpu);
	// So if everything goes ok, redo that values...
	if (result)
	{
		unsigned char m = 0x01;
		unsigned char dt = MCHEmul::UByte::_0;
		MCHEmul::UByte o = _PB.OR ();
		if (_T2.runMode () == COMMODORE::VIATimer::RunMode::_ONESHOOTSIGNAL ||
			_T2.runMode () == COMMODORE::VIATimer::RunMode::_CONTINUOUSSIGNAL)
			o.setBit (7, _PB.p7 ());
		unsigned char msk = (o.value () | ~_PB.DDR ().value ());
		for (size_t i = 0; i < 8; m <<= 1, i++)
			if ((~msk & m) != 0x00)
				dt |= ~_rev_keyboardStatusMatrix [i].value (); // 1 if clicked...
		_PA.setPortValue ((_PA.OR ().value () | ~_PA.DDR ().value ()) & ~dt);

		m = 0x01;
		dt = MCHEmul::UByte::_0;
		msk = (_PA.OR ().value () | ~_PA.DDR ().value ()) & _joystickStatus;
		for (size_t i = 0; i < 8; m <<= 1, i++)
			if ((~msk & m) != 0x00)
				dt |= ~_keyboardStatusMatrix [i].value ();  // 1 if clicked...
		_PB.setPortValue ((_PB.OR ().value () | ~_PB.DDR ().value ()) & ~dt);
	}

	return (result);
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
						setKeyboardStatusMatrix (j.first, j.second, false);
			}

			break;

		case MCHEmul::InputOSSystem::_KEYBOARDKEYRELEASED:
			{
				const VIC20::InputOSSystem::Keystrokes& ks = ((VIC20::InputOSSystem*) n) -> keystrokesFor
				(std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> (evnt.data ()) -> _key);
				if (!ks.empty ()) // The key has to be defined...
					for (const auto& j : ks)
						setKeyboardStatusMatrix (j.first, j.second, true);
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

				setJoystickStatus 
					((dr == 0x00) ? 0xff /** none connected. */ : joystickStatus () & ~dr);
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKBUTTONPRESSED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickButtonEvent> jb = 
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickButtonEvent> (evnt.data ());
				if (jb -> _joystickId != 0)
					break; // Only joystick 0 is allowed!

				setJoystickStatus (joystickStatus () & ~0x10);
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKBUTTONRELEASED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickButtonEvent> jb = 
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickButtonEvent> (evnt.data ());
				if (jb -> _joystickId != 0)
					break; // Only joystick 0 is allowed!
	
				setJoystickStatus (joystickStatus () | 0x10);
			}

			break;

		default:
			break;
	}
}
