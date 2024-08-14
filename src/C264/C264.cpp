#include <C264/C264.hpp>
#include <C264/Screen.hpp>
#include <C264/Sound.hpp>
#include <C264/OSIO.hpp>
#include <C264/DatasettePort.hpp>
#include <C264/ExpansionPort.hpp>
#include <C264/C6529B1.hpp>
#include <C264/C6529B2.hpp>
#include <C264/TED.hpp>
#include <C264/ROMRAMSwitch.hpp>
#include <C264/Cartridge.hpp>

// ---
C264::Commodore264::Commodore264 (const MCHEmul::Chips& cps, MCHEmul::Memory* m, const MCHEmul::IODevices& dvs, 
		C264::Commodore264::VisualSystem vS, const std::string& lng)
	: COMMODORE::Computer 
		(new F6500::C7501 (0 /** Only one micro. */),
		 cps,
		 m,
		 dvs,
		 (vS == C264::Commodore264::VisualSystem::_PAL) ? _PALCLOCK : _NTSCCLOCK,
		 { }, { }, // The C264 series emulation has been done without neither Buses nor Wires!
		 { { "Name", "C264 Series (C116/C16/CPlus4)" },
		   { "Manufacturer", "Commodore Business Machines CBM" },
		   { "Year", "1984" }
		 }),
	  _visualSystem (vS),
	  _configuration (MCHEmul::UByte::_0)
{
	assert (_memory != nullptr);

	setConfiguration (_configuration, false);
}

// ---
bool C264::Commodore264::initialize (bool iM)
{
	bool result = COMMODORE::Computer::initialize (iM);
	if (!result)
		return (false);

	// Depending on the configuration of the memory 
	// zones of the 64k memory will or not be available as RAM...
	setConfiguration (static_cast <C264::Memory*> (memory ()) -> configuration (), false /** Not restart. */);

	// It is also needed to observe the expansion port...
	// Events when it is disonnected and connected are sent and with many implications
	// in the structure of the memory...
	observe (dynamic_cast <COMMODORE::ExpansionIOPort*> (device (COMMODORE::ExpansionIOPort::_ID)));
	// The ted has to observe the events comming from the C6521B1 chip on regards 
	// changes in the keyboard matrix...
	chip (COMMODORE::TED::_ID) -> observe (chip (C264::C6529B1::_ID));

	// The ROMRAMSwitch observes the TED
	// This chip is not the PLA but at the end groups all related functions of this!
	chip (C264::ROMRAMSwitch::_ID) -> observe (chip (COMMODORE::TED::_ID));
	// and that "virtual" chip has to be set up for not changes...
	dynamic_cast <C264::ROMRAMSwitch*> (chip (C264::ROMRAMSwitch::_ID)) -> setBasicROMAccessConfiguration ();

	// TODO: The IOPorRegister has to be observed...

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
void C264::Commodore264::setConfiguration (const MCHEmul::UByte& cfg, bool rs) 
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
MCHEmul::Chips C264::Commodore264::standardChips (const std::string& sS, C264::Commodore264::VisualSystem vS)
{
	MCHEmul::Chips result;

	// The TED...
	// The one to create will depend on the type of visualization system...
	// Notice that the sound chip frequency is multiplied by 16 in NTSC systems and by 20 in PAL systems
	C264::TED* ted = 
		vS == C264::Commodore264::VisualSystem::_PAL 
			? (C264::TED*) new C264::TED_PAL 
				(C264::Memory::_CPU_VIEW, new COMMODORE::TEDSoundSimpleLibWrapper 
					((vS == C264::Commodore264::VisualSystem::_PAL) 
						? (C264::Commodore264::_PALCLOCK * 20) : (C264::Commodore264::_NTSCCLOCK * 16),
					 COMMODORE::TED::_SOUNDSAMPLINGCLOCK))
			: (C264::TED*) new C264::TED_NTSC
				(C264::Memory::_CPU_VIEW, new COMMODORE::TEDSoundSimpleLibWrapper 
					((vS == C264::Commodore264::VisualSystem::_PAL) 
						? (C264::Commodore264::_PALCLOCK * 20) : (C264::Commodore264::_NTSCCLOCK * 16), 
					 COMMODORE::TED::_SOUNDSAMPLINGCLOCK));
	result.insert (MCHEmul::Chips::value_type (COMMODORE::TED::_ID, (MCHEmul::Chip*) ted));
	
	// The sound chip doesn't exist in C264 series. 
	// It is part of the TED.
	// However, he emulation needs to treat it as an independent chip...
	result.insert (MCHEmul::Chips::value_type (COMMODORE::TED::SoundFunction::_ID, ted -> soundFunction ()));

	// This intermediate chip is just used to latch the value of the keyboard matrix...
	result.insert (MCHEmul::Chips::value_type (C264::C6529B1::_ID, new C264::C6529B1));

	// This chip is "virtual". It doesn't exist in the real C264 series. 
	// Althought it could be understood as part of the PLA!
	// It is to manage the changes in the ROM/RAM configuration
	result.insert (MCHEmul::Chips::value_type (C264::ROMRAMSwitch::_ID, new C264::ROMRAMSwitch));

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
MCHEmul::Chips C264::Commodore16_116::standardChips (const std::string& sS, C264::Commodore264::VisualSystem vS)
{
	MCHEmul::Chips result = 
		std::move (C264::Commodore264::standardChips (sS, vS));

	return (result);
}

// ---
MCHEmul::IODevices C264::Commodore16_116::standardDevices (C264::Commodore264::VisualSystem vS)
{
	MCHEmul::IODevices result =
		std::move (C264::Commodore264::standardDevices (vS));

	// The screen with the C16/C116 text...
	result.insert (MCHEmul::IODevices::value_type (C264::Screen::_ID, 
		(vS == C264::Commodore264::VisualSystem::_PAL) 
			? (MCHEmul::IODevice*) new C264::ScreenPAL ("C16/116") 
			: (MCHEmul::IODevice*) new C264::ScreenNTSC ("C16/116")));

	return (result);
}

// ---
MCHEmul::Chips C264::CommodorePlus4::standardChips (const std::string& sS, C264::Commodore264::VisualSystem vS)
{
	MCHEmul::Chips result = 
		std::move (C264::Commodore264::standardChips (sS, vS));

	// The ACIA...
	result.insert (MCHEmul::Chips::value_type (COMMODORE::ACIA::_ID, new COMMODORE::ACIA));
	// ...and the other C6529...
	result.insert (MCHEmul::Chips::value_type (C264::C6529B2::_ID, new C264::C6529B2));

	return (result);
}

// ---
MCHEmul::IODevices C264::CommodorePlus4::standardDevices (C264::Commodore264::VisualSystem vS)
{
	MCHEmul::IODevices result =
		std::move (C264::Commodore264::standardDevices (vS));

	// The screen with the Plus/4 text...
	result.insert (MCHEmul::IODevices::value_type (C264::Screen::_ID, 
		(vS == C264::Commodore264::VisualSystem::_PAL) 
			? (MCHEmul::IODevice*) new C264::ScreenPAL ("Plus/4") 
			: (MCHEmul::IODevice*) new C264::ScreenNTSC ("Plus/4")));

	return (result);
}
