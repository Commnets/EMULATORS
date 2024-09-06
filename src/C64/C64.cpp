#include <C64/C64.hpp>
#include <C64/Memory.hpp>
#include <C64/IO6510PortRegisters.hpp>
#include <C64/VICII.hpp>
#include <C64/PLA.hpp>
#include <C64/Screen.hpp>
#include <C64/Sound.hpp>
#include <C64/OSIO.hpp>
#include <C64/DatasettePort.hpp>
#include <C64/ExpansionPort.hpp>
#include <C64/Cartridge.hpp>
#include <C64/IOExpansionMemory.hpp>
#include <COMMODORE/UserPort.hpp>
#include <F6500/C6510.hpp>

// ---
C64::Commodore64::Commodore64 (C64::Commodore64::VisualSystem vS, const std::string& sS, const std::string& lang)
	: COMMODORE::Computer 
		(new F6500::C6510 (0 /** Only 1 micro. */),
		 C64::Commodore64::standardChips (vS, sS),
		 new C64::Memory (lang), // The memory is loaded with different info depending on the language...
		 C64::Commodore64::standardDevices (vS),
		 vS == C64::Commodore64::VisualSystem::_PAL ? _PALCLOCK : _NTSCCLOCK,
		 { }, { }, // The C64 emulation has been done without neither buses nor wires...
		 { { "Name", "Commodore 64" },
		   { "Manufacturer", "Commodore Business Machines CBM" },
		   { "Year", "1982" }
		 }),
	  _visualSystem (vS)
{
	// Assign the color RAM to the VICII and also the address where it starts...
	static_cast <COMMODORE::VICII*> (chip (COMMODORE::VICII::_ID)) -> setColorRAM 
		(static_cast <C64::Memory*> (memory ()) -> colorRAM (), C64::Memory::_COLORMEMORY_ADDRESS);
}

// ---
bool C64::Commodore64::initialize (bool iM)
{
	// Not to buffer modifications when the memory/chips are initialized!
	// The instruction goes directly to the memory instead through the chip
	// because at this point the registers are not still linked to the chip
	// This is something that happens later when the VICII is initialized...
	static_cast <C64::Memory*> (memory ()) -> vicIIRegisters () -> setBufferRegisters (false);

	bool result = COMMODORE::Computer::initialize (iM);
	if (!result)
		return (false);

	// The connections among chips and devices are set in the method "linkToChips" of the IODevice class
	// Any other type of connetion has to be done at C64 level...

	// Links between chips...
	// Both chips CIAII and VICII are link somehow
	// Because the banks connected at VICII are determined in CIA chip...
	// In the real C64 the communciation happens through the PLA, but here it has been simplified...
	chip (COMMODORE::VICII::_ID) -> observe (memory () -> subset (C64::CIA2Registers::_CIA2_SUBSET));
	// The C64 IO Ports (Memory location 1, bit 0,1,2) and the PLA are linked...
	// This link is used to determine whether the BASIC ROM, KERNEL or CHARROM is or nor visible.
	chip (C64::PLA::_ID) -> observe (memory () -> subset (C64::IO6510PortRegisters::_IO6510REGISTERS_SUBSET));
	// THe CIA1 controls the information of the potentiometers that the SID must reflect!
	static_cast <C64::CIA1*> (chip (C64::CIA1::_ID)) -> 
		linkToSID (static_cast <COMMODORE::SID*> (chip (COMMODORE::SID::_ID)));

	// Limks between memory and devices...
	// But the bit 3 of the same position is where the info to send to the casette is writtem...
	device (COMMODORE::DatasetteIOPort::_ID) -> observe (memory () -> subset (C64::IO6510PortRegisters::_IO6510REGISTERS_SUBSET));
	// But also the bit 4 is where is possible to read whether any key in the datasette has been preseed 
	// and where to know whether the motor is running or not...
	memory () -> subset (C64::IO6510PortRegisters::_IO6510REGISTERS_SUBSET) -> observe (device (COMMODORE::DatasetteIOPort::_ID));
	// The expansion memory is observed by the expansion port
	// which can do different actions attending to the type of peripheral connected...
	device (COMMODORE::ExpansionIOPort::_ID) -> observe 
		(memory () -> subset (C64::IOExpansionMemoryI::_IO1_SUBSET));
	device (COMMODORE::ExpansionIOPort::_ID) -> observe 
		(memory () -> subset (C64::IOExpansionMemoryII::_IO2_SUBSET));

	// C64 direct with devices...
	// It is also needed to observe the expansion port...
	// Events when it is disonnected and connected are sent and with many implications
	// in the structure of the memory...
	observe (dynamic_cast <COMMODORE::ExpansionIOPort*> (device (COMMODORE::ExpansionIOPort::_ID)));

	// ...and finally the special link of the C64 with the cartridge
	// Check whether there is an expansion element inserted in the expansion port
	// If it is, it's info is loaded if any...
	C64::Cartridge* cT =
		dynamic_cast <C64::Cartridge*> (dynamic_cast <COMMODORE::ExpansionIOPort*> 
			(device (COMMODORE::ExpansionIOPort::_ID)) -> expansionElement ());
	if (cT != nullptr)
		cT -> dumpDataInto (dynamic_cast <C64::Memory*> (memory ()), 
			memory () -> activeView (), memory () -> view (C64::Memory::_VICII_VIEW));

	// From now onwards, the VICII buffer the modifications!
	static_cast <C64::Memory*> (memory ()) -> vicIIRegisters () -> setBufferRegisters (true);

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
		setRestartAfterExit (true, 9999 /** Big enough */);
	}
}

// ---
MCHEmul::Chips C64::Commodore64::standardChips (C64::Commodore64::VisualSystem vS, const std::string& sS)
{
	MCHEmul::Chips result;

	// The CIA1 & 2
	result.insert (MCHEmul::Chips::value_type (C64::CIA1::_ID, (MCHEmul::Chip*) new C64::CIA1));
	result.insert (MCHEmul::Chips::value_type (C64::CIA2::_ID, (MCHEmul::Chip*) new C64::CIA2));

	// The VicII created will depend on whether the visualization is PAL or NTSC...
	// Some how it is also controlled by CIA II and Special Control Chip
	result.insert (MCHEmul::Chips::value_type (COMMODORE::VICII::_ID, 
		(MCHEmul::Chip*) ((vS == C64::Commodore64::VisualSystem::_NTSC) 
			? (COMMODORE::VICII*) new C64::VICII_NTSC (nullptr, MCHEmul::Address (), 
				C64::Memory::_VICII_VIEW) // The color RAM will be assigned later...
			: (COMMODORE::VICII*) new C64::VICII_PAL (nullptr, MCHEmul::Address (), 
				C64::Memory::_VICII_VIEW))));

	// The SID...
	unsigned int cSpd = (vS == C64::Commodore64::VisualSystem::_NTSC) ? _NTSCCLOCK : _PALCLOCK;
	MCHEmul::SoundLibWrapper* sLib = nullptr;
	if (sS == "SID") sLib = new COMMODORE::SoundRESIDWrapper (cSpd, RESID::SAMPLE_FAST, COMMODORE::SID::_SOUNDSAMPLINGCLOCK);
	else sLib = (MCHEmul::SoundLibWrapper*) new COMMODORE::SoundSIDSimpleWrapper (cSpd, COMMODORE::SID::_SOUNDSAMPLINGCLOCK);
	result.insert (MCHEmul::Chips::value_type (COMMODORE::SID::_ID, 
		(MCHEmul::Chip*) new COMMODORE::SID (cSpd, sLib))); // The SID depends on the speed...

	// The PLA
	result.insert (MCHEmul::Chips::value_type (C64::PLA::_ID, (MCHEmul::Chip*) new C64::PLA));

	return (result);
}

// ---
MCHEmul::IODevices C64::Commodore64::standardDevices (C64::Commodore64::VisualSystem vS)
{
	MCHEmul::IODevices result;

	// The very basic systems...
	// They are really part of the system, with no simulated connections at all!
	result.insert (MCHEmul::IODevices::value_type (C64::Screen::_ID, 
		(MCHEmul::IODevice*) ((vS == C64::Commodore64::VisualSystem::_NTSC) 
			? (C64::Screen*) new C64::ScreenNTSC : (C64::Screen*) new C64::ScreenPAL)));
	result.insert (MCHEmul::IODevices::value_type (C64::SoundSystem::_ID, new C64::SoundSystem));
	result.insert (MCHEmul::IODevices::value_type (C64::InputOSSystem::_ID, new C64::InputOSSystem));

	// The different ports: 4!
	// The port where usually the datasette is connected...
	result.insert (MCHEmul::IODevices::value_type (COMMODORE::DatasetteIOPort::_ID, new C64::DatasetteIOPort));
	// The port where the floppy disk & printers are connected...
	result.insert (MCHEmul::IODevices::value_type (COMMODORE::SerialIOPort::_ID, new COMMODORE::SerialIOPort));
	// The port where the cartriges are connected...
	result.insert (MCHEmul::IODevices::value_type (COMMODORE::ExpansionIOPort::_ID, new C64::ExpansionIOPort));
	// The port where very specific devices are connected...
	result.insert (MCHEmul::IODevices::value_type (COMMODORE::UserIOPort::_ID, new COMMODORE::UserIOPort));

	return (result);
}
