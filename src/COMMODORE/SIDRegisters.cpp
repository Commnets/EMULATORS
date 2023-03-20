#include <COMMODORE/SIDRegisters.hpp>

// ---
COMMODORE::SIDRegisters::SIDRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (_SIDREGS_SUBSET, ps, pp, a, s),
	  _resid_sid (nullptr)
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
							 
	if (_resid_sid != nullptr) // Can happen when emulation starts...
		_resid_sid -> write ((RESID::reg8) (p % 0x20), (RESID::reg8) v.value ()); 
}

// ---
const MCHEmul::UByte& COMMODORE::SIDRegisters::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	if (_resid_sid != nullptr)
	{
		size_t pp = p % 0x20;

		switch (pp)
		{ 
			case 0x00:
			case 0x01:
			case 0x02:
			case 0x03:
			case 0x04:
			case 0x05:
			case 0x06:
			case 0x07:
			case 0x08:
			case 0x09:
			case 0x0a:
			case 0x0b:
			case 0x0c:
			case 0x0d:
			case 0x0e:
			case 0x0f:
			case 0x10:
			case 0x11:
			case 0x12:
			case 0x13:
			case 0x14:
			case 0x15:
			case 0x16:
			case 0x17:
			case 0x18:
				result = std::move (_resid_sid -> read_state ()).sid_register [pp];
				break;


			// Direct information from the structure kept into the REDSID::SID
			case 0x19:
			case 0x1a:
			case 0x1b:
			case 0x1c:
				MCHEmul::UByte ((unsigned char) _resid_sid -> read ((RESID::reg8) (p % 0x20)));
				break;

			// Not connected...
			case 0x1d:
			case 0x1e:
			case 0x1f:
				result = MCHEmul::UByte::_FF;
				break;

			default:
				result = MCHEmul::PhysicalStorageSubset::readValue (pp);
				break;
		}
	}

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
