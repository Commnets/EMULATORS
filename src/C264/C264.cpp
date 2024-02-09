#include <C264/C264.hpp>
#include <C264/Screen.hpp>
#include <C264/Sound.hpp>
#include <C264/OSIO.hpp>
#include <C264/DatasettePort.hpp>
#include <C264/ExpansionPort.hpp>
#include <C264/Cartridge.hpp>
#include <COMMODORE/VICI/VICI.hpp>
#include <F6500/C6502.hpp>

// ---
C264::Commodore264::Commodore264 (unsigned int cfg, 
		C264::Commodore264::VisualSystem vS, const std::string& lg,
		const MCHEmul::Chips& cps, const MCHEmul::IODevices& dvs)
	: COMMODORE::Computer 
		(new F6500::C6502 (0 /** Only one micro. */),
		 cps,
		 new C264::Memory (cfg, lg),
		 dvs,
		 vS == C264::Commodore264::VisualSystem::_PAL ? _PALCLOCK : _NTSCCLOCK,
		 { }, { }, // The C264 series emulation has been done without neither Buses nor Wires!
		 { { "Name", "C264 Series (C264/CPlus4/C116)" },
		   { "Manufacturer", "Commodore Business Machines CBM" },
		   { "Year", "1980" }
		 }),
	  _visualSystem (vS)
{
	setConfiguration (cfg, false /** Not restart at initialization. */);
}

// ---
bool C264::Commodore264::initialize (bool iM)
{
	bool result = MCHEmul::Computer::initialize (iM);
	if (!result)
		return (false);

	// Depending on the memory configuration, 
	// the registers that point out to the char and color memory must be adapted!
	setConfiguration (static_cast <C264::Memory*> (memory ()) -> configuration (), false /** Not restart. */);

	// It is also needed to observe the expansion port...
	// Events when it is disonnected and connected are sent and with many implications
	// in the structure of the memory...
	observe (dynamic_cast <COMMODORE::ExpansionIOPort*> (device (COMMODORE::ExpansionIOPort::_ID)));

	// Check whether there is an expansion element inserted in the expansion port
	// If it is, it's info is loaded if any...
	C264::Cartridge* cT = 
		dynamic_cast <C264::Cartridge*> (dynamic_cast <COMMODORE::ExpansionIOPort*> 
			(device (COMMODORE::ExpansionIOPort::_ID)) -> expansionElement ());
	if (cT != nullptr)
		cT -> dumpDataInto (dynamic_cast <C264::Memory*> (memory ()), memory () -> activeView ());

	return (true);
}

// ---
void C264::Commodore264::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	// When a expansion element is inserted, then everything has to restart...
	if (evnt.id () == COMMODORE::ExpansionIOPort::_EXPANSIONELEMENTIN ||
		evnt.id () == COMMODORE::ExpansionIOPort::_EXPANSIONELEMENTOUT)
	{
		setExit (true);
		setRestartAfterExit (true, 9999 /** Big enough */);
	}
}

// ---
void C264::Commodore264::setConfiguration (unsigned int cfg, bool rs) 
{
	static_cast <C264::Memory*> (memory ()) -> setConfiguration (cfg);

	// Restart?
	if (rs)
	{
		setExit (true);
		setRestartAfterExit (true, 9999 /** Big enough */);
	}
}

// ---
MCHEmul::Chips C264::Commodore264::standardChips 
	(C264::Commodore264::VisualSystem vS, const std::string& sS)
{
	MCHEmul::Chips result;

	// TODO

	return (result);
}

// ---
MCHEmul::IODevices C264::Commodore264::standardDevices (C264::Commodore264::VisualSystem vS)
{
	MCHEmul::IODevices result;

	// The very basic systems...
	// They are really part of the system, with no simulated connections at all!
	// ...The screem will depend on the computer...as it shows how it is inthe header...
	result.insert (MCHEmul::IODevices::value_type (C264::SoundSystem::_ID, new C264::SoundSystem));
	result.insert (MCHEmul::IODevices::value_type (C264::InputOSSystem::_ID, new C264::InputOSSystem));

	// Any C264 has a minimum of 3 ports!
	// Someone else could be added attending to the type of specific computer...
	// The port where usually the datasette is connected...
	result.insert (MCHEmul::IODevices::value_type (COMMODORE::DatasetteIOPort::_ID, new C264::DatasetteIOPort));
	// The port where the floppy disk & printers are connected...
	result.insert (MCHEmul::IODevices::value_type (COMMODORE::SerialIOPort::_ID, new COMMODORE::SerialIOPort));
	// The port where the cartriges are connected...
	result.insert (MCHEmul::IODevices::value_type (COMMODORE::ExpansionIOPort::_ID, new C264::ExpansionIOPort));

	return (result);
}

// ---
MCHEmul::Chips C264::Commodore16::standardChips 
	(C264::Commodore264::VisualSystem vS, const std::string& sS)
{
	MCHEmul::Chips result = 
		std::move (C264::Commodore264::standardChips (vS, sS));

	// TODO

	return (result);
}

// ---
MCHEmul::IODevices C264::Commodore16::standardDevices (C264::Commodore264::VisualSystem vS)
{
	MCHEmul::IODevices result =
		std::move (C264::Commodore264::standardDevices (vS));

	// The screen with the C16 text...
	result.insert (MCHEmul::IODevices::value_type (C264::Screen::_ID, 
		(MCHEmul::IODevice*) ((vS == C264::Commodore264::VisualSystem::_NTSC) 
			? (C264::Screen*) new C264::ScreenNTSC ("C16") : (C264::Screen*) new C264::ScreenPAL ("C16"))));

	// TODO

	return (result);
}

// ---
MCHEmul::Chips C264::Commodore116::standardChips 
	(C264::Commodore264::VisualSystem vS, const std::string& sS)
{
	MCHEmul::Chips result = 
		std::move (C264::Commodore264::standardChips (vS, sS));

	// TODO

	return (result);
}

// ---
MCHEmul::IODevices C264::Commodore116::standardDevices (C264::Commodore264::VisualSystem vS)
{
	MCHEmul::IODevices result =
		std::move (C264::Commodore264::standardDevices (vS));

	// The screen with the C116 text...
	result.insert (MCHEmul::IODevices::value_type (C264::Screen::_ID, 
		(MCHEmul::IODevice*) ((vS == C264::Commodore264::VisualSystem::_NTSC) 
			? (C264::Screen*) new C264::ScreenNTSC ("C116") : (C264::Screen*) new C264::ScreenPAL ("C116"))));

	return (result);
}

// ---
MCHEmul::Chips C264::CommodorePlus4::standardChips 
	(C264::Commodore264::VisualSystem vS, const std::string& sS)
{
	MCHEmul::Chips result = 
		std::move (C264::Commodore264::standardChips (vS, sS));

	// TODO

	return (result);
}

// ---
MCHEmul::IODevices C264::CommodorePlus4::standardDevices (C264::Commodore264::VisualSystem vS)
{
	MCHEmul::IODevices result =
		std::move (C264::Commodore264::standardDevices (vS));

	// The screen with the C116 text...
	result.insert (MCHEmul::IODevices::value_type (C264::Screen::_ID, 
		(MCHEmul::IODevice*) ((vS == C264::Commodore264::VisualSystem::_NTSC) 
			? (C264::Screen*) new C264::ScreenNTSC ("Plus/4") : (C264::Screen*) new C264::ScreenPAL ("Plus/4"))));

	return (result);
}
