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
		// Data Port Register A: CIAPRA
		case 0x00:
			{
				// To indicate the row of the key matrix to be selected (later)
				// It is affected by the communication direction (CIDDRA) (bits 1 output)
				// bits = 0 will indicate rows selected!
				_keyboardRowToRead = v.value ();
			}

			break;

		// Data Direction Register A: CIDDRA
		// The row to read is also affected...
		case 0x02:
			{
				_dataPortADir = v.value ();
			}

			break;

		// Data Direction Register B: CIDDRB
		case 0x03:
			{
				_dataPortBDir = v.value ();
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
			result = MCHEmul::UByte (_joystick2Status);
			break;

		case 0x01:
			{
				switch (_keyboardRowToRead)
				{
					case 0x00:
						break;

					case 0xff:
						{
							result = _keyboardStatusMatrix [7] & ~_dataPortBDir;
						}

						break;

					default:
						{
							// Identify the row to read...
							// Take into account that the row is identified writting 0 in its bit (so it is necessary to inverse it first)
							unsigned char bRV = ~_keyboardRowToRead & _dataPortADir; // 0xff all output...
							size_t c = 0; while ((bRV >>= 1) != 0) c++;
							// If more than 1 row will be possible, the greatest one will be choosen...
							// The columns (from keyboard or joystick 1) connected will have its bits to 0!...
							result = _keyboardStatusMatrix [c] & ~_dataPortBDir; // 0x00 all input...
						}

						break;
				}

				// These bits override anything if the timer has been parameterized for that...
				if (reflectTimerAAtPortDataB () != 0) 
					result.setBit (6, reflectTimerAAtPortDataB () == 1 ? true : false);
				if (reflectTimerBAtPortDataB () != 0)
					result.setBit (7, reflectTimerBAtPortDataB () == 1 ? true : false);
			}
			
			break;
			
		default:
			result = COMMODORE::CIARegisters::readValue (pp);
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

	_joystick2Status = 0xff; // No switches clicked, no fire buttons pressed...
	for (size_t i = 0; i < 8; _keyboardStatusMatrix [i++] = MCHEmul::UByte::_FF); // No keys pressed...
}
