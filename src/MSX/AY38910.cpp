#include <MSX/AY38910.hpp>

// ---
MSX::AY38910::AY38910 (
		GENERALINSTRUMENTS::AY38910Registers* reg,
		GENERALINSTRUMENTS::AY38910LibWrapper* w)
	: GENERALINSTRUMENTS::AY38910 (reg, w),
	  _joystickStatus { MCHEmul::UByte (0x3f), MCHEmul::UByte (0x3f) },
	  _JISKeyboard (false),
	  _cassetteInput (true)
{
	setClassName ("MSXAY38910");
}

// ---
void MSX::AY38910::setRegister (
	unsigned char rId, const MCHEmul::UByte& v)
{
	// The selected internal register must be captured before the generic bus access.
	// A write to the address port itself can change that selection.
	unsigned char sR = registers () -> selectedControlRegister ();
	GENERALINSTRUMENTS::AY38910::setRegister (rId, v);

	// Port B selects and drives the joystick connectors. A direction change in R7
	// can also make its previously latched outputs electrically visible.
	if (rId == 0x01 && (sR == 0x07 || sR == 0x0f))
		refreshPortAInput ();
}

// ---
bool MSX::AY38910::initialize ()
{
	if (!GENERALINSTRUMENTS::AY38910::initialize ())
		return (false);

	_joystickStatus [0] = _joystickStatus [1] = MCHEmul::UByte (0x3f);
	_JISKeyboard = false;
	_cassetteInput = true;
	refreshPortAInput ();

	return (true);
}

// ---
void MSX::AY38910::processEvent (
	const MCHEmul::Event& evnt, MCHEmul::Notifier*)
{
	switch (evnt.id ())
	{
		case MCHEmul::InputOSSystem::_JOYSTICKMOVED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickMovementEvent> jm =
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickMovementEvent> (evnt.data ());
				if (jm != nullptr)
					updateJoystickMovement (*jm);
			}

			break;

		case MCHEmul::InputOSSystem::_JOYSTICKBUTTONPRESSED:
		case MCHEmul::InputOSSystem::_JOYSTICKBUTTONRELEASED:
			{
				std::shared_ptr <MCHEmul::InputOSSystem::JoystickButtonEvent> jb =
					std::static_pointer_cast <MCHEmul::InputOSSystem::JoystickButtonEvent> (evnt.data ());
				if (jb != nullptr)
					updateJoystickButton (*jb);
			}

			break;

		default:
			break;
	}
}

// ---
void MSX::AY38910::updateJoystickMovement (
	const MCHEmul::InputOSSystem::JoystickMovementEvent& jm)
{
	if (jm._joystickId < 0 || jm._joystickId > 1)
		return;

	unsigned char dr = 0x0f;
	if (!jm._axisValues.empty ())
		dr &= (jm._axisValues [0] < 0)
			? (unsigned char) ~0x04
			: ((jm._axisValues [0] > 0) ? (unsigned char) ~0x08 : 0xff);
	if (jm._axisValues.size () > 1)
		dr &= (jm._axisValues [1] < 0)
			? (unsigned char) ~0x01
			: ((jm._axisValues [1] > 0) ? (unsigned char) ~0x02 : 0xff);

	// Axis events contain the current movement state. Preserve the two trigger bits.
	_joystickStatus [(size_t) jm._joystickId] =
		MCHEmul::UByte ((_joystickStatus [(size_t) jm._joystickId].value () & 0x30) | dr);
	refreshPortAInput ();
}

// ---
void MSX::AY38910::updateJoystickButton (
	const MCHEmul::InputOSSystem::JoystickButtonEvent& jb)
{
	if (jb._joystickId < 0 || jb._joystickId > 1 ||
		jb._buttonId < 0 || jb._buttonId > 1)
		return;

	_joystickStatus [(size_t) jb._joystickId].setBit (
		4 + (size_t) jb._buttonId, !jb._on);
	refreshPortAInput ();
}

// ---
void MSX::AY38910::refreshPortAInput ()
{
	using IOPort = GENERALINSTRUMENTS::AY38910Registers::IOPort;

	const MCHEmul::UByte& pB = registers () -> ioPortOutputLatch (IOPort::_B);
	size_t j = pB.bit (6) ? 1 : 0;
	MCHEmul::UByte pA (0xff);

	if (registers () -> ioPortIsOutput (IOPort::_B))
	{
		pA = _joystickStatus [j];

		// Every joystick switch connects its input to pin 8. With pin 8 high,
		// closing a switch therefore does not produce an active-low input.
		if (pB.bit (4 + j))
			pA = MCHEmul::UByte (pA.value () | 0x3f);

		// Pins 6 and 7 can also be driven from PSG port B. A low output wins
		// over the corresponding trigger input; a high output releases it.
		if (!pB.bit (j << 1))
			pA.setBit (4, false);
		if (!pB.bit ((j << 1) + 1))
			pA.setBit (5, false);
	}

	pA.setBit (6, _JISKeyboard);
	pA.setBit (7, _cassetteInput);
	registers () -> setIOPortInputValue (IOPort::_A, pA);
}
