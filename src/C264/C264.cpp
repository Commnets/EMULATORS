#include <C264/C264.hpp>
#include <C264/Screen.hpp>
#include <C264/Sound.hpp>
#include <C264/OSIO.hpp>
#include <C264/DatasettePort.hpp>
#include <C264/ExpansionPort.hpp>
#include <C264/IO7510PortRegisters.hpp>
#include <C264/C6529B1.hpp>
#include <C264/C6529B2.hpp>
#include <C264/TED.hpp>
#include <C264/ROMRAMSwitch.hpp>
#include <C264/C1C2Selector.hpp>
#include <C264/Cartridge.hpp>
#include <C264/SerialIONotPresent.hpp>
#include <F6500/C7501.hpp>

// ---
C264::Commodore264::Commodore264 (
		const MCHEmul::ASCIIToCodeConverter* cnv, 
		const MCHEmul::Chips& cps, MCHEmul::Memory* m, const MCHEmul::IODevices& dvs, 
		C264::Commodore264::VisualSystem vS, C264::Type t, unsigned int cfg, const std::string& lng)
	: COMMODORE::Computer 
		(new F6500::C7501 (0 /** Only one micro. */),
		 cps,
		 m,
		 dvs,
		 (vS == C264::Commodore264::VisualSystem::_PAL)
			? (_PALCLOCK << 1) : (_NTSCCLOCK << 1),
		 cnv,
		 { }, { }, // The C264 series emulation has been done without neither Buses nor Wires!
		 { { "Name", "C264 Series (C116/C16/CPlus4)" },
		   { "Manufacturer", "Commodore Business Machines CBM" },
		   { "Year", "1984" }
		 }),
	  _visualSystem (vS),
	  _configuration (cfg),
	  _ted (nullptr)
{
	assert (_memory != nullptr);

	setConfiguration (_configuration, false /** No restart. */);

	// Look for the TED...
	_ted = dynamic_cast <C264::TED*> (ted ());
	// ...and cannot be nullptr!
	assert (_ted != nullptr);
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

	// Observe CPU transaction notifications so TED can prepare readable prefixes
	// and recover instruction and interrupt writes at their scheduled cycles.
	observe (_cpu);

	// It is also needed to observe the expansion port...
	// Events when it is disonnected and connected are sent and with many implications
	// in the structure of the memory...
	observe (dynamic_cast <COMMODORE::ExpansionIOPort*> (device (COMMODORE::ExpansionIOPort::_ID)));
	// TED observes keyboard-matrix pin changes from the 6529B.
	chip (COMMODORE::TED::_ID) -> observe (chip (C264::C6529B1::_ID));

	// Links between memory and devices...
	// The device observe the interactions between the IO7501 port registers and itself...
	device (COMMODORE::DatasetteIOPort::_ID) -> observe 
		(memory () -> subset (C264::IO7501PortRegisters::_IO7501REGISTERS_SUBSET));
	// ...but also the port registers observe the datasette device...
	memory () -> subset (C264::IO7501PortRegisters::_IO7501REGISTERS_SUBSET) -> observe 
		(device (COMMODORE::DatasetteIOPort::_ID));

	// Check whether there is an expansion element inserted in the expansion port
	// If it is, it's info is loaded if any...
	C264::Cartridge* cT = 
		dynamic_cast <C264::Cartridge*> (dynamic_cast <COMMODORE::ExpansionIOPort*> 
			(device (COMMODORE::ExpansionIOPort::_ID)) -> expansionElement ());
	if (cT != nullptr)
		cT -> dumpDataInto (dynamic_cast <C264::Memory*> (memory ()), memory () -> activeView ());

	// Setup writes consume no emulated transaction time.
	_cpu -> executeBufferedCommands (true);

	// Apply setup selector latches before the first CPU transaction.
	if (!chip (C264::ROMRAMSwitch::_ID) -> simulate (_cpu) ||
		!chip (C264::C1C2Selector::_ID) -> simulate (_cpu))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	if (!_ted -> prepareCPUTimedWrites
			(_cpu, static_cast <C264::Memory*> (memory ()) -> cpuWriteSubsets ()) ||
		!_ted -> enableCPUTimedWrites ())
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	return (true);
}

// ---
void C264::Commodore264::processEvent
	(const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	if (evnt.id () == COMMODORE::ExpansionIOPort::_EXPANSIONELEMENTIN ||
		evnt.id () == COMMODORE::ExpansionIOPort::_EXPANSIONELEMENTOUT)
	{
		setExit (true);
		setRestartAfterExit (true, 9999 /** Big enough */);
	}
	else if (n == _cpu &&
		evnt.id () == MCHEmul::CPU::_CPUTOEXECUTEINSTRUCTION)
	{
		static_cast <COMMODORE::TED*> (graphicalChip ()) ->
			CPUAboutToExecute
				(static_cast <const MCHEmul::InstructionContextEventData*>
					(evnt.data ().get ()));
	}
	else if (n == _cpu &&
		evnt.id () == MCHEmul::CPU::_CPUTOEXECUTEINTERRUPT)
	{
		static_cast <COMMODORE::TED*> (graphicalChip ()) ->
			CPUAboutToExecute
				(static_cast <const MCHEmul::InterruptContextEventData*>
					(evnt.data ().get ()));
	}
}

// ---
unsigned int C264::Commodore264::clockCyclesForPacing
	(unsigned int cpuCycles)
{
	const unsigned int pacingCycles = _ted -> takePendingPacingCycles ();

	if (deepDebug ())
		debugClockPacing (cpuCycles, pacingCycles);

	return (pacingCycles);
}

// ---
bool C264::Commodore264::executeComputerHooks ()
{
	// External writes may have changed selector latches while stopped.
	chip (C264::ROMRAMSwitch::_ID) -> simulate (_cpu);
	chip (C264::C1C2Selector::_ID) -> simulate (_cpu);

	const bool result = COMMODORE::Computer::executeComputerHooks ();

	// Hooks may also change selectors before the next CPU transaction.
	chip (C264::ROMRAMSwitch::_ID) -> simulate (_cpu);
	chip (C264::C1C2Selector::_ID) -> simulate (_cpu);

	return (result);
}

// ---
void C264::Commodore264::setConfiguration (unsigned int cfg, bool rs) 
{
	// Just change the configuration...
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
				(F6500::IRQInterrupt::_ID, (C264::TED_PAL::_CLOCK / C264::Commodore264::_PALCLOCK), /** = 20. */
					C264::Memory::_TED_VIEW, new COMMODORE::TEDSoundSimpleLibWrapper 
						(C264::TED_PAL::_CLOCK, COMMODORE::TED_PAL::_DIVIDERCPUCLOCK, COMMODORE::TED::_SOUNDSAMPLINGCLOCK))
			: (C264::TED*) new C264::TED_NTSC
				(F6500::IRQInterrupt::_ID, (C264::TED_NTSC::_CLOCK / C264::Commodore264::_NTSCCLOCK), /** = 16. */
					C264::Memory::_TED_VIEW, new COMMODORE::TEDSoundSimpleLibWrapper 
						(C264::TED_NTSC::_CLOCK, COMMODORE::TED_NTSC::_DIVIDERCPUCLOCK, COMMODORE::TED::_SOUNDSAMPLINGCLOCK));
	result.insert (MCHEmul::Chips::value_type (COMMODORE::TED::_ID, (MCHEmul::Chip*) ted));

	// The sound chip doesn't exist in C264 series. 
	// It is part of the TED.
	// However, the emulation needs to treat it as an independent chip...
	result.insert (MCHEmul::Chips::value_type (COMMODORE::TED::SoundFunction::_ID, ted -> soundFunction ()));

	// Other chips...
	// This intermediate chip is just used to latch the value of the keyboard matrix...
	result.insert (MCHEmul::Chips::value_type (C264::C6529B1::_ID, new C264::C6529B1));
	// This chip is like virtual...
	// It is to manage the changes in the ROM/RAM configuration
	result.insert (MCHEmul::Chips::value_type (C264::ROMRAMSwitch::_ID, new C264::ROMRAMSwitch));
	// This chip is like virtual...
	// It is to manage the changes in the ROM/RAM configuration
	result.insert (MCHEmul::Chips::value_type (C264::C1C2Selector::_ID, new C264::C1C2Selector));

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
	result.insert (MCHEmul::IODevices::value_type (COMMODORE::SerialIOPort::_ID, 
		new COMMODORE::SerialIOPort (new C264::SerialNotPresentIOPeripheralSimulation)));
	// The port where the cartriges are connected...
	result.insert (MCHEmul::IODevices::value_type (COMMODORE::ExpansionIOPort::_ID, new C264::ExpansionIOPort));

	return (result);
}

//-----
// ---
void C264::Commodore264::debugClockPacing
	(unsigned int cpuCycles, unsigned int pacingCycles) const
{
	_deepDebug.writeCompleteLine
		(className (), _cpu -> clockCycles (), "Clock pacing",
		{ { "Timing", "Reference=ObservedCPUClock" },
		  { "Pacing",
			"Unit=FastCPUCycle,CPUCycles=" + std::to_string (cpuCycles) +
			",ChargedCycles=" + std::to_string (pacingCycles) },
		  { "Clock",
			"BaseHz=" + std::to_string (_clock.cyclesPerSecond ()) +
			",Factor=" + std::to_string (_clock.factor ()) } });
}
//-----

// ---
bool C264::Commodore16_116::initialize (bool iM)
{
	bool result = C264::Commodore264::initialize (iM);
	if (!result)
		return (false);

	// In the case of this computer, this specific memory subset
	// has to observe the datasette port too...
	// to control whether the keys are pressed or not...
	memory () -> subset (C264::C16_116Memory::_IONOMAPPED1_SUBSET) -> observe 
		(device (COMMODORE::DatasetteIOPort::_ID));

	return (true);
}

// ---
MCHEmul::Chips C264::Commodore16_116::standardChips (const std::string& sS, C264::Commodore264::VisualSystem vS)
{
	MCHEmul::Chips result = 
		std::move (C264::Commodore264::standardChips (sS, vS));

	// There is no additional chips inside...

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
bool C264::CommodorePlus4::initialize (bool iM)
{
	bool result = C264::Commodore264::initialize (iM);
	if (!result)
		return (false);

	// In the case of this computer, this specific chip
	// has to observe the datasette port too (and also other things in the case of CPlus4)
	// to control whether the keys are pressed or not...
	// In the case of the C16, the datasette is direcly observed from the memory set...
	chip (C264::C6529B2::_ID) -> observe (device (COMMODORE::DatasetteIOPort::_ID));

	return (true);
}

// ---
MCHEmul::Chips C264::CommodorePlus4::standardChips (const std::string& sS, C264::Commodore264::VisualSystem vS)
{
	MCHEmul::Chips result = 
		std::move (C264::Commodore264::standardChips (sS, vS));

	// Other important chips in this version...
	// The ACIA...
	result.insert (MCHEmul::Chips::value_type (COMMODORE::ACIA::_ID, new COMMODORE::ACIA));
	// ...and the other C6529 apparently used in the IO operations...
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
