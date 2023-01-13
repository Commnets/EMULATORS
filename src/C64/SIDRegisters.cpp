#include <C64/SIDRegisters.hpp>

// ---
C64::SIDRegisters::SIDRegisters (int id, MCHEmul::PhysicalStorage* pS)
	: MCHEmul::ChipRegisters (id, pS, 0xd400, MCHEmul::Address ({ 0x00, 0xd4 }, false), 0x0400)
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
