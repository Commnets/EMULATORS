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
				// As explained the port A y connected to the portB through the keyboard matrix,...
				// ...so when a column is selected for output the value is taken into account. 
				// Addign all those values received and store in the portB for them later to be read!
				// Notice that is the value into the portB what is put and not the value received
				// as what it is transfer to the portB is what is really there!
				MCHEmul::UByte pB = MCHEmul::UByte::_0;
				if (v != MCHEmul::UByte::_FF)
					for (size_t i = 0; i < 8; i++)
						if ((~portA ().value () & (1 << i)) != 0x00)
							pB |= MCHEmul::UByte (~_keyboardStatusMatrix [i]);
				setPortB (~pB);
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
				result = COMMODORE::CIARegisters::readValue (pp) & // What is in the port A...
					MCHEmul::UByte (_joystick2Status); // ...plus the info from the joystick...
			}

			break;

		case 0x01:
			{
				result = COMMODORE::CIARegisters::readValue (0x01) & // What is in the port B...(taking into account the affection from timers) 
					MCHEmul::UByte (_joystick1Status); // ...plust the info from the joystick
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
