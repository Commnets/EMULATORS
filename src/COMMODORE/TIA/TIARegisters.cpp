#include <COMMODORE/TIA/TIARegisters.hpp>

// ---
COMMODORE::TIARegisters::TIARegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (_TIAREGS_SUBSET, ps, pp, a, s),
	  _lastValueRead (MCHEmul::PhysicalStorage::_DEFAULTVALUE)
{ 
	setClassName ("TIARegisters"); 

	initializeInternalValues ();
}

// ---
void COMMODORE::TIARegisters::initialize ()
{
	MCHEmul::PhysicalStorageSubset::initialize (); 

	initializeInternalValues ();
}

// ---
MCHEmul::InfoStructure COMMODORE::TIARegisters::getInfoStructure () const
{
	MCHEmul::InfoStructure result = 
		std::move (MCHEmul::ChipRegisters::getInfoStructure ());

	// TODO

	return (result);
}

// ---
void COMMODORE::TIARegisters::setValue (size_t p, const MCHEmul::UByte& v)
{
	MCHEmul::PhysicalStorageSubset::setValue (p, v);

	size_t pp = p % 0x10;

	switch (pp)
	{
		// TODO

		default:
			// It shouldn't be here...
			assert (false);
			break;
	}
} 

// ---
const MCHEmul::UByte& COMMODORE::TIARegisters::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	size_t pp = p % 0x10;

	switch (pp)
	{
		// TODO

		// It should be here, just in case...
		default:
			assert (false);
			break;
	}

	return (_lastValueRead = result);
}

// ---
void COMMODORE::TIARegisters::initializeInternalValues ()
{
	// TODO
}
