#include <C264/ROMRAMSwitchRegisters.hpp>

// ---
C264::ROMRAMSwitchRegisters::ROMRAMSwitchRegisters 
		(MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (_ROMRAMSWITCHREGS_SUBSET, ps, pp, a, s),
	  _configurationChanged (false)
{ 
	initializeInternalValues ();

	// The effect in any change in the configuration has to be inmediate!
	// So, there is no possibility to buffer the changes...
	// Imagine a instruction setting a new configuration...
	// but, if it were buffered, the seeting won't really happen until the beginning of the next instruction...
	// At that point the change is "marked" but it is not really executed until the "Selector" was exectuted
	// Because it is simulated like a chip, the simulate method will be invoked after the execution of that
	// previous instruction. If that next instruction tried to access positions of that new configuration
	// it won't happen, because it will not be active yet!
	setBufferMemorySetCommands (false);
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

	result.add ("ROMACTIVE", _ROMActive);

	return (result);
}

// ---
void C264::ROMRAMSwitchRegisters::setValue (size_t p, const MCHEmul::UByte& v)
{ 
	MCHEmul::ChipRegisters::setValue (p, v); 

	size_t pp =  p % 0x02;

	switch (pp)
	{
		// Switch to ROM between positions $8000-$FFFF
		case 0x00: 
			{
				_ROMActive = true;

				_configurationChanged = true;
			}

			break;

		// Switch to RAM between positions $8000-$FFFF
		case 0x01: 
			{
				_ROMActive = false;

				_configurationChanged = true;
			}

			break;

		// This situation never happens...
		default:
			return;
	}
}

// ---
void C264::ROMRAMSwitchRegisters::initializeInternalValues ()
{
	// Switch to ROM between positions $8000-$FFFF
	setValue (0x00, 0 /** The byte is the same. */);
}
