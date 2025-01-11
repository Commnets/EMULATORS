#include <ZX81/ZX81.hpp>
#include <ZX81/PortManager.hpp>
#include <ZX81/Screen.hpp>
#include <ZX81/OSIO.hpp>
#include <ZX81/EdgeConnector.hpp>
#include <ZX81/Cartridge.hpp>
#include <ZX81/DatasettePort.hpp>
#include <FZ80/CZ80.hpp>
#include <FZ80/NMIInterrupt.hpp>

// ---
ZX81::SinclairZX81::SinclairZX81 (ZX81::Memory::Configuration cfg, 
		ZX81::SinclairZX81::VisualSystem vS, ZX81::Type t)
	: SINCLAIR::Computer 
		(new FZ80::CZ80 (0, 
			{ }), // Some other ports (the ones related with chips) are added later...
		 ZX81::SinclairZX81::standardChips (vS),
		 new ZX81::Memory (cfg, t), // Depending on the configuration...
		 ZX81::SinclairZX81::standardDevices (vS),
		 loadSystemVariablesFrom ((t == ZX81::Type::_ZX80) ? "ZX80SysVars.txt" : "ZX81SysVars.txt"),
		 _CLOCK, // In ZX81 the speed is constant as the CPU is aimed also to draw!
		 { }, { }, // The ZX81 emulation has been done without neither Buses nor Wires!
		 { { "Name", "ZX81" },
		   { "Manufacturer", "Sinclair Research/Timex Coporation" },
		   { "Year", "1981" }
		 }),
	  _visualSystem (vS),
	  _type (t),
	  _ula (nullptr),
	  _A6 (false)
{
	// Add the port manager for all ports!
	ZX81::PortManager* pM = new ZX81::PortManager;
	FZ80::Z80PortsMap pMps;
	for (unsigned short i = 0; i < 256; i++)
		pMps.insert (FZ80::Z80PortsMap::value_type ((unsigned char) i, pM));
	static_cast <FZ80::CZ80*> (cpu ()) -> addPorts (pMps);

	_ula = dynamic_cast <ULA*> (chip (ULA::_ID));
	assert (_ula != nullptr);

	// Assign the ULA to the PortManager...
	pM -> linkToULA (_ula);

	setConfiguration (cfg, t, false /** Not restart at initialization. */);
}

// ---
MCHEmul::Strings ZX81::SinclairZX81::charsDrawSnapshot (MCHEmul::CPU* cpu,
	const std::vector <size_t>& chrs) const
{
	MCHEmul::Strings result;
	for (size_t i = 0; i < 64; i++)
	{
		if (!chrs.empty () && 
			std::find (chrs.begin (), chrs.end (), i) == chrs.end ())
			continue;

		MCHEmul::Address chrAdd = CHARS () + (i << 3);
		std::string dt = std::to_string (i) + "---\n$" +
			MCHEmul::removeAll0 (chrAdd.asString (MCHEmul::UByte::OutputFormat::_HEXA, '\0', 2)) + "\n";
		MCHEmul::UBytes chrDt = cpu -> memoryRef () -> values (chrAdd, 0x08);
		for (size_t j = 0; j < 8; j++) // 8 lines per character...
		{
			if (j != 0)
				dt += "\n";

			for (size_t l = 0; l < 8; l++)
				dt += ((chrDt [j].value () & (1 << (7 - l))) != 0x00) ? "#" : " ";
		}

		result.emplace_back (std::move (dt));
	}

	result.emplace_back ("---");

	return (result);
}

// ---
bool ZX81::SinclairZX81::initialize (bool iM)
{
	bool result = SINCLAIR::Computer::initialize (iM);
	if (!result)
		return (false);

	setConfiguration (static_cast <ZX81::Memory*> 
		(memory ()) -> configuration (), type (), false /** Not restart. */);

	/** This memory has to know where the CPU is on to return
		either a value of other. */
	ZX81::MemoryVideoCode::_programCounter = &cpu () -> programCounter ();

	// It is also needed to observe the edge connector...
	// Events when it is disonnected and connected are sent and with many implications
	// in the structure of the memory, and in the content of this...
	observe (dynamic_cast <ZX81::EdgeConnector*> (device (ZX81::EdgeConnector::_ID)));
	// Connect the casette port to the ULA
	// as the ULA (ports) generates also the signal that goes to the casette throught out the IN/OUT port...
	device (ZX81::DatasetteIOPort::_ID) -> observe (_ula);

	// Check whether there is an expansion element inserted in the edge connector port
	// If it is, it's info is loaded...
	ZX81::Cartridge* cT = 
		dynamic_cast <ZX81::Cartridge*> (dynamic_cast <ZX81::EdgeConnector*> 
			(device (ZX81::EdgeConnector::_ID)) -> expansionElement ());
	if (cT != nullptr)
		cT -> dumpDataInto (dynamic_cast <ZX81::Memory*> (memory ()), memory () -> activeView ());

	return (true);
}

// ---
void ZX81::SinclairZX81::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	// When a expansion element is inserted (or extracted) in (off) the edge connector, 
	// then everything has to restart...
	if (evnt.id () == ZX81::EdgeConnector::_EXPANSIONELEMENTIN ||
		evnt.id () == ZX81::EdgeConnector::_EXPANSIONELEMENTOUT)
	{
		setExit (true);
		setRestartAfterExit (true, 9999 /** Big enough */);
	}
}

// ---
void ZX81::SinclairZX81::specificComputerCycle ()
{
	_A6.set ((cpu () -> lastINOUTAddress ().value () & 0b01000000) != 0);

	// If the bit 6 of the address went from 1 to 0...
	// the INT line of the Z80 should be up (interrupt requested).
	// However at this point, the ULA cycle hasn't been executed yet,
	// and a NMI requests could be generated and obvioulsy with more priority than this one.
	// So the INT here is only requested if the ULA were not about to do so later!
	// In the definition of the ZX81 (machine) is possbile to have INT and NMI at the same time...
	// It is guaranteed by the code!...
	if (_A6.negativeEdge () &&	// From 1 to 0...
		cpu () -> interrupt (FZ80::INTInterrupt::_ID) -> 
			canBeExecutedOver (cpu (), cpu () -> clockCycles ()) &&
		!_ula -> aboutToGenerateNMIAfterCycles (cpu () -> lastCPUClockCycles ()))
	{ 
		// The INT is requested...
		cpu () -> requestInterrupt
			(FZ80::INTInterrupt::_ID, cpu () -> clockCycles (), nullptr, 2);

		// The ULA has to know that the INT is requested, 
		// because the counter of horizontal lines has to be updated when that happen...
		_ula -> setINTack ();
	}
}

// ---
void ZX81::SinclairZX81::setConfiguration 
	(ZX81::Memory::Configuration cfg, ZX81::Type t, bool rs) 
{
	static_cast <ZX81::Memory*> (memory ()) -> setConfiguration (cfg, t);

	// Restart?
	if (rs)
	{
		setExit (true);
		setRestartAfterExit (true, 9999 /** Big enough */);
	}
}

// ---
MCHEmul::Chips ZX81::SinclairZX81::standardChips (ZX81::SinclairZX81::VisualSystem vS)
{
	MCHEmul::Chips result;

	// The ULA
	result.insert (MCHEmul::Chips::value_type (ZX81::ULA::_ID, 
		(vS == ZX81::SinclairZX81::VisualSystem::_PAL) // Will depend on the type of screen...
			? (ZX81::ULA*) new ZX81::ULA_PAL (ZX81::Memory::_ULA_VIEW)
			: (ZX81::ULA*) new ZX81::ULA_NTSC (ZX81::Memory::_ULA_VIEW)));

	return (result);
}

// ---
MCHEmul::IODevices ZX81::SinclairZX81::standardDevices (ZX81::SinclairZX81::VisualSystem vS)
{
	MCHEmul::IODevices result;

	// The very basic systems...
	// They are really part of the system, with no simulated connections at all!
	result.insert (MCHEmul::IODevices::value_type (ZX81::Screen::_ID, 
		(MCHEmul::IODevice*) ((vS == ZX81::SinclairZX81::VisualSystem::_NTSC) 
			? (ZX81::Screen*) new ZX81::ScreenNTSC : (ZX81::Screen*) new ZX81::ScreenPAL)));
	result.insert (MCHEmul::IODevices::value_type (ZX81::InputOSSystem::_ID, new ZX81::InputOSSystem));

	// The Edge Connector
	result.insert (MCHEmul::IODevices::value_type (ZX81::EdgeConnector::_ID, new ZX81::EdgeConnector));
	// The IO port
	result.insert (MCHEmul::IODevices::value_type (ZX81::DatasetteIOPort::_ID, new ZX81::DatasetteIOPort));

	return (result);
}
