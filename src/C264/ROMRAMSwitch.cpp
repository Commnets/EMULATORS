#include <C264/ROMRAMSwitch.hpp>
#include <C264/ROMRAMSwitchRegisters.hpp>
#include <C264/Memory.hpp>
#include <C264/TED.hpp>

// ---
C264::ROMRAMSwitch::ROMRAMSwitch ()
	: MCHEmul::Chip (_ID,
		{ { "Name", "FDD0 simulation" } }),
	  _ROMRAMSwitchRegisters (nullptr)
{ 
	// Nothing else to do...
}

// ---
bool C264::ROMRAMSwitch::initialize ()
{
	assert (memoryRef () != nullptr);

	// The parent is not initialized because the registers are not the parent ones...

	// Gets the memory block dedicated to this register...
	if (!(_ROMRAMSwitchRegisters =
		dynamic_cast <C264::ROMRAMSwitchRegisters *> 
			(memoryRef () -> subset (C264::ROMRAMSwitchRegisters::_ROMRAMSWITCHREGS_SUBSET))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_ROMRAMSwitchRegisters -> initialize ();

	return (true);
}

// ---
bool C264::ROMRAMSwitch::simulate (MCHEmul::CPU* cpu)
{
	assert (_ROMRAMSwitchRegisters != nullptr);

	if (_ROMRAMSwitchRegisters -> configurationChanged ())
	{ 
		_IFDEBUG debugROMRAMSwitchCycle (cpu);

		// The memory is always a C264::Memory! (use of static to speed up)
		static_cast <C264::Memory*> (memoryRef ()) -> 
			setROMactive (_ROMRAMSwitchRegisters -> ROMActive () /** But what changes is the status of the ROM. */);
	}

	return (true);
}

// ---
void C264::ROMRAMSwitch::debugROMRAMSwitchCycle (MCHEmul::CPU* cpu)
{
	assert (_deepDebugFile != nullptr);

	*_deepDebugFile
		// Where
		<< "FF3E/FF3F,X\t" 
		// When
		<< std::to_string (cpu -> clockCycles ()) << "\t" // clock cycles at that point
		// What
		<< "Info cycle\t\t"
		// Data
		<< "ROM Active:"
		<< (_ROMRAMSwitchRegisters -> ROMActive () ? "YES" : "NO") << "\n";
}
