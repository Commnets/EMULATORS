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
		// To select the slot of every bank...
		case 0x00:
			{
				_slotBank0 = (v.value () & 0x03);
				_slotBank1 = (v.value () & 0x0c) >> 2;
				_slotBank2 = (v.value () & 0x30) >> 4;
				_slotBank3 = (v.value () & 0xc0) >> 6;
				_slotChanged = true; // To send the event...
			}

			break;

		case 0x01:
			{
				// TODO
			}

			break;

		// Among other things this register selects the row of the keyboard to be read...
		case 0x02:
			{
				if ((_keyboardRowSelected = v.value () & 0x0f) > 10) // From 0 to 10. Over that nothing is valid...
					_keyboardRowSelected = 0x0f; // Meaaning none...
			}

			break;

		case 0x03:
			{
				// TODO
			}

			break;

		// It shouldn't be here...
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
		// To select the slot of every bank...
		case 0x00:
			{
				result = MCHEmul::PhysicalStorageSubset::readValue (0x00);
			}

			break;

		// To read the status of the keyboard...
		case 0x01:
			{
				result = _keyboardStatusMatrix [_keyboardRowSelected];
			}

			break;

		case 0x02:
			{
				// TODO
			}

			break;

		case 0x03:
			{
				// TODO
			}

			break;

		// It shouldn't be here...
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

	switch (p)
	{
		// To select the slot of every bank...
		case 0x00:
			{
				result = MCHEmul::PhysicalStorageSubset::readValue (0x00);
			}

			break;

		case 0x01:
			{
				// TODO
			}

			break;

		case 0x02:
			{
				// TODO
			}

			break;

		case 0x03:
			{
				// TODO
			}

			break;

		// It shouldn't be here...
		default:
			break;
	}

	return (_lastValueRead = result);
}

// ---
void MSX::PPI8255Registers::initializeInternalValues ()
{
	_slotBank0 = _slotBank1 = _slotBank2 = _slotBank3 = 0;

	for (size_t i = 0; i < _keyboardStatusMatrix.size (); 
		_keyboardStatusMatrix [i++] = MCHEmul::UByte::_FF); // None pressed...
	_keyboardRowSelected = 0x0f; // ...Meaning none...
}
