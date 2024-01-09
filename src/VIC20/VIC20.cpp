#include <VIC20/VIC20.hpp>
#include <VIC20/VIA1.hpp>
#include <VIC20/VIA2.hpp>
#include <VIC20/Screen.hpp>
#include <VIC20/Sound.hpp>
#include <VIC20/OSIO.hpp>
#include <VIC20/DatasettePort.hpp>
#include <VIC20/ExpansionPort.hpp>
#include <VIC20/Cartridge.hpp>
#include <COMMODORE/VICI/VICI.hpp>
#include <F6500/C6502.hpp>

// ---
VIC20::CommodoreVIC20::CommodoreVIC20 (VIC20::Memory::Configuration cfg, 
		VIC20::CommodoreVIC20::VisualSystem vS, const std::string& lg)
	: COMMODORE::Computer 
		(new F6500::C6502 (0 /** Only one micro. */),
		 VIC20::CommodoreVIC20::standardChips (vS, lg),
		 new VIC20::Memory (cfg, lg),
		 VIC20::CommodoreVIC20::standardDevices (vS),
		 vS == VIC20::CommodoreVIC20::VisualSystem::_PAL ? _PALCLOCK : _NTSCCLOCK,
		 { }, { }, // The VIC20 emulation has been done without neither Buses nor Wires!
		 { { "Name", "VIC20" },
		   { "Manufacturer", "Commodore Business Machines CBM" },
		   { "Year", "1980" }
		 }),
	  _visualSystem (vS)
{
	setConfiguration (cfg, false /** Not restart at initialization. */);
}

// ---
bool VIC20::CommodoreVIC20::initialize (bool iM)
{
	bool result = MCHEmul::Computer::initialize (iM);
	if (!result)
		return (false);

	// Depending on the memory configuration, 
	// the registers that point out to the char and color memory must be adapted!
	setConfiguration (static_cast <VIC20::Memory*> (memory ()) -> configuration (), false /** Not restart. */);

	// It is also needed to observe the expansion port...
	// Events when it is disonnected and connected are sent and with many implications
	// in the structure of the memory...
	observe (dynamic_cast <COMMODORE::ExpansionIOPort*> (device (COMMODORE::ExpansionIOPort::_ID)));

	// Check whether there is an expansion element inserted in the expansion port
	// If it is, it's info is loaded if any...
	VIC20::Cartridge* cT = 
		dynamic_cast <VIC20::Cartridge*> (dynamic_cast <COMMODORE::ExpansionIOPort*> 
			(device (COMMODORE::ExpansionIOPort::_ID)) -> expansionElement ());
	if (cT != nullptr)
		cT -> dumpDataInto (dynamic_cast <VIC20::Memory*> (memory ()), memory () -> activeView ());

	return (true);
}

// ---
void VIC20::CommodoreVIC20::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
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
void VIC20::CommodoreVIC20::setConfiguration (VIC20::Memory::Configuration cfg, bool rs) 
{
	static_cast <VIC20::Memory*> (memory ()) -> setConfiguration (cfg);

	// Restart?
	if (rs)
	{
		setExit (true);
		setRestartAfterExit (true, 9999 /** Big enough */);
	}
}

// ---
MCHEmul::Chips VIC20::CommodoreVIC20::standardChips 
	(VIC20::CommodoreVIC20::VisualSystem vS, const std::string& sS)
{
	MCHEmul::Chips result;

	// The VIA1 & 2
	result.insert (MCHEmul::Chips::value_type (VIC20::VIA1::_ID, (MCHEmul::Chip*) new VIC20::VIA1));
	result.insert (MCHEmul::Chips::value_type (VIC20::VIA2::_ID, (MCHEmul::Chip*) new VIC20::VIA2));

	// The VICI created will depend on whether the visualization is PAL or NTSC...
	// Some how it is also controlled by VIA II
	COMMODORE::VICI* vic = (COMMODORE::VICI*) ((vS == VIC20::CommodoreVIC20::VisualSystem::_NTSC) 
		? (COMMODORE::VICI*) new COMMODORE::VICI_NTSC 
			(VIC20::Memory::_CPU_VIEW, new COMMODORE::VICISoundSimpleLibWrapper 
				(VIC20::CommodoreVIC20::_NTSCCLOCK, COMMODORE::VICI::_SOUNDSAMPLINGCLOCK)) 
		: (COMMODORE::VICI*) new COMMODORE::VICI_PAL
			(VIC20::Memory::_CPU_VIEW, new COMMODORE::VICISoundSimpleLibWrapper 
				(VIC20::CommodoreVIC20::_PALCLOCK, COMMODORE::VICI::_SOUNDSAMPLINGCLOCK)));
	result.insert (MCHEmul::Chips::value_type (COMMODORE::VICI::_ID, (MCHEmul::Chip*) vic));
	
	// The sound chip doesn't exist in VIC20. 
	// It is part of the VICI.
	// However, he emulation needs to treat it as an independent chip...
	result.insert (MCHEmul::Chips::value_type (COMMODORE::VICI::SoundFunction::_ID, vic -> soundFunction ()));

	return (result);
}

// ---
MCHEmul::IODevices VIC20::CommodoreVIC20::standardDevices (VIC20::CommodoreVIC20::VisualSystem vS)
{
	MCHEmul::IODevices result;

	// The very basic systems...
	// They are really part of the system, with no simulated connections at all!
	result.insert (MCHEmul::IODevices::value_type (VIC20::Screen::_ID, 
		(MCHEmul::IODevice*) ((vS == VIC20::CommodoreVIC20::VisualSystem::_NTSC) 
			? (VIC20::Screen*) new VIC20::ScreenNTSC : (VIC20::Screen*) new VIC20::ScreenPAL)));
	result.insert (MCHEmul::IODevices::value_type (VIC20::SoundSystem::_ID, new VIC20::SoundSystem));
	result.insert (MCHEmul::IODevices::value_type (VIC20::InputOSSystem::_ID, new VIC20::InputOSSystem));

	// The different ports: 4!
	// The port where usually the datasette is connected...
	result.insert (MCHEmul::IODevices::value_type (COMMODORE::DatasetteIOPort::_ID, new VIC20::DatasetteIOPort));
	// The port where the floppy disk & printers are connected...
	result.insert (MCHEmul::IODevices::value_type (COMMODORE::SerialIOPort::_ID, new COMMODORE::SerialIOPort));
	// The port where the cartriges are connected...
	result.insert (MCHEmul::IODevices::value_type (COMMODORE::ExpansionIOPort::_ID, new VIC20::ExpansionIOPort));
	// The port where very specific devices are connected...
	result.insert (MCHEmul::IODevices::value_type (COMMODORE::UserIOPort::_ID, new COMMODORE::UserIOPort));

	return (result);
}
