#include <C264/ROMRAMSwitchRegisters.hpp>

// ---
C264::ROMRAMSwitchRegisters::ROMRAMSwitchRegisters 
		(MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s)
	: MCHEmul::ChipRegisters (_ROMRAMSWITCHREGS_SUBSET, ps, pp, a, s),
	  _configurationChanged (false)
{ 
	initializeInternalValues ();

	// Setup and independent use apply writes directly.
	// Connected C264 initialization enables buffering so TED can apply CPU
	// writes and their mapping effects at the scheduled bus cycle.
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
