#include <C264/C1C2SelectorRegisters.hpp>


// ---
C264::C1C2SelectorRegisters::C1C2SelectorRegisters 
		(MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (_C1C2SELECTORREGS_SUBSET, ps, pp, a, s),
	  _configurationChanged (false)
{ 
	initializeInternalValues ();

	// The effect in any change in the configuration has to be inmediate!
	// So, there is no possibility to buffer the changes...
	// IMPORTANT:
	// Imagine a instruction setting a new configuration...
	// but, if it were buffered, the seeting won't really happen until the beginning of the next instruction...
	// At that point the change is "marked" but it is not really executed until the "Selector" was exectuted
	// Because it is simulated like a chip, the simulate method will be invoked after the execution of that
	// previous instruction. If that next instruction tried to access positions of that new configuration
	// it won't happen, because it will not be active yet!
	setBufferMemorySetCommands (false);
}

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
