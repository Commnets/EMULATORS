#include <VIC20/VIA1Registers.hpp>

// ---
VIC20::VIA1Registers::VIA1Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: COMMODORE::VIARegisters (_VIA1_SUBSET, ps, pp, a, s)
{ 
	setClassName ("VIA1Registers");

	initializeInternalValues ();
}

// ---
const MCHEmul::UByte& VIC20::VIA1Registers::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	if (_CA1 == nullptr ||
		_CA2 == nullptr ||
		_CB1 == nullptr ||
		_CB2 == nullptr ||
		_T1  == nullptr ||
		_T2  == nullptr ||
		_SR  == nullptr ||
		_PA  == nullptr ||
		_PB  == nullptr)
		return (_lastValueRead);

	size_t pp = p % 0x10;

	switch (pp)
	{
		// VIA1 Port B
		case 0x00:
			{
				const MCHEmul::UByte ddr = _PB -> DDR ();
				MCHEmul::UByte out = _PB -> OR ();

				// Timer 1 can drive PB7 only when PB7 is output...
				if ((ddr & 0x80) != 0x00)
				{
					// ...and the way of Timer1 works allows it...
					if (_T1 -> runMode () == COMMODORE::VIATimer::RunMode::_ONESHOOTSIGNAL ||
						_T1 -> runMode () == COMMODORE::VIATimer::RunMode::_CONTINUOUSSIGNAL)
						out = _PB -> p7 () ? (out | 0x80) : (out & ~0x80);
				}

				// Live external pins for user port / RS-232.
				// Default: pull-up/high.
				MCHEmul::UByte pins = 0xff;
				// TODO:
				// Apply external RS-232/user-port input states:
				// PB7 DSR, PB6 CTS, PB4 DCD, PB3 RI, PB0 SIN.
				//
				// Example:
				// if (!_rs232DSR) pins &= ~0x80;
				// if (!_rs232CTS) pins &= ~0x40;
				// if (!_rs232DCD) pins &= ~0x10;
				// if (!_rs232RI ) pins &= ~0x08;
				// if (!_rs232SIN) pins &= ~0x01;

				MCHEmul::UByte input = pins;
				// If you model input latching inside VIAPort, this part should
				// use the port's latched input value instead.
				if (_PB -> latchIR ())
					input = _PB -> valueLatched ();

				// Sets the new value...
				_PB -> setPortValue ((out & ddr) | (input & ~ddr));
				// Reading ORB affects CB1/CB2 interrupt flags.
				result = _PB -> value (true);
			}

			break;

		// VIA1 Port A
		case 0x01:
		case 0x0f:
			{
				const MCHEmul::UByte ddr = _PA -> DDR ();
				const MCHEmul::UByte out = _PA -> OR ();

				// Live external pins for serial/tape/joystick/fire.
				// Default: pull-up/high.
				MCHEmul::UByte pins = 0xff;
				// PA2, PA3, PA4, PA5:
				// joystick up/down/left/fire, active low.
				pins &= (_joystickStatus | 0x80);
				// PA6:
				// tape sense, active low when any cassette key is down.
				pins = _tapeSensePressed ? (pins & ~0x40) : (pins | 0x40);
				// TODO:
				// PA1/PA0:
				// serial data in / serial clock in.
				// Default high unless serial bus pulls low.
				// if (!_serialDataIn)  pins &= ~0x02;
				// if (!_serialClockIn) pins &= ~0x01;

				MCHEmul::UByte input = pins;
				if (_PA -> latchIR ())
					input = _PA -> valueLatched ();

				// Sets the new value...
				_PA -> setPortValue ((out & ddr) | (input & ~ddr));
				// Reading ORA affects CA1/CA2 interrupt flags.
				result = _PA -> value (pp == 0x01);
			}

			break;
			
		default:
			{
				result = COMMODORE::VIARegisters::readValue (pp);
			}

			break;
	}

	return (_lastValueRead = result);
}

// ---
void VIC20::VIA1Registers::initializeInternalValues ()
{
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

	_joystickStatus = 0xff; // No switches clicked, no fire buttons pressed...
	_tapeSensePressed = false; // NO key pressed in the datasette...
}

