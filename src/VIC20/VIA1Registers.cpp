#include <VIC20/VIA1Registers.hpp>

// ---
VIC20::VIA1Registers::VIA1Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: COMMODORE::VIARegisters (_VIA1_SUBSET, ps, pp, a, s)
{ 
	setClassName ("VIA1Registers");

	initializeInternalValues ();
}

// ---
void VIC20::VIA1Registers::setJoystickStatus (unsigned char js)
{
	_joystickStatus = js;

	refreshPortAInputs ();
}

// ---
void VIC20::VIA1Registers::setTapeSensePressed (bool v)
{
	_tapeSensePressed = v;

	refreshPortAInputs ();
}

// ---
void VIC20::VIA1Registers::refreshPortAInputs ()
{
	assert (_PA != nullptr);

	// Only joystick PA2..PA5 and cassette sense PA6 belong to these sources.
	// Preserve the external IEC levels and PA7; DDR resolution belongs to the port.
	MCHEmul::UByte pins = (_PA -> externalPins () & 0x83) | (_joystickStatus & 0x3c);
	pins.setBit (6, !_tapeSensePressed);
	_PA -> setExternalPins (pins);
}

// ---
void VIC20::VIA1Registers::initializeInternalValues ()
{
	_joystickStatus = 0xff; // No switches clicked, no fire buttons pressed...
	_tapeSensePressed = false; // NO key pressed in the datasette...

	COMMODORE::VIARegisters::initializeInternalValues ();

	if (_CA1 == nullptr ||
		_CA2 == nullptr ||
		_CB1 == nullptr ||
		_CB2 == nullptr ||
		_T1  == nullptr ||
		_T2  == nullptr ||
		_SR  == nullptr ||
		_PA  == nullptr ||
		_PB  == nullptr)
		return;
	
	// VIC-20 INITVIA default for VIA1.
	setValue (0x02, MCHEmul::UByte (0x00)); // DDRB: all input
	setValue (0x03, MCHEmul::UByte (0x80)); // DDRA: PA7 output, PA0..PA6 input
	// All values up!...
	setValue (0x00, MCHEmul::UByte (0xff));
	setValue (0x01, MCHEmul::UByte (0xff));

	refreshPortAInputs ();
}

