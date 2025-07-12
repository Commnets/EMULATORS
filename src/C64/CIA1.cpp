#include <C64/CIA1.hpp>
#include <C64/Memory.hpp>
#include <C64/OSIO.hpp>
#include <C64/DatasettePort.hpp>

// ---
C64::CIA1::CIA1 ()
	: CIA (_ID, CIA1Registers::_CIA1_SUBSET, F6500::IRQInterrupt::_ID),
	  _CIA1Registers (nullptr),
	  _sid (nullptr),
	  _lastKeyPressed (""), _lastKeyReleased ("")
{ 
	setClassName ("CIA1"); 
}

// ---
bool C64::CIA1::initialize ()
{
	assert (memoryRef () != nullptr);

	// Gets the memory block dedicated to the CIA1
	// That has special uses with the keyboard and joysticks...
	if (!(_CIA1Registers = 
		dynamic_cast <C64::CIA1Registers*> (memoryRef () -> subset (_registersId))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	return (COMMODORE::CIA::initialize ());
}

// ---
bool C64::CIA1::simulate (MCHEmul::CPU* cpu)
{
	bool result = COMMODORE::CIA::simulate (cpu);

	_IFDEBUG debugCIA1Cycle ();

	return (result);
}

// ---
void C64::CIA1::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	COMMODORE::CIA::processEvent (evnt, n);

	switch (evnt.id ())
	{
		case MCHEmul::InputOSSystem::_KEYBOARDKEYPRESSED:
			{
				if (deepDebugActive ())
					_lastKeyPressed = MCHEmul::InputOSSystem::nameOfkey 
						(std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> (evnt.data ()) -> _key);

				const C64::InputOSSystem::Keystrokes& ks = ((C64::InputOSSystem*) n) -> keystrokesFor
					(std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> (evnt.data ()) -> _key);
				if (!ks.empty ()) // The key has to be defined...
					for (const auto& j : ks)
						_CIA1Registers -> setKeyboardStatusMatrix (j.first, j.second, false);
			}

			break;

		case MCHEmul::InputOSSystem::_KEYBOARDKEYRELEASED:
			{
				if (deepDebugActive ())
					_lastKeyReleased = MCHEmul::InputOSSystem::nameOfkey 
						(std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> (evnt.data ()) -> _key);

				const C64::InputOSSystem::Keystrokes& ks = ((C64::InputOSSystem*) n) -> keystrokesFor
				(std::static_pointer_cast <MCHEmul::InputOSSystem::KeyboardEvent> (evnt.data ()) -> _key);
				if (!ks.empty ()) // The key has to be defined...
					for (const auto& j : ks)
						_CIA1Registers -> setKeyboardStatusMatrix (j.first, j.second, true);
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKMOVED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickMovementEvent> jm = 
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickMovementEvent> (evnt.data ());
				if ((jm -> _joystickId != 0 && jm -> _joystickId != 1) || jm -> _axisValues.size () > 2)
					break; // Only joysticks 0 y 1 are allowed and never more than 2 axis each!

				unsigned char dr = 0;
				size_t ct = 0;
				for (auto i : jm -> _axisValues)
				{
					dr |= (ct == 0) // The x axis first...
						? ((i > 0) ? 0x08 /** right. */ : ((i < 0) ? 0x04 /** left. */ : 0x00))
						: ((i > 0) ? 0x02 /** down. */ : ((i < 0) ? 0x01 /** up. */ : 0x00));

					ct++;
				}

				_CIA1Registers -> setJoystickStatusAtPort ((size_t) jm -> _joystickId,
					((dr == 0x00) 
						? 0xff /** none connected. */ 
						: _CIA1Registers -> joystickStatusAtPort ((size_t) jm -> _joystickId) & ~dr));
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKBUTTONPRESSED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickButtonEvent> jb = 
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickButtonEvent> (evnt.data ());
				if (jb -> _joystickId != 0 && jb -> _joystickId != 1)
					break; // Only joysticks 0 y 1 are allowed!

				if (_CIA1Registers -> isPaddleConnectedAtPort ((size_t) jb -> _joystickId))
					// Buttons 0 and 1 don't do the same action in paddle view!
					// But the boundary is converted to a limit less than 2...
					_CIA1Registers -> setPaddleFireButtonStatus
						((size_t) jb -> _joystickId, (size_t) jb -> _buttonId % 2 /** 0 or 1. */, true);
				else
					// Any button does the same action in joystick view!
					_CIA1Registers -> setJoystickStatusAtPort ((size_t) jb -> _joystickId, 
						_CIA1Registers -> joystickStatusAtPort ((size_t) jb -> _joystickId) & ~0x10);
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKBUTTONRELEASED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickButtonEvent> jb = 
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickButtonEvent> (evnt.data ());
				if (jb -> _joystickId != 0 && jb -> _joystickId != 1)
					break; // Only joysticks 0 y 1 are allowed, but button number is not controlled!
	
				if (_CIA1Registers -> isPaddleConnectedAtPort ((size_t) jb -> _joystickId))
					_CIA1Registers -> setPaddleFireButtonStatus 
						((size_t) jb -> _joystickId, (size_t) jb -> _buttonId % 2 /** 0 or 1. */, false);
				else
					_CIA1Registers -> setJoystickStatusAtPort ((size_t) jb -> _joystickId,
						_CIA1Registers -> joystickStatusAtPort ((size_t) jb -> _joystickId) | 0x10);
			}

			break;

		// A change in the datsette port has been detected...
		// Notice that the value of the event is not taken into account
		// because just the change in the signal is detected!
		case C64::DatasetteIOPort::_READ:
			{
				_CIA1Registers -> setFlagLineInterruptRequested (true);
			}

			break;

		default:
			break;
	}
}

// ---
void C64::CIA1::debugCIA1Cycle ()
{
	assert (_deepDebugFile != nullptr);

	_deepDebugFile -> writeCompleteLine (className (), _lastClockCycles, "Additional Info",
		{ { "Key matrix", 
			_CIA1Registers -> keyboardMatrixAsString () + " [P:" +
			_lastKeyPressed + ",R:" + _lastKeyReleased + "]" },
		  { "Joystick registers", 
			_CIA1Registers -> joystickStatusAsString () } });
}
