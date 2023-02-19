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
		// Dataport 6510
		case 0x01:
			{
				MCHEmul::UByte val0		= MCHEmul::ChipRegisters::readValue (0x00);

				// The following bits are only taking into account when the register 0 is defined as output direction...
				_LORAM					= val0.bit (0) ? v.bit (0) : false; 
				_HIRAM					= val0.bit (1) ? v.bit (1) : false; 
				_CHAREN					= val0.bit (2) ? v.bit (2) : false;
				_casetteData			= val0.bit (3) ? v.bit (3) : false; 
				// The bit 4 is important when reading, because is something comming from the casette...
				_casetteMotorStopped	= val0.bit (5) ? v.bit (5) : false;
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
	MCHEmul::UByte result = MCHEmul::ChipRegisters::readValue (p);

	switch (p)
	{
		case 0x01:
			{
				MCHEmul::UByte val0 = MCHEmul::ChipRegisters::readValue (0x00);

				result = MCHEmul::UByte::_0;
				result.setBit (0, _LORAM);
				result.setBit (1, _HIRAM);
				result.setBit (2, _CHAREN);
				result.setBit (3, _casetteData);
				// The bit 4 is taken into account only when the bit 4 in the register 0 is defined as input...
				result.setBit (4, !val0.bit (4) ? _casetteNoKeyPressed : true);
				result.setBit (5, _casetteMotorStopped);
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
	// CASETTE SENSE LINE (NOTHING PRESSED) = 0 + CASETTE MOTOR STOPPED = 1; 
	// The ROM when initialize will set this variable automatically...
	setValue (0x01, 0xe7);

	// This is an input from the casette and it is not automatically initialized...
	_casetteNoKeyPressed = true;
}
