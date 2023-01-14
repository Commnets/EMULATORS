#include <C64/CIA2Registers.hpp>
#include <C64/CIATimer.hpp>
#include <C64/CIAClock.hpp>

// ---
C64::CIA2Registers::CIA2Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: C64::CIARegisters (_CIA2_SUBSET, ps, pp, a, s)
	  // At this point all internal variables will have random values...
{ 
	setClassName ("CIA2Registers");

	initializeInternalValues (); 
}

// ---
void C64::CIA2Registers::setValue (size_t p, const MCHEmul::UByte& v)
{
	C64::CIARegisters::setValue (p, v);

	size_t pp = p % 0x10;

	switch (pp)
	{
		// Data Port Register A: CI2PRA
		// The three first bites control VICIIB active bank
		case 0x00:
			{
				_VICBank = 0x03 - (v.value () & 0x03); // From 0 to 3...
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
		// TODO
			
		default:
			result = C64::CIARegisters::readValue (pp);
			break;
	}

	return (_lastValueRead = result);
}

// ---
void C64::CIA2Registers::initializeInternalValues ()
{
	C64::CIARegisters::initializeInternalValues ();

	// To set the bank 0...
	setValue (0x00, 0x03); 
}
