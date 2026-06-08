#include <VIC20/VIA2Registers.hpp>

// ---
VIC20::VIA2Registers::VIA2Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: COMMODORE::VIARegisters (_VIA2_SUBSET, ps, pp, a, s)
	  // At this point all internal variables will have random values...
{ 
	setClassName ("VIA2Registers");

	initializeInternalValues ();
}

// ---
const MCHEmul::UByte& VIC20::VIA2Registers::readValue (size_t p) const
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
		// VIA2 Port B: keyboard columns + joystick right
		case 0x00: 
			{
				const MCHEmul::UByte ddra	= _PA -> DDR ().value ();
				const MCHEmul::UByte ora	= _PA -> OR  ().value ();
				const MCHEmul::UByte ddrb	= _PB -> DDR ().value ();
				MCHEmul::UByte orb			= _PB -> OR  ().value ();

				// PB7 may be driven by Timer 1 if it is configured as output.
				if ((ddrb & 0x80) != 0x00)
				{
					if (_T1 -> runMode () == COMMODORE::VIATimer::RunMode::_ONESHOOTSIGNAL ||
						_T1 -> runMode () == COMMODORE::VIATimer::RunMode::_CONTINUOUSSIGNAL)
						orb = _PB -> p7 () ? (orb | 0x80) : (orb & ~0x80);
				}

				// External PB pins.
				// Normally high.
				MCHEmul::UByte externalPB = 0xff;
				// PB7: joystick right/east, active low, only relevant when PB7 is input.
				if ((ddrb & 0x80) == 0x00)
					externalPB = (externalPB & 0x7f) | (_joystickStatus & 0x80);

				unsigned char mask = 0x01;
				const MCHEmul::UByte effectivePA = (ora & ddra) | (MCHEmul::UByte (0xff) & ~ddra);
				MCHEmul::UByte colsLow = 0x00;
				for (size_t row = 0; row < 8; row++, mask <<= 1)
					// Row selected when PA bit is low.
					if ((effectivePA & mask) == 0x00)
						// If row is selected low, pressed keys can pull columns low.
						colsLow |= ~_rev_keyboardStatusMatrix [row];

				// Base PB value...
				MCHEmul::UByte pb = (orb & ddrb) | (externalPB & ~ddrb);
				// Keyboard pulls input cols low.
				pb &= ~colsLow;

				// Sets the new value...
				_PB -> setPortValue (pb);
				// Reading $9120 affects CB1/CB2 flags.
				result = _PB -> value (true);
			}

			break;

		// VIA2 Port A: keyboard rows
		case 0x01: 
		case 0x0f:
			{
				const MCHEmul::UByte ddra	= _PA -> DDR ();
				const MCHEmul::UByte ora	= _PA -> OR  ();
 				MCHEmul::UByte ddrb			= _PB -> DDR ();
				MCHEmul::UByte orb			= _PB -> OR  ();

				// PB7 may be driven by Timer 1 when configured as output
				// and ACR selects the PB7 output mode.
				if ((ddrb & 0x80) != 0x00)
				{
					if (_T1 -> runMode () == COMMODORE::VIATimer::RunMode::_ONESHOOTSIGNAL ||
						_T1 -> runMode () == COMMODORE::VIATimer::RunMode::_CONTINUOUSSIGNAL)
						orb = _PB -> p7 () ? (orb | 0x80) : orb & ~0x80;
				}

				// Effective columns driven by Port B.
				// DDRB=1 -> ORB drives the pin.
				// DDRB=0 -> input/pull-up, considered high for keyboard scanning.
				unsigned char mask = 0x01;
				const MCHEmul::UByte effectivePB = (orb & ddrb) | (MCHEmul::UByte (0xff) & ~ddrb);
				MCHEmul::UByte rowsLow = MCHEmul::UByte::_0;
				for (size_t col = 0; col < 8; col++, mask <<= 1)
					// Column selected when PB bit is low.
					if ((effectivePB & mask) == 0x00)
						// Keys pressed in this column pull rows low.
						rowsLow |= ~_keyboardStatusMatrix [col].value ();

				// Base PA value:
				// DDRA=1 -> ORA output.
				// DDRA=0 -> external row pins, normally high.
				MCHEmul::UByte pa = (ora & ddra) | (MCHEmul::UByte (0xff) & ~ddra);
				// Keyboard pulls input rows low.
				pa &= ~rowsLow;

				// Sets the new value...
				_PA -> setPortValue (pa);
				// Reading $9121 affects CA1/CA2 flags.
				result = _PA -> value (pp == 0x00);
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
void VIC20::VIA2Registers::initializeInternalValues ()
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
	
	// Data Port B all input...
	setValue (0x00, MCHEmul::UByte::_FF); 
	// Data Port A all output...
	setValue (0x01, MCHEmul::UByte::_0);
	// Just to be able to read well the keyboard...

	_joystickStatus = 0xff; // No switches clicked, no fire buttons pressed...
	for (size_t i = 0; i < 8; i++)
		_keyboardStatusMatrix [i] = _rev_keyboardStatusMatrix [i] = MCHEmul::UByte::_FF; // No keys pressed...
}
