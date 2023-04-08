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
void C64::CIA1Registers::setValue (size_t p, const MCHEmul::UByte& v)
{
	COMMODORE::CIARegisters::setValue (p, v);

	size_t pp = p % 0x10;

	switch (pp)
	{
		// Data Port Register A: CIA1PRA
		case 0x00:
			{
				// To indicate the row of the key matrix to be selected (later)
				// It is affected by the communication direction (CIDDRA1) (bits 1 output)
				// bits = 0 will indicate rows selected!
				_keyboardRowToRead = v.value ();
			}

			break;
			
		default:
			break;
	}
}

// ---
const MCHEmul::UByte& C64::CIA1Registers::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	size_t pp = p % 0x10;

	switch (pp)
	{
		case 0x00:
			{
				result = MCHEmul::UByte (_joystick2Status);
			}

			break;

		case 0x01:
			{
				// Result will mark with true the bit corresponding to the key of the row selected....
				result = MCHEmul::UByte::_0;
				// All rows are going to be tested...
				if (_keyboardRowToRead != 0xff)
				{
					for (size_t i = 0; i < 8; i++)
					{ 
						// if the row has been selected and it is also defined for input...
						// ...the values in that row are taken into account...
						if ((~_keyboardRowToRead & _dataPortADir & (1 << i)) != 0x00)
							result |= ~_keyboardStatusMatrix [i] & ~_dataPortBDir /** 0 bit marked is input */;
					}
				}

				// The input of the joystick has also to be taken into account...
				result |= MCHEmul::UByte (~_joystick1Status & ~_dataPortBDir);

				// But C64 expects the result in the opposite way...
				// it is: the bits corresponding to keys selected must be set to 0...
				result = ~result;

				// These bits override anything if the timer has been parameterized for that...
				// Replicates the behaviour already written in CIA generic readvalue method.
				if (reflectTimerAAtPortDataB () != 0) 
					result.setBit (6, reflectTimerAAtPortDataB () == 1
						? (result.bit (6) ? false : true) /** toggle. */ 
						: (reflectTimerAAtPortDataB () == 2) ? true /** pulse on. */ : false /** pulse off. */);
				if (reflectTimerBAtPortDataB () != 0)
					result.setBit (7, reflectTimerBAtPortDataB () == 1
						? (result.bit (7) ? false : true) /** toggle. */ 
						: (reflectTimerBAtPortDataB () == 2) ? true /** pulse on. */ : false /** pulse off. */);
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
	for (size_t i = 0; i < 8; _keyboardStatusMatrix [i++] = MCHEmul::UByte::_FF); // No keys pressed...
}
