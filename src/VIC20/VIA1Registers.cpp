#include <VIC20/VIA1Registers.hpp>

// ---
VIC20::VIA1Registers::VIA1Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: COMMODORE::VIARegisters (_VIA1_SUBSET, ps, pp, a, s),
	  _VICBank (0)
	  // At this point all internal variables will have random values...
{ 
	setClassName ("VIA1Registers");

	initializeInternalValues (); 
}

// ---
void VIC20::VIA1Registers::setValue (size_t p, const MCHEmul::UByte& v)
{
	COMMODORE::VIARegisters::setValue (p, v);

	size_t pp = p % 0x10;

	switch (pp)
	{
		// TODO
		
		default:
			break;
	}
}

// ---
const MCHEmul::UByte& VIC20::VIA1Registers::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	size_t pp = p % 0x10;

	switch (pp)
	{
		// TODO
			
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
	
	// TODO
}
