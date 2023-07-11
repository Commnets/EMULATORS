#include <COMMODORE/SIDRegisters.hpp>

// ---
COMMODORE::SIDRegisters::SIDRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (_SIDREGS_SUBSET, ps, pp, a, s),
	  _sidWrapper (nullptr)
{ 
	setClassName ("SIDRegisters"); 

	initializeInternalValues ();
}

// ---
void COMMODORE::SIDRegisters::initialize ()
{
	MCHEmul::PhysicalStorageSubset::initialize ();

	initializeInternalValues ();
}

// ---
void COMMODORE::SIDRegisters::setValue (size_t p, const MCHEmul::UByte& v)
{ 
	MCHEmul::PhysicalStorageSubset::setValue (p, v);
							 
	if (_sidWrapper != nullptr) // Can happen when emulation starts...
		_sidWrapper -> setValue ((p % 0x20), v.value ()); 
}

// ---
const MCHEmul::UByte& COMMODORE::SIDRegisters::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;
	if (_sidWrapper != nullptr)
		result = _sidWrapper -> readValue (p % 0x20);

	return (_lastValueRead = result);
}

// ---
const MCHEmul::UByte& COMMODORE::SIDRegisters::peekValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;
	if (_sidWrapper != nullptr)
		result = _sidWrapper -> peekValue (p % 0x20); // Not modifying...

	return (_lastValueRead = result);
}

// ---
void COMMODORE::SIDRegisters::initializeInternalValues ()
{
	setValue (0x00, MCHEmul::UByte::_0);
	setValue (0x01, MCHEmul::UByte::_0);
	setValue (0x02, MCHEmul::UByte::_0);
	setValue (0x03, MCHEmul::UByte::_0);
	setValue (0x04, MCHEmul::UByte::_0);
	setValue (0x05, MCHEmul::UByte::_0);
	setValue (0x06, MCHEmul::UByte::_0);
	setValue (0x07, MCHEmul::UByte::_0);
	setValue (0x08, MCHEmul::UByte::_0);
	setValue (0x09, MCHEmul::UByte::_0);
	setValue (0x0a, MCHEmul::UByte::_0);
	setValue (0x0b, MCHEmul::UByte::_0);
	setValue (0x0c, MCHEmul::UByte::_0);
	setValue (0x0d, MCHEmul::UByte::_0);
	setValue (0x0e, MCHEmul::UByte::_0);
	setValue (0x0f, MCHEmul::UByte::_0);
	setValue (0x10, MCHEmul::UByte::_0);
	setValue (0x11, MCHEmul::UByte::_0);
	setValue (0x12, MCHEmul::UByte::_0);
	setValue (0x13, MCHEmul::UByte::_0);
	setValue (0x14, MCHEmul::UByte::_0);
	setValue (0x15, MCHEmul::UByte::_0);
	setValue (0x16, MCHEmul::UByte::_0);
	setValue (0x17, MCHEmul::UByte::_0);
	setValue (0x18, MCHEmul::UByte::_0);
	setValue (0x19, MCHEmul::UByte::_0);
	setValue (0x1a, MCHEmul::UByte::_0);
	setValue (0x1b, MCHEmul::UByte::_0);
	setValue (0x1c, MCHEmul::UByte::_0);
	setValue (0x1d, MCHEmul::UByte::_0);
	setValue (0x1e, MCHEmul::UByte::_0);
	setValue (0x1f, MCHEmul::UByte::_0);
}
