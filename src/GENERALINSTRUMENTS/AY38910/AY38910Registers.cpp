#include <GENERALINSTRUMENTS/AY38910/AY38910Registers.hpp>

// ---
GENERALINSTRUMENTS::AY38910Registers::AY38910Registers ()
	: MCHEmul::ChipRegisters (GENERALINSTRUMENTS::AY38910Registers::_ID, 
		2 /** 2 bytes per address. */, 3 /** 3 Registers. */),
	  _lastValueRead (MCHEmul::UByte::_0),
	  _AY38910Wrapper (nullptr)
	// The rest of the attributes are initialized with the method initializeInternalValues...
{
	initializeInternalValues ();
}

// ---
GENERALINSTRUMENTS::AY38910Registers::AY38910Registers
		(MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (GENERALINSTRUMENTS::AY38910Registers::_ID, ps, pp, a, s),
	  _lastValueRead (MCHEmul::UByte::_0),
	  _AY38910Wrapper (nullptr)
	 // The rest of the attributes are initialized with the method initializeInternalValues...
{
	initializeInternalValues ();
}

// ---
void GENERALINSTRUMENTS::AY38910Registers::initialize ()
{
	MCHEmul::ChipRegisters::initialize ();

	// By defasult the initial selected control register is 0x00...
	_selectedControlRegister = 0x00;

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
		// This is the register to select the control register... 
		// ...where the following read/write instruction must be done
		case 0x00:
			{
				_selectedControlRegister = v.value ();
			}

			break;

		// The action over the control register selected by the previous instruction
		case 0x01:
			{
				_AY38910Wrapper -> setValue ((size_t) _selectedControlRegister, v);
			}

			break;

		// This register is just to read data not to write, so nothing to do here...
		case 0x02:
			break;

		// It shouldn't be here...
		// Any case these registers are not mapped in the TMS99xxFamily
		// But it is not erro getting this point, just not doing anything...
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
		// These two regiters can not be read...
		// So trying to do so might likely return the last value in the data bus...
		// So far, it is simulated returning the default value!
		case 0x00:
		case 0x01:
			break;

		// The action over the control register selected by the previous instruction
		case 0x02:
			{
				result = _AY38910Wrapper -> readValue ((size_t) _selectedControlRegister);
			}

			break;

		// It shouldn't be here...
		// Any case these registers are not mapped in the TMS99xxFamily
		// But it is not erro getting this point, just not doing anything...
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
