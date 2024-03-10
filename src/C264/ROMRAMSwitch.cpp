#include <C264/ROMRAMSwitch.hpp>
#include <C264/ROMRAMSwitchRegisters.hpp>
#include <C264/Memory.hpp>
#include <C264/TED.hpp>

//
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
		dynamic_cast <C264::Memory*> (memoryRef ()) -> 
			setConfiguration (_ROMRAMSwitchRegisters -> configurationROM ().value ());

	return (true);
}

// ---
void C264::ROMRAMSwitch::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	if (C264::TED::_ROMACCESSCHANGED)
		_ROMRAMSwitchRegisters -> allowROMConfiguration (evnt.value () == 1);
}
