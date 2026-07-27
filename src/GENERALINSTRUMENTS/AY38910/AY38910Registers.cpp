#include <GENERALINSTRUMENTS/AY38910/AY38910Registers.hpp>

// ---
GENERALINSTRUMENTS::AY38910Registers::AY38910Registers ()
	: MCHEmul::ChipRegisters (GENERALINSTRUMENTS::AY38910Registers::_ID, 
		2 /** 2 bytes per address. */, 3 /** 3 Registers. */),
	  _selectedControlRegister (0x00),
	  _AY38910Wrapper (nullptr),
	  _IOPortInputValues { MCHEmul::UByte::_FF, MCHEmul::UByte::_FF },
	  _IOPortOutputLatches { MCHEmul::UByte::_0, MCHEmul::UByte::_0 },
	  _lastValueRead (MCHEmul::UByte::_0)
	// The rest of the attributes are initialized with the method initializeInternalValues...
{
	setClassName ("AY38910Registers");

	initializeInternalValues ();
}

// ---
GENERALINSTRUMENTS::AY38910Registers::AY38910Registers
		(MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (GENERALINSTRUMENTS::AY38910Registers::_ID, ps, pp, a, s),
	  _selectedControlRegister (0x00),
	  _AY38910Wrapper (nullptr),
	  _IOPortInputValues { MCHEmul::UByte::_FF, MCHEmul::UByte::_FF },
	  _IOPortOutputLatches { MCHEmul::UByte::_0, MCHEmul::UByte::_0 },
	  _lastValueRead (MCHEmul::UByte::_0)
	 // The rest of the attributes are initialized with the method initializeInternalValues...
{
	setClassName ("AY38910Registers");

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

	std::vector <MCHEmul::UByte> cR (0x10, MCHEmul::UByte::_0);
	if (_AY38910Wrapper != nullptr)
	{
		// R14 and R15 are resolved here because the generic sound wrapper
		// does not own the I/O pins connected to those registers.
		for (size_t i = 0; i < 0x0e; i++)
			cR [i] = _AY38910Wrapper -> peekValue (i);
		cR [0x0e] = ioPortValue (GENERALINSTRUMENTS::AY38910Registers::IOPort::_A);
		cR [0x0f] = ioPortValue (GENERALINSTRUMENTS::AY38910Registers::IOPort::_B);
	}

	result.add ("SELECTEDREGISTER",	(int) _selectedControlRegister);
	result.add ("CONTROLREGISTERS",	cR);
	result.add ("PORTAINPUT",		_IOPortInputValues [(size_t) IOPort::_A]);
	result.add ("PORTAOUTPUTLATCH",	_IOPortOutputLatches [(size_t) IOPort::_A]);
	result.add ("PORTAVALUE",		(_AY38910Wrapper != nullptr)
		? ioPortValue (IOPort::_A) : _IOPortInputValues [(size_t) IOPort::_A]);
	result.add ("PORTAOUTPUT",		(_AY38910Wrapper != nullptr)
		? ioPortIsOutput (IOPort::_A) : false);
	result.add ("PORTBINPUT",		_IOPortInputValues [(size_t) IOPort::_B]);
	result.add ("PORTBOUTPUTLATCH",	_IOPortOutputLatches [(size_t) IOPort::_B]);
	result.add ("PORTBVALUE",		(_AY38910Wrapper != nullptr)
		? ioPortValue (IOPort::_B) : _IOPortInputValues [(size_t) IOPort::_B]);
	result.add ("PORTBOUTPUT",		(_AY38910Wrapper != nullptr)
		? ioPortIsOutput (IOPort::_B) : false);
	result.add ("LASTVALUEREAD",	_lastValueRead);

	return (result);
}

// ---
bool GENERALINSTRUMENTS::AY38910Registers::ioPortIsOutput (
	GENERALINSTRUMENTS::AY38910Registers::IOPort p) const
{
	assert (_AY38910Wrapper != nullptr);

	return (_AY38910Wrapper -> readValue (0x07).bit (6 + (size_t) p));
}

// ---
MCHEmul::UByte GENERALINSTRUMENTS::AY38910Registers::ioPortValue (
	GENERALINSTRUMENTS::AY38910Registers::IOPort p) const
{
	return (ioPortIsOutput (p)
		? _IOPortOutputLatches [(size_t) p]
		: _IOPortInputValues [(size_t) p]);
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
				_selectedControlRegister = v.value () & 0x0f;
			}

			break;

		// The action over the control register selected by the previous instruction
		case 0x01:
			{
				assert (_AY38910Wrapper != nullptr);

				// Writes always reach the output latch. If the port is currently an input,
				// the value will become visible only after its direction changes to output.
				if (_selectedControlRegister == 0x0e ||
					_selectedControlRegister == 0x0f)
					_IOPortOutputLatches [_selectedControlRegister - 0x0e] = v;
				else
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
				assert (_AY38910Wrapper != nullptr);

				if (_selectedControlRegister == 0x0e ||
					_selectedControlRegister == 0x0f)
					result = ioPortValue ((GENERALINSTRUMENTS::AY38910Registers::IOPort)
						(_selectedControlRegister - 0x0e));
				else
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
	_selectedControlRegister = 0x00;
	_IOPortInputValues [0] = _IOPortInputValues [1] = MCHEmul::UByte::_FF;
	_IOPortOutputLatches [0] = _IOPortOutputLatches [1] = MCHEmul::UByte::_0;
	_lastValueRead = MCHEmul::UByte::_0;
}
