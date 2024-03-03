#include <C264/C6529B1.hpp>
#include <C264/C6529B1Registers.hpp>
#include <C264/OSIO.hpp>

// ---
bool C264::C6529B1::initialize ()
{
	assert (memoryRef () != nullptr);

	// The parent is not initialized because the registers are not the parent ones...

	// Gets the memory block dedicated to this register...
	if (!(_C6529B1Registers =
		dynamic_cast <C264::C6529B1Registers*> (memoryRef () -> subset (C264::C6529B1Registers::_C6529B1REGS_SUBSET))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_C6529BRegisters = _C6529B1Registers;
	_C6529B1Registers -> initialize ();

	initializeKeyboardAndJoystickStatus ();

	return (true);
}

// ---
bool C264::C6529B1::simulate (MCHEmul::CPU* cpu)
{
	// Has the keyboard latched changed?
	if (latchChanged ()) // This method can be used once when the value changed...
	{ 
		unsigned char dr = 0;
		for (size_t i = 0; i < 7; i++)
			if (latchValue ().bit (i))
				dr |= _rev_keyboardStatusMatrix [i].value ();
		
		notify (MCHEmul::Event (_LACTCHEDVALUECHANGED, (unsigned int) ~dr));
	}

	// ...and the joysticks?
	if (_joyValueChanged [0]) // Only used once...
		notify (MCHEmul::Event (_JOYSTICK1VALUECHANGED, (unsigned int) _joystickStatus [0].value ()));
	if (_joyValueChanged [1]) // Only used once...
		notify (MCHEmul::Event (_JOYSTICK2VALUECHANGED, (unsigned int) _joystickStatus [1].value ()));

	return (true);
}

// ---
void C264::C6529B1::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	switch (evnt.id ())
	{
		case MCHEmul::InputOSSystem::_KEYBOARDKEYPRESSED:
			{
				const C264::InputOSSystem::Keystrokes& ks = 
					((C264::InputOSSystem*) n) -> keystrokesFor // Will depend on the implementation...
						(std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> (evnt.data ()) -> _key);
				if (!ks.empty ()) // The key has to be defined...
					for (const auto& j : ks)
						setKeyboardStatusMatrix (j.first, j.second, false);
			}

			break;

		case MCHEmul::InputOSSystem::_KEYBOARDKEYRELEASED:
			{
				const C264::InputOSSystem::Keystrokes& ks = 
					((C264::InputOSSystem*) n) -> keystrokesFor // Will depend on the implementation...
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

				size_t ct = 0; // Counts the axis...
				unsigned char dr = 0;
				for (size_t ct = 0; ct < jm ->_axisValues.size (); ct++)
					dr |= 1 << ((C264::InputOSSystem*) n) -> bitForJoystickAxis 
						(jm -> _joystickId, (int) ct, jm -> _axisValues [ct]);

				/** Saves the full status of the joystick. */
				setJoystickStatus 
					(jm -> _joystickId, (dr == 0x00) 
						? 0xff /** none connected. */ : joystickStatus (jm -> _joystickId) & ~dr);
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKBUTTONPRESSED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickButtonEvent> jb = 
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickButtonEvent> (evnt.data ());

				setJoystickStatus (jb -> _joystickId, 
					joystickStatus (jb -> _joystickId) & 
						(1 << ((C264::InputOSSystem*) n) -> bitForJoystickButton (jb -> _joystickId, jb -> _buttonId)));
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKBUTTONRELEASED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickButtonEvent> jb = 
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickButtonEvent> (evnt.data ());
	
				setJoystickStatus (jb -> _joystickId, 
					joystickStatus (jb -> _joystickId) & 
						~(1 << ((C264::InputOSSystem*) n) -> bitForJoystickButton (jb -> _joystickId, jb -> _buttonId)));
			}

			break;

		default:
			break;
	}
}
