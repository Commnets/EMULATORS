#include <COMMODORE/SID/SIDRegisters.hpp>

// ---
COMMODORE::SIDRegisters::SIDRegisters (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (_SIDREGS_SUBSET, ps, pp, a, s),
	  _potenciometerValues (nullptr),
	  _sidWrapper (nullptr) // Initialized later with the specif method!
{ 
	setClassName ("SIDRegisters");

	// The info about the potenciomater's values!
	_potenciometerValues = new unsigned char* [2]; // Two "groups" (joysticks)
	for (size_t i = 0; i < 2; 
		_potenciometerValues [i++] = new unsigned char [2]); // To different movements each (axis in the joystick)

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
							 
	// In this case neither POTX nor POTY 
	// are affected when a value is written in the record...

	if (_sidWrapper != nullptr) // Can happen when emulation starts...
		_sidWrapper -> setValue ((p % 0x20), v.value ()); 
}

// ---
const MCHEmul::UByte& COMMODORE::SIDRegisters::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	// Neither POTX not PTY are simulated in any 
	// of the libraries that might be connected to the simulation.
	// So they are implemented here and using the SIGRegisters...

	if (_sidWrapper != nullptr)
	{
		size_t pp = p % 0x20;
		switch (pp)
		{
			// POTX
			case 0x19:
				if (_potenciometerGroupActive != MCHEmul::_S0) // A potenciometer has to be defined...
					result = _potenciometerValues [_potenciometerGroupActive][0];
				break;

			// POTY
			case 0x1a:
				if (_potenciometerGroupActive != MCHEmul::_S0) // A potenciometer has to be defined...
					result = _potenciometerValues [_potenciometerGroupActive][1];
				break;

			default:
				result = _sidWrapper -> readValue (pp);
		}
	}

	return (_lastValueRead = result);
}

// ---
const MCHEmul::UByte& COMMODORE::SIDRegisters::peekValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	if (_sidWrapper != nullptr)
	{ 
		size_t pp = p % 0x20;
		switch (pp)
		{
			// POTX and POTY...
			case 0x19:
			case 0x1a:
				result = readValue (p);
				break;

			default:
				result = _sidWrapper -> peekValue (p % 0x20); // Not modifying...
				break;
		}
	}

	// The last value read is not affected by peek function...
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

	// All potenciometer positions to 0. */
	_potenciometerGroupActive = MCHEmul::_S0; // None...
	for (size_t i = 0; i < 2; i++)
		_potenciometerValues [i][0] = _potenciometerValues [i][1] = (unsigned char) 0;
}
