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
void C264::ROMRAMSwitch::setBasicROMAccessConfiguration ()
{
	assert (_ROMRAMSwitchRegisters != nullptr);

	_ROMRAMSwitchRegisters -> allowROMConfiguration (true);
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
		if (deepDebugActive ())
		{
			*_deepDebugFile
				// Where
				<< "FDDO,X\t" 
				// When
				<< std::to_string (cpu -> clockCycles ()) << "\t" // clock cycles at that point
				// What
				<< "Info cycle\t\t"
				// Data
				<< "LOROM:"
				<< ((_ROMRAMSwitchRegisters -> configurationROM ().value () != MCHEmul::UByte::_FF)
						? std::to_string (_ROMRAMSwitchRegisters -> configurationROM ().value () & 0x03)
						: "-") << ", "
				<< "HIROM:"
				<< ((_ROMRAMSwitchRegisters -> configurationROM ().value () != MCHEmul::UByte::_FF)
						? std::to_string ((_ROMRAMSwitchRegisters -> configurationROM ().value () & 0x0c) >> 2) 
						: "-") << "\n";

			dynamic_cast <C264::Memory*> (memoryRef ()) -> 
				setConfiguration (_ROMRAMSwitchRegisters -> configurationROM ().value ());
		}
	}

	return (true);
}

// ---
void C264::ROMRAMSwitch::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	if (evnt.id () == C264::TED::_ROMACCESSCHANGED)
		_ROMRAMSwitchRegisters -> allowROMConfiguration (evnt.value () == 1);
}
