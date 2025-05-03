#include <MSX/PPI8255Registers.hpp>

// ---
MSX::PPI8255Registers::PPI8255Registers ()
	: MCHEmul::ChipRegisters (MSX::PPI8255Registers::_ID,
		2 /** 2 bytes per address. */, 4 /** Registers. */),
	  _lastValueRead (MCHEmul::UByte::_0),
	  _slotChanged (false),
	  _keyboardStatusMatrix (11, MCHEmul::UByte::_FF)
	// The rest of the attributes are initialized with the method initializeInternalValues...
{
	initializeInternalValues ();
}

// ---
MSX::PPI8255Registers::PPI8255Registers
		(MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (MSX::PPI8255Registers::_ID, ps, pp, a, s),
	  _lastValueRead (MCHEmul::UByte::_0),
	  _slotChanged (false)
	 // The rest of the attributes are initialized with the method initializeInternalValues...
{
	initializeInternalValues ();
}

// ---
void MSX::PPI8255Registers::initialize ()
{
	MCHEmul::ChipRegisters::initialize ();

	// To force the configuration...
	_slotChanged = true;

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
	if (p >= numberRegisters ())
	{
		_LOG ("Register: " + std::to_string (p) + " not supported in PPI8255");

		return;
	}

	MCHEmul::PhysicalStorageSubset::setValue (p, v);

	switch (p)
	{
		// PPI Port A (Register A)
		// Primary Slot Select Register
		case 0x00:
			{
				_slotBank0 = (v.value () & 0x03);
				_slotBank1 = (v.value () & 0x0c) >> 2;
				_slotBank2 = (v.value () & 0x30) >> 4;
				_slotBank3 = (v.value () & 0xc0) >> 6;
				_slotChanged = true; // To send the event...
			}

			break;

		// PPI Port B (Register B)
		// Keyboard Matrix Input Register
		// This is read-only register, so it cannot be written...
		case 0x01:
			break;

		// PPI Por C (Register C)
		// Keyboard & Casette Interface
		case 0x02:
			{
				_keyboardRowSelected	= v.value () & 0x0f; // It could be > 10...
				_casetteMotorControl	= v.bit (4);
				_casetteWriteSignal		= v.bit (5);
				_keyboardCAPSLED		= v.bit (6);
				_keyClickSoundOuput		= v.bit (7);
			}

			break;

		// PPI Command Register
		// This is the fastest way to modify individual bits in the register C (the previous one)
		case 0x03:
			{
				// In the MSX1 this bit must be to 0 for something to happen
				// Otherwise the system doesn't do anything and the registers continue as they were before...
				if (v.bit (7))
					break;

				bool s = v.bit (0); // Set o Reset?
				unsigned nb = (v.value () & 0x0e) >> 1; // From 0 to 7...
				switch (nb)
				{
					case 0: _keyboardRowSelected = (_keyboardRowSelected & ~0x01) | (s ? 0x01 : 0x00); break;
					case 1: _keyboardRowSelected = (_keyboardRowSelected & ~0x02) | (s ? 0x02 : 0x00); break;
					case 2: _keyboardRowSelected = (_keyboardRowSelected & ~0x04) | (s ? 0x04 : 0x00); break;
					case 3: _keyboardRowSelected = (_keyboardRowSelected & ~0x08) | (s ? 0x08 : 0x00); break;
					case 4: _casetteMotorControl	= s; break;
					case 5: _casetteWriteSignal		= s; break;
					case 6: _keyboardCAPSLED		= s; break;
					case 7: _keyClickSoundOuput		= s; break;
				}
			}

			break;

		// It shouldn't be here...
		// It is guarantted with the check at the beginning of the method...
		default:
			break;
	}

}

// ---
const MCHEmul::UByte& MSX::PPI8255Registers::readValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	if (p >= numberRegisters ())
	{
		_LOG ("Register: " + std::to_string (p) + " not supported in PPI8255");

		return (_lastValueRead = result);
	}

	switch (p)
	{
		// PPI Port A (Register A)
		// Primary Slot Select Register
		case 0x00:
			{
				result = MCHEmul::PhysicalStorageSubset::readValue (0x00);
			}

			break;

		// PPI Port B (Register B)
		// Keyboard Matrix Input Register
		case 0x01:
			{
				if (_keyboardRowSelected <= 10) // To avoid running out of the limits...
					result = _keyboardStatusMatrix [_keyboardRowSelected];
			}

			break;

		// PPI Por C (Register C)
		// Keyboard & Casette Interface
		case 0x02:
			{
				result = MCHEmul::PhysicalStorageSubset::readValue (0x02);
			}

			break;

		// PPI Command Register
		// This Register is write only, so it cannot read...
		case 0x03:
			break;

		// It shouldn't be here...
		// It is guarantted by the lines at the top of the method...
		default:
			break;
	}

	return (_lastValueRead = result);
}

// ---
const MCHEmul::UByte& MSX::PPI8255Registers::peekValue (size_t p) const
{
	MCHEmul::UByte result = MCHEmul::PhysicalStorage::_DEFAULTVALUE;

	if (p >= numberRegisters ())
	{
		_LOG ("Register: " + std::to_string (p) + " not supported in PPI8255");

		return (_lastValueRead = result);
	}

	result = MCHEmul::PhysicalStorageSubset::readValue (0x00);

	return (_lastValueRead = result);
}

// ---
void MSX::PPI8255Registers::initializeInternalValues ()
{
	_slotBank0 = _slotBank1 = _slotBank2 = _slotBank3 = 0;

	for (size_t i = 0; i < _keyboardStatusMatrix.size (); 
		_keyboardStatusMatrix [i++] = MCHEmul::UByte::_FF); // None pressed...
	_keyboardRowSelected = 15; // ...Meaning none...
}
