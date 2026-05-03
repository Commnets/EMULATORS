#include <C64/CIA2Registers.hpp>

// ---
C64::CIA2Registers::CIA2Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: COMMODORE::CIARegisters (_CIA2_SUBSET, ps, pp, a, s),
	  _VICBank (0)
	  // At this point all internal variables will have random values...
{ 
	setClassName ("CIA2Registers");

	initializeInternalValues (); 
}

// ---
void C64::CIA2Registers::setValue (size_t p, const MCHEmul::UByte& v)
{
	COMMODORE::CIARegisters::setValue (p, v);

	size_t pp = p % 0x10;

	switch (pp)
	{
		// Data Port Register A: CI2PRA
		// The three first bits control VICII active bank
		// When the data direction dir is changed, the bank has also to be calculated!
		case 0x00:
		case 0x02:
			{
				_VICBank = 0x03 - (portA ().value () & 0x03); // From 0 to 3...

				// Pending to implement the RS232 and Serial bus in the port A...

				// Notifies the change...
				notify (MCHEmul::Event (COMMODORE::VICII::_BANK0SET + (unsigned int) _VICBank));
			}

			break;
			
		default:
			break;
	}
}

// ---
const MCHEmul::UByte& C64::CIA2Registers::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	size_t pp = p % 0x10;

	switch (pp)
	{
		case 0x00:
			{
				result = COMMODORE::CIARegisters::readValue (pp);

				// The read must be modified to take into account the Serial Port info.
				// The two most significant bits are always 1 in the CIA2PRA register 
				// because the serial port is not connected to anything, so they are always in high level (1)...
				result |= 0xc0; 
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
void C64::CIA2Registers::initializeInternalValues ()
{
	COMMODORE::CIARegisters::initializeInternalValues ();

	// To set the bank 0...
	setValue (0x00, 0x03); 
}
