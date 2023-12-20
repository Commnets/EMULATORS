#include <COMMODORE/VICIRegisters.hpp>

// ---
COMMODORE::VICIRegisters::VICIRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (_VICREGS_SUBSET, ps, pp, a, s),
	  _lastValueRead (MCHEmul::PhysicalStorage::_DEFAULTVALUE)
{
	setClassName ("VICIRegisters");

	initializeInternalValues ();
}

// ---
void COMMODORE::VICIRegisters::initialize ()
{
	MCHEmul::PhysicalStorageSubset::initialize ();

	initializeInternalValues ();
}

// ---
MCHEmul::InfoStructure COMMODORE::VICIRegisters::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::ChipRegisters::getInfoStructure ());

	// TODO

	return (result);
}

// ---
void COMMODORE::VICIRegisters::setValue (size_t p, const MCHEmul::UByte& v)
{
	size_t pp = p % 0x10;

	// The 64 first bytes will keep the right value...
	MCHEmul::PhysicalStorageSubset::setValue (p, v);

	switch (pp)
	{
		// TODO
	}
}       

// ---
const MCHEmul::UByte& COMMODORE::VICIRegisters::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	size_t pp = p % 0x10;

	switch (pp)
	{
		// TODO
	}

	return (_lastValueRead = result);
}

// ---
void COMMODORE::VICIRegisters::initializeInternalValues ()
{
	// TODO
}
