#include <GENERALINSTRUMENTS/AY38910/AY38910Registers.hpp>

// ---
GENERALINSTRUMENTS::AY38910Registers::AY38910Registers ()
	: MCHEmul::ChipRegisters 
		(GENERALINSTRUMENTS::AY38910Registers::_ID, 
		 new MCHEmul::PhysicalStorage 
			(GENERALINSTRUMENTS::AY38910Registers::_ID, MCHEmul::PhysicalStorage::Type::_RAM, 2),
		 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 2),
	  _internalPhysicalMemory (nullptr),
	  _lastValueRead (MCHEmul::UByte::_0)
	// The rest of the attributes are initialized with the method initializeInternalValues...
{
	_internalPhysicalMemory = physicalStorage ();

	initializeInternalValues ();
}

// ---
GENERALINSTRUMENTS::AY38910Registers::AY38910Registers
		(MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (GENERALINSTRUMENTS::AY38910Registers::_ID, ps, pp, a, s),
	  _internalPhysicalMemory (nullptr)
	 // The rest of the attributes are initialized with the method initializeInternalValues...
{
	initializeInternalValues ();
}

// ---
GENERALINSTRUMENTS::AY38910Registers::~AY38910Registers ()
{
	// It could be nullptr, if it wouldn't have been created internally...
	delete (_internalPhysicalMemory);
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
	size_t pp = p % 4;

	MCHEmul::PhysicalStorageSubset::setValue (pp, v);

	switch (pp)
	{
		// TODO

		default:
			break;
	}

}

// ---
const MCHEmul::UByte& GENERALINSTRUMENTS::AY38910Registers::readValue (size_t p) const
{
	size_t pp = p % 2;

	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	switch (pp)
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
	size_t pp = p % 2;

	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	switch (pp)
	{
		// TODo

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
