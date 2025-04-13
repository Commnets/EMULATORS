#include <MSX/PPI8255Registers.hpp>

// ---
MSX::PPI8255Registers::PPI8255Registers ()
	: MCHEmul::ChipRegisters 
		(MSX::PPI8255Registers::_ID, 
		 new MCHEmul::PhysicalStorage 
			(MSX::PPI8255Registers::_ID, MCHEmul::PhysicalStorage::Type::_RAM, 2),
		 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 2),
	  _internalPhysicalMemory (nullptr),
	  _lastValueRead (MCHEmul::UByte::_0)
	// The rest of the attributes are initialized with the method initializeInternalValues...
{
	_internalPhysicalMemory = physicalStorage ();

	initializeInternalValues ();
}

// ---
MSX::PPI8255Registers::PPI8255Registers
		(MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (MSX::PPI8255Registers::_ID, ps, pp, a, s),
	  _internalPhysicalMemory (nullptr)
	 // The rest of the attributes are initialized with the method initializeInternalValues...
{
	initializeInternalValues ();
}

// ---
MSX::PPI8255Registers::~PPI8255Registers ()
{
	// It could be nullptr, if it wouldn't have been created internally...
	delete (_internalPhysicalMemory);
}

// ---
void MSX::PPI8255Registers::initialize ()
{
	MCHEmul::ChipRegisters::initialize ();

	// TODO

	initializeInternalValues ();
}

// ---
MCHEmul::InfoStructure MSX::PPI8255Registers::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	// TODO

	return (result);
}

// ---
void MSX::PPI8255Registers::setValue (size_t p, const MCHEmul::UByte& v)
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
const MCHEmul::UByte& MSX::PPI8255Registers::readValue (size_t p) const
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
const MCHEmul::UByte& MSX::PPI8255Registers::peekValue (size_t p) const
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
void MSX::PPI8255Registers::initializeInternalValues ()
{
	// TODO
}
