#include <C264/ROMRAMSwitchRegisters.hpp>

// ---
void C264::ROMRAMSwitchRegisters::allowROMConfiguration (bool a)
{ 
	_configurationROM = 
		(_allowROMConfiguration = a) 
			? MCHEmul::UByte::_0	// BASIC and Kernel until anything different is said...
			: MCHEmul::UByte::_FF;	// Meaning nothing all RAMS active...
	
	_configurationChanged = true;
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

	// Only if it is allowed...
	if (_allowROMConfiguration)
	{
		_configurationROM = (unsigned char) p;

		_configurationChanged = true;
	}
}

// ---
void C264::ROMRAMSwitchRegisters::initializeInternalValues ()
{
	_allowROMConfiguration = true;

	_configurationROM = 0; // Meaning BASIC and KERNEL active!

	_configurationChanged = false; 
}
