#include <C64/SFChipRegisters.hpp>

// ---
C64::SpecialFunctionsChipRegisters::SpecialFunctionsChipRegisters 
		(MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (_SPECIALFUNCTIONSCHIP_SUBSET, ps, pp, a, s),
	  _lastValueRead (MCHEmul::PhysicalStorage::_DEFAULTVALUE)
	  // At this point all internal variables will have random values...
{ 
	setClassName ("SFChipRegisters");

	initializeInternalValues ();
}

// ---
void C64::SpecialFunctionsChipRegisters::initialize ()
{
	MCHEmul::PhysicalStorageSubset::initialize ();

	initializeInternalValues ();
}

// ---
void C64::SpecialFunctionsChipRegisters::setValue (size_t p, const MCHEmul::UByte& v)
{
	MCHEmul::ChipRegisters::setValue (p, v);

	switch (p)
	{
		// The data direction is kept as it is...

		// Dataport 6510
		case 0x01:
			{
				MCHEmul::UByte val0		= MCHEmul::ChipRegisters::readValue (0x00);
				_LORAM					= v.bit (0) && val0.bit (0); // The value of the direction is taken into account...
				_HIRAM					= v.bit (1) && val0.bit (1);
				_CHAREN					= v.bit (2) && val0.bit (2);
				_casetteData			= v.bit (3) && val0.bit (3);
				_noCasetteKeyPressed	= v.bit (4) && !val0.bit (4);
				_casetteMotorRunning	= v.bit (5) && val0.bit (5);;
				// The bits 6 & 7 are not used...
			}

			break;
			
		default:
			break;
	}
}

// ---
const MCHEmul::UByte& C64::SpecialFunctionsChipRegisters::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	switch (p)
	{
		case 0x01:
			{
				result = MCHEmul::UByte::_0;
				result.setBit (0, _LORAM);
				result.setBit (1, _HIRAM);
				result.setBit (2, _CHAREN);
				result.setBit (3, _casetteData);
				result.setBit (4, _noCasetteKeyPressed);
				result.setBit (5, _casetteMotorRunning);
				// Bits 6 & 7 are not used...
			}

			break;
			
		default:
			break;
	}

	return (_lastValueRead = result);
}

// ---
void C64::SpecialFunctionsChipRegisters::initializeInternalValues ()
{
	// Dataport. Dtermines the direction of the bytes 0x01
	// All lines for output except line bit 4 (datasette line sense) that equal to 0.
	setValue (0x00, 0x2f);
	// Very important register
	// = BASIC + KERNAL + I/O + 0 in OUTPUT + 
	// CASETTE SENSE LINE (NOTHING PRESSED) = 1 + CASETTE MOTOR STOPPED = 1; 
	setValue (0x01, 0x37);
}
