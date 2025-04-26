#include <GENERALINSTRUMENTS/AY38910/AY38910Registers.hpp>

// ---
GENERALINSTRUMENTS::AY38910Registers::AY38910Registers ()
	: MCHEmul::ChipRegisters (GENERALINSTRUMENTS::AY38910Registers::_ID, 
		2 /** 2 bytes per address. */, 3 /** 3 Registers. */),
	  _lastValueRead (MCHEmul::UByte::_0)
	// The rest of the attributes are initialized with the method initializeInternalValues...
{
	initializeInternalValues ();
}

// ---
GENERALINSTRUMENTS::AY38910Registers::AY38910Registers
		(MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (GENERALINSTRUMENTS::AY38910Registers::_ID, ps, pp, a, s)
	 // The rest of the attributes are initialized with the method initializeInternalValues...
{
	initializeInternalValues ();
}

// ---
void GENERALINSTRUMENTS::AY38910Registers::initialize ()
{
	MCHEmul::ChipRegisters::initialize ();

	// TODO

	initializeInternalValues ();
}

// ---
MCHEmul::InfoStructure GENERALINSTRUMENTS::AY38910Registers::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	// TODO

	return (result);
}

// ---
void GENERALINSTRUMENTS::AY38910Registers::setValue (size_t p, const MCHEmul::UByte& v)
{
	if (p >= numberRegisters ())
	{
		_LOG ("Register: " + std::to_string (p) + " not supported in AY38910");

		return;
	}

	MCHEmul::PhysicalStorageSubset::setValue (p, v);

	switch (p)
	{
		// TODO

		// It shouldn't be here...
		default:
			break;
	}

}

// ---
const MCHEmul::UByte& GENERALINSTRUMENTS::AY38910Registers::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	if (p >= numberRegisters ())
	{
		_LOG ("Register: " + std::to_string (p) + " not supported in AY38910");

		return (_lastValueRead = result);
	}

	switch (p)
	{
		// TODO

		// It shouldn't be here...
		default:
			break;
	}

	return (_lastValueRead = result);
}

// ---
const MCHEmul::UByte& GENERALINSTRUMENTS::AY38910Registers::peekValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	if (p >= numberRegisters ())
	{
		_LOG ("Register: " + std::to_string (p) + " not supported in AY38910");

		return (_lastValueRead = result);
	}

	switch (p)
	{
		// TODO

		default:
			break;
	}

	return (_lastValueRead = result);
}

// ---
void GENERALINSTRUMENTS::AY38910Registers::initializeInternalValues ()
{
	// TODO
}
