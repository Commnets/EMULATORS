#include <C64/CIA1Registers.hpp>

// ---
C64::CIA1Registers::CIA1Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: COMMODORE::CIARegisters (_CIA1_SUBSET, ps, pp, a, s)
	  // At this point all internal variables will have random values...
{ 
	setClassName ("CIA1Registers");

	initializeInternalValues (); 
}

// ---
const MCHEmul::UByte& C64::CIA1Registers::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	size_t pp = p % 0x10;

	switch (pp)
	{
		// Data Port Register A: CIA1PRA
		case 0x00:
			{
				// In CIA 1 the value of the port can be affected 
				// by the keyboard and the joystick 1
				// What is in the portB make conexions (see .hpp) in the portA
				// determining what is shown there...
				unsigned char dtA = MCHEmul::UByte::_0;
				for (size_t i = 0; i < 8; i++)
					if ((~(_portB & _joystick1Status) & (1 << i)) != 0x00)
						dtA |= ~_rev_keyboardStatusMatrix [i].value (); // 1 if clicked...
				_portA = (_outputRegA | ~_dataPortADir) /** What it should go to portA as internal configuration determines. */ & 
					(~dtA & _joystick2Status) /** but affected by the keys and joystick switches pressed. */;
				result = MCHEmul::UByte (_portA);
			}

			break;

		// Data Port Register B: CIA1PRB
		case 0x01:
			{
				// @see above
				unsigned char dtB = MCHEmul::UByte::_0;
				for (size_t i = 0; i < 8; i++)
					if ((~(_portA & _joystick2Status) & (1 << i)) != 0x00)
						dtB |= ~_keyboardStatusMatrix [i].value ();  // 1 if clicked...
				_portB = (_outputRegB | ~_dataPortBDir) & (~dtB & _joystick1Status);
				result = MCHEmul::UByte (_portB);
			}

			break;
			
		default:
			{
				result = COMMODORE::CIARegisters::readValue (pp);
			}

			break;
	}

	return (_lastValueRead = result);
}

// ---
void C64::CIA1Registers::initializeInternalValues ()
{
	COMMODORE::CIARegisters::initializeInternalValues ();
	
	// Data Port A all input...
	setValue (0x02, MCHEmul::UByte::_FF); 
	// Data Port B all output...
	setValue (0x03, MCHEmul::UByte::_0);
	// Just to be able to read well the keyboard...

	_joystick1Status = 0xff; // No switches clicked, no fire buttons pressed...
	_joystick2Status = 0xff; // No switches clicked, no fire buttons pressed...
	for (size_t i = 0; i < 8; i++)
		_keyboardStatusMatrix [i] = _rev_keyboardStatusMatrix [i] = MCHEmul::UByte::_FF; // No keys pressed...
}
