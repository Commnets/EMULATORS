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
C264::Commodore264::Commodore264 (unsigned int mT, const std::string& lg,
		const MCHEmul::Chips& cps, const MCHEmul::IODevices& dvs)
	: COMMODORE::Computer 
		(new F6500::C6502 (0 /** Only one micro. */),
		 cps,
		 new C264::Memory (mT, lg),
		 dvs,
		 _CLOCK,
		 { }, { }, // The C264 series emulation has been done without neither Buses nor Wires!
		 { { "Name", "C264 Series (C116/C16/CPlus4)" },
		   { "Manufacturer", "Commodore Business Machines CBM" },
		   { "Year", "1984" }
		 })
{
	setMachineType (mT, false /** Not restart at initialization. */);
}

// ---
bool C264::Commodore264::initialize (bool iM)
{
	bool result = MCHEmul::Computer::initialize (iM);
	if (!result)
		return (false);

	// Depending on the type of machine different 
	// zones of the 64k memory will or not be available as RAM...
	setMachineType (static_cast <C264::Memory*> (memory ()) -> machineType (), false /** Not restart. */);

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
void C264::Commodore264::setMachineType (unsigned int mT, bool rs) 
{
	static_cast <C264::Memory*> (memory ()) -> setMachineType (mT);

	// Restart?
	if (rs)
	{
		setExit (true);
		setRestartAfterExit (true, 9999 /** Big enough */);
	}
}

// ---
MCHEmul::Chips C264::Commodore264::standardChips (const std::string& sS)
{
	MCHEmul::Chips result;

	// The TED...
	COMMODORE::TED* ted = (COMMODORE::TED*) new COMMODORE::TED 
			(C264::Memory::_CPU_VIEW, new COMMODORE::TEDSoundSimpleLibWrapper 
				(C264::Commodore264::_CLOCK, COMMODORE::TED::_SOUNDSAMPLINGCLOCK));
	result.insert (MCHEmul::Chips::value_type (COMMODORE::TED::_ID, (MCHEmul::Chip*) ted));
	
	// The sound chip doesn't exist in C264 series. 
	// It is part of the TED.
	// However, he emulation needs to treat it as an independent chip...
	result.insert (MCHEmul::Chips::value_type (COMMODORE::TED::SoundFunction::_ID, ted -> soundFunction ()));

	return (result);
}

// ---
MCHEmul::IODevices C264::Commodore264::standardDevices ()
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
MCHEmul::Chips C264::Commodore16_116::standardChips (const std::string& sS)
{
	MCHEmul::Chips result = 
		std::move (C264::Commodore264::standardChips (sS));

	// TODO

	return (result);
}

// ---
MCHEmul::IODevices C264::Commodore16_116::standardDevices ()
{
	MCHEmul::IODevices result =
		std::move (C264::Commodore264::standardDevices ());

	// The screen with the C16 text...
	result.insert (MCHEmul::IODevices::value_type (C264::Screen::_ID, 
		(MCHEmul::IODevice*) new C264::Screen ("C16/116")));

	// TODO

	return (result);
}

// ---
MCHEmul::Chips C264::CommodorePlus4::standardChips (const std::string& sS)
{
	MCHEmul::Chips result = 
		std::move (C264::Commodore264::standardChips (sS));

	// TODO

	return (result);
}

// ---
MCHEmul::IODevices C264::CommodorePlus4::standardDevices ()
{
	MCHEmul::IODevices result =
		std::move (C264::Commodore264::standardDevices ());

	// The screen with the C116 text...
	result.insert (MCHEmul::IODevices::value_type (C264::Screen::_ID, 
		(MCHEmul::IODevice*) new C264::Screen ("Plus/4")));

	return (result);
}
