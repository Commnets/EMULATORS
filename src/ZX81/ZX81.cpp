#include <ZX81/ZX81.hpp>
#include <ZX81/PortManager.hpp>
#include <ZX81/Screen.hpp>
#include <ZX81/OSIO.hpp>
#include <ZX81/EdgeConnector.hpp>
#include <ZX81/Cartridge.hpp>
#include <FZ80/CZ80.hpp>
#include <FZ80/NMIInterrupt.hpp>

// ---
ZX81::SinclairZX81::SinclairZX81 (ZX81::Memory::Configuration cfg, 
		ZX81::SinclairZX81::VisualSystem vS, unsigned char tc)
	: SINCLAIR::Computer 
		(new FZ80::CZ80 (0, 
			{ }), // Some other ports (the ones related with chips) are added later...
		 ZX81::SinclairZX81::standardChips (vS),
		 new ZX81::Memory (cfg, tc), // Depending on the configuration...
		 ZX81::SinclairZX81::standardDevices (vS),
		 _CLOCK, // In ZX81 the speed is constant as the CPU is aimed also to draw!
		 { }, { }, // The ZX81 emulation has been done without neither Buses nor Wires!
		 { { "Name", "ZX81" },
		   { "Manufacturer", "Sinclair Research/Timex Coporation" },
		   { "Year", "1981" }
		 }),
	  _A6 (false),
	  _visualSystem (vS)
{
	// Add the port manager for all ports!
	ZX81::PortManager* pM = new PortManager;
	FZ80::Z80PortsMap pMps;
	for (unsigned short i = 0; i < 256; i++)
		pMps.insert (FZ80::Z80PortsMap::value_type ((unsigned char) i, pM));
	static_cast <FZ80::CZ80*> (cpu ()) -> addPorts (pMps);

	// Assign the ULA to the PortManager...
	pM -> linkToULA (static_cast <ZX81::ULA*> (chip (ZX81::ULA::_ID)));

	setConfiguration (cfg, false /** Not restart at initialization. */);
}

// ---
bool ZX81::SinclairZX81::initialize (bool iM)
{
	bool result = SINCLAIR::Computer::initialize (iM);
	if (!result)
		return (false);

	setConfiguration (static_cast <ZX81::Memory*> 
		(memory ()) -> configuration (), false /** Not restart. */);

	/** This memory has to know where the CPU is on to return
		either a value of other. */
	ZX81::MemoryVideoCode::_programCounter = &cpu () -> programCounter ();

	// It is also needed to observe the edge connector...
	// Events when it is disonnected and connected are sent and with many implications
	// in the structure of the memory, and in the content of this...
	observe (dynamic_cast <ZX81::EdgeConnector*> (device (ZX81::EdgeConnector::_ID)));

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
	if (_A6.negativeEdge ()) // From 1 to 0...
		cpu () -> requestInterrupt
			(FZ80::INTInterrupt::_ID, cpu () -> clockCycles (), nullptr, 2);
}

// ---
void ZX81::SinclairZX81::setConfiguration (ZX81::Memory::Configuration cfg, bool rs) 
{
	static_cast <ZX81::Memory*> (memory ()) -> setConfiguration (cfg);

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

	// TODO

	return (result);
}
