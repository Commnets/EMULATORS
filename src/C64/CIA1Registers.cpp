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
				// @see 0x01 for futher explanations.
				// This behaviour is a mirror in the port A of that one...
				result = MCHEmul::UByte::_0;
				MCHEmul::UByte msk = (MCHEmul::PhysicalStorageSubset::readValue (0x01) & 
					_joystick1Status) | ~dataPortBDir ();
				for (size_t i = 0; i < 8; i++)
					if ((~msk.value () & (1 << i)) != 0x00)
						result |= ~(_rev_keyboardStatusMatrix [i] & ~dataPortADir ());
				result |= ~(_joystick2Status & ~dataPortADir ());
				result = ~result;
				result |= MCHEmul::UByte (dataPortADir () & dataPortBDir ()) & 
					MCHEmul::PhysicalStorageSubset::readValue (0x00);
			}

			break;

		// Data Port Register B: CIA1PRB
		case 0x01:
			{
				// As explained the port A y connected to the portB through the keyboard matrix,...
				// So when portA (columns) is selected to be read, 
				// the values from the portB (including the joystick1) are taken.
				// The initial active points in that portB (rows) will the OR 
				// of all those ones where the portA is active.

				// "result" will mark with true the bit corresponding to the key of the row selected....
				result = MCHEmul::UByte::_0;
				// What is in the 0x00 port (+ joystick2) corrected by the direction at that port (output bits = 1)
				// is what will drive the value at the port B depending on the keys pressed (corrected by input, bits = 0)
				MCHEmul::UByte msk = (MCHEmul::PhysicalStorageSubset::readValue (0x00) &
					_joystick2Status) | ~dataPortADir ();
				for (size_t i = 0; i < 8; i++)
					if ((~msk & (1 << i)) != 0x00)
						result |= ~(_keyboardStatusMatrix [i] & ~dataPortBDir ());
				// The input of the joystick1 connected has also to be taken into account...
				result |= ~(_joystick1Status & ~dataPortBDir ());
				// But C64 expects the result in the opposite way...
				// it is: the bits corresponding to keys selected must be set to 0...
				result = ~result;
				result |= MCHEmul::UByte (dataPortADir () & dataPortBDir ()) & 
					MCHEmul::PhysicalStorageSubset::readValue (0x01);
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
