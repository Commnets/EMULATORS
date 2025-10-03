#include <C264/C1C2SelectorRegisters.hpp>

// ---
void C264::C1C2SelectorRegisters::initialize ()
{
	MCHEmul::ChipRegisters::initialize ();

	initializeInternalValues ();
}

// ---
MCHEmul::InfoStructure C264::C1C2SelectorRegisters::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::ChipRegisters::getInfoStructure ());

	result.add ("C1Low",	_C1Low);
	result.add ("C1High",	_C1High);
	result.add ("C2Low",	_C2Low);
	result.add ("C2High",	_C2High);

	return (result);
}

// ---
void C264::C1C2SelectorRegisters::setValue (size_t p, const MCHEmul::UByte& v)
{ 
	MCHEmul::ChipRegisters::setValue (p, v); 

	// Whatever register is accessed, the configuration is changed...
	_C1Low	= v.bit (0);
	_C1High = v.bit (1);
	_C2Low	= v.bit (2);
	_C2High = v.bit (3);

	_configurationChanged = true;
}

// ---
void C264::C1C2SelectorRegisters::initializeInternalValues ()
{
	// The standard configuration is C1=0 and C2=0
	setValue (0x00, 0 /** The byte is the same. */);
}
