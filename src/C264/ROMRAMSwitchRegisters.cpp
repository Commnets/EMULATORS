#include <C264/ROMRAMSwitchRegisters.hpp>

// ---
void C264::ROMRAMSwitchRegisters::allowROMConfiguration (bool a)
{ 
	_configurationROM = 
		(_allowROMConfiguration = a) 
			? _latchedConfigurationROM		// ..the last configuration latched...
			: MCHEmul::UByte::_FF;			// Meaning nothing but all RAMS active...
	
	_configurationChanged = true;
}

// ---
void C264::ROMRAMSwitchRegisters::initialize ()
{
	MCHEmul::ChipRegisters::initialize ();

	initializeInternalValues ();
}

// ---
MCHEmul::InfoStructure C264::ROMRAMSwitchRegisters::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::ChipRegisters::getInfoStructure ());

	result.add ("CONFIGURATION", _configurationROM);

	return (result);
}

// ---
void C264::ROMRAMSwitchRegisters::setValue (size_t p, const MCHEmul::UByte& v)
{ 
	MCHEmul::ChipRegisters::setValue (p, v); 

	size_t pp =  p % 0x10;

	// Only if it is allowed...
	if (_allowROMConfiguration)
	{
		_configurationROM = (unsigned char) pp;

		_configurationChanged = true;
	}

	_latchedConfigurationROM = (unsigned char) pp;
}

// ---
void C264::ROMRAMSwitchRegisters::initializeInternalValues ()
{
	_allowROMConfiguration = true;

	_latchedConfigurationROM = _configurationROM = 0; // Meaning BASIC and KERNEL active!

	_configurationChanged = false; 
}
