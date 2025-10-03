#include <COMMODORE/ACIA/ACIARegisters.hpp>

// ---
COMMODORE::ACIARegisters::ACIARegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (_ACIAREGS_SUBSET, ps, pp, a, s),
	  _lastValueRead (MCHEmul::PhysicalStorage::_DEFAULTVALUE)
{ 
	setClassName ("ACIARegisters"); 

	initializeInternalValues ();
}

// ---
void COMMODORE::ACIARegisters::initialize ()
{
	MCHEmul::PhysicalStorageSubset::initialize (); 

	initializeInternalValues ();
}

// ---
MCHEmul::InfoStructure COMMODORE::ACIARegisters::getInfoStructure () const
{
	MCHEmul::InfoStructure result = 
		std::move (MCHEmul::ChipRegisters::getInfoStructure ());

	// TODO

	return (result);
}

// ---
void COMMODORE::ACIARegisters::setValue (size_t p, const MCHEmul::UByte& v)
{
	MCHEmul::PhysicalStorageSubset::setValue (p, v);

	// In block of 4 elements...
	size_t pp = p % 0x04;

	switch (pp)
	{
		// TODO

		default:
			break;
	}
} 

// ---
const MCHEmul::UByte& COMMODORE::ACIARegisters::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	// In block of 4 elements...
	size_t pp = p % 0x04;

	switch (pp)
	{
		// TODO

		default:
			break;
	}

	return (_lastValueRead = result);
}

// ---
void COMMODORE::ACIARegisters::initializeInternalValues ()
{
	// TODO
}
