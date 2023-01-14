#include <C64/SIDRegisters.hpp>

// ---
C64::SIDRegisters::SIDRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (_SIDREGS_SUBSET, ps, pp, a, s)
{
	setClassName ("SIDRegisters");

	initializeInternalValues ();
}

// ---
void C64::SIDRegisters::initialize ()
{
	MCHEmul::PhysicalStorageSubset::initialize ();

	initializeInternalValues ();
}

// ---
void C64::SIDRegisters::setValue (size_t p, const MCHEmul::UByte& v)
{
	MCHEmul::PhysicalStorageSubset::setValue (p, v);

	size_t pp = p % 0x20;

	switch (pp)
	{
		// TODO

		default:
			break;
	}
}

// ---
const MCHEmul::UByte& C64::SIDRegisters::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	size_t pp = p % 0x20;

	switch (pp)
	{
		// TODO

		default:
			{
				result = MCHEmul::UByte::_FF;
			}

			break;
	}

	return (_lastValueRead = result);
}

// ---
void C64::SIDRegisters::initializeInternalValues ()
{
	// TODO
}
