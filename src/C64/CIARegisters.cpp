#include <C64/CIARegisters.hpp>

// ---
void C64::CIA1Registers::setValue (size_t p, const MCHEmul::UByte& v)
{
	MCHEmul::PhisicalStorageSubset::setValue (p, v);

	// TODO
}

// ---
const MCHEmul::UByte& C64::CIA1Registers::readValue (size_t p) const
{
	return (MCHEmul::PhisicalStorageSubset::readValue  (p));

	// TODO
}

// ---
void C64::CIA2Registers::initialize ()
{
	MCHEmul::PhisicalStorageSubset::initialize ();

	initializeInternalValues ();
}

// ---
void C64::CIA2Registers::setValue (size_t p, const MCHEmul::UByte& v)
{
	MCHEmul::PhisicalStorageSubset::setValue (p, v);

	switch (p)
	{
		case 0x00:
			_VICBank = 3 - (v.value () & 0x03); // From 0 to 3...
			break;

		default:
			break;
	}
}

// ---
void C64::CIA2Registers::initializeInternalValues ()
{
	// The internal variables are initialized through the data in memory...

	setValue (0, 0x03); // To set the bank 0...
}

// ---
const MCHEmul::UByte& C64::CIA2Registers::readValue (size_t p) const
{
	return (MCHEmul::PhisicalStorageSubset::readValue  (p));

	// TODO
}
