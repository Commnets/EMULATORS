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

	result.add ("MEMORYCFG",	_memoryConfiguration);

	return (result);
}

// ---
void C264::C1C2SelectorRegisters::setValue (size_t p, const MCHEmul::UByte& v)
{ 
	MCHEmul::ChipRegisters::setValue (p, v);

	// The configuration selected will depend on the element accesed...
	_memoryConfiguration = p & 0x0f;

	_configurationChanged = true;
}

// ---
void C264::C1C2SelectorRegisters::initializeInternalValues ()
{
	// The standard configuration is C1=0 and C2=0
	setValue (0x00, 0 /** The byte is the same. */);
}
