#include <C64/C64.hpp>
#include <C64/Memory.hpp>
#include <C64/SFChip.hpp>
#include <C64/PLA.hpp>
#include <C64/Screen.hpp>
#include <C64/Sound.hpp>
#include <C64/OSIO.hpp>
#include <C64/DatasettePort.hpp>
#include <C64/ExpansionPort.hpp>
#include <C64/Cartridge.hpp>
#include <COMMODORE/UserPort.hpp>
#include <F6500/C6510.hpp>

// ---
C64::Commodore64::Commodore64 (C64::Commodore64::VisualSystem vS, const std::string& lang)
	: COMMODORE::Computer 
		(new F6500::C6510 (),
		 C64::Commodore64::standardChips (vS),
		 new C64::Memory (lang), // The memory is loaded with different info depending on the language...
		 C64::Commodore64::standardDevices (vS),
		 vS == C64::Commodore64::VisualSystem::_PAL ? 985000 /* 0.986 MHz */: 1023000 /** 1.023 MHz */,
		 { { "Name", "Commodore 64" },
		   { "Manufacturer", "Commodore Business Machines CBM" },
		   { "Year", "1980" }
		 }),
	  _visualSystem (vS)
{
	// Nothing else to do...
}

// ---
bool C64::Commodore64::initialize (bool iM)
{
	bool result = MCHEmul::Computer::initialize (iM);
	if (!result)
		return (false);

	// Both chips CIAII and VICII are link somehow
	// Because the banks connected at VICII are determined in CIA chip.
	chip (COMMODORE::VICII::_ID) -> observe (chip (C64::CIA2::_ID));
	// It is needed to observe the expansion port...
	// Events when it is disonnected and connected are sent and with many implications
	// in the structure of the memory...
	observe (dynamic_cast <COMMODORE::ExpansionIOPort*> (device (COMMODORE::ExpansionIOPort::_ID)));
	// The C64 IO Ports (Memory location 1) and the PLA are linked...
	chip (C64::PLA::_ID) -> observe (chip (C64::SpecialFunctionsChip::_ID));
	// The expansion port and the PLA are linked...
	chip (C64::PLA::_ID) -> observe (device (C64::ExpansionIOPort::_ID));

	// Check whether there is an expansion element inserted in the expansion port
	// If it is, it's info is loaded if any...
	COMMODORE::ExpansionPeripheral* eP = 
		dynamic_cast <COMMODORE::ExpansionIOPort*> (device (COMMODORE::ExpansionIOPort::_ID)) -> expansionElement ();
	if (eP != nullptr && !eP -> data ()._data.empty ())
		dynamic_cast <C64::Cartridge*> (eP) -> dumpDataInto 
			(memory () -> subset (C64::Memory::_EXPANSIONROML_SUBSET),
			 memory () -> subset (C64::Memory::_EXPANSIONROMH1_SUBSET),
			 memory () -> subset (C64::Memory::_EXPANSIONROMH2_SUBSET));

	return (true);
}

// ---
void C64::Commodore64::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	// When a expansion element is inserted, then everything has to restart...
	if (evnt.id () == COMMODORE::ExpansionIOPort::_EXPANSIONELEMENTIN ||
		evnt.id () == COMMODORE::ExpansionIOPort::_EXPANSIONELEMENTOUT)
	{
		setExit (true);
		setRestartAfterExit (true, 1 /** Without memory. */);
	}
}

// ---
MCHEmul::Chips C64::Commodore64::standardChips (C64::Commodore64::VisualSystem vS)
{
	MCHEmul::Chips result;

	// The Special Control Chip...
	// Invented by me to maintain the architcture, but softly mention at any doc (i guess)!
	result.insert (MCHEmul::Chips::value_type (C64::SpecialFunctionsChip::_ID, (MCHEmul::Chip*) new C64::SpecialFunctionsChip));

	// The CIA1 & 2
	result.insert (MCHEmul::Chips::value_type (C64::CIA1::_ID, (MCHEmul::Chip*) new C64::CIA1));
	result.insert (MCHEmul::Chips::value_type (C64::CIA2::_ID, (MCHEmul::Chip*) new C64::CIA2));

	// The VicII created will depend on whether the visualization is PAL or NTSC...
	// Some how it is also controlled by CIA II and Special Control Chip
	result.insert (MCHEmul::Chips::value_type (COMMODORE::VICII::_ID, 
		(MCHEmul::Chip*) ((vS == C64::Commodore64::VisualSystem::_NTSC) 
			? (C64::VICII*) new COMMODORE::VICII_NTSC (C64::Memory::_VICII_VIEW) 
			: (C64::VICII*) new COMMODORE::VICII_PAL (C64::Memory::_VICII_VIEW))));

	// The SID
	result.insert (MCHEmul::Chips::value_type (COMMODORE::SID::_ID, (MCHEmul::Chip*) new COMMODORE::SID));

	// The PLA
	result.insert (MCHEmul::Chips::value_type (C64::PLA::_ID, (MCHEmul::Chip*) new C64::PLA));

	return (result);
}

// ---
MCHEmul::IODevices C64::Commodore64::standardDevices (C64::Commodore64::VisualSystem vS)
{
	MCHEmul::IODevices result;

	// The very basic systems...
	// They are really part of the system, with no simlated connections at all!
	result.insert (MCHEmul::IODevices::value_type (C64::Screen::_ID, 
		(MCHEmul::IODevice*) ((vS == C64::Commodore64::VisualSystem::_NTSC) 
			? (C64::Screen*) new C64::ScreenNTSC : (C64::Screen*) new C64::ScreenPAL)));
	result.insert (MCHEmul::IODevices::value_type (C64::SoundSystem::_ID, new C64::SoundSystem));
	result.insert (MCHEmul::IODevices::value_type (C64::InputOSSystem::_ID, new C64::InputOSSystem));

	// The different ports
	// The port where usually the datasette is connected...
	result.insert (MCHEmul::IODevices::value_type (C64::DatasetteIOPort::_ID, new C64::DatasetteIOPort));
	// The port where the printers and similar are connected...
	result.insert (MCHEmul::IODevices::value_type (COMMODORE::UserIOPort::_ID, new COMMODORE::UserIOPort));
	// The port where the cartriges are connected...
	result.insert (MCHEmul::IODevices::value_type (C64::ExpansionIOPort::_ID, new C64::ExpansionIOPort));

	return (result);
}
