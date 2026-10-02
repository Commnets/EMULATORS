#include <ZX81/ZX81.hpp>
#include <ZX81/PortManager.hpp>
#include <ZX81/Screen.hpp>
#include <ZX81/OSIO.hpp>
#include <ZX81/EdgeConnector.hpp>
#include <ZX81/Cartridge.hpp>
#include <ZX81/DatasettePort.hpp>
#include <ZX81/CZ80.hpp>
#include <FZ80/INTInterrupt.hpp>

// ---
ZX81::SinclairZX81::SinclairZX81 (
		const MCHEmul::ASCIIToCodeConverter* cvs,
		ZX81::Memory::Configuration cfg, 
		ZX81::SinclairZX81::VisualSystem vS, ZX81::Type t)
	: SINCLAIR::Computer 
		(new ZX81::CZ80 (0,
			{ }), // Some other ports (the ones related with chips) are added later...
		 ZX81::SinclairZX81::standardChips (vS, t), // The chips can vary depending on the type of model...
		 new ZX81::Memory (cfg, t), // Depending on the configuration...
		 ZX81::SinclairZX81::standardDevices (vS, t), // The devices can vary depending on the type of model...
		 loadSystemVariablesFrom ((t == ZX81::Type::_ZX80) ? "ZX80SysVars.txt" : "ZX81SysVars.txt"),
		 _CLOCK, // In ZX81 the speed is constant as the CPU is aimed also to draw!
		 cvs,
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
	ZX81::PortManager* pM = new ZX81::PortManager (_type);
	FZ80::Z80PortsMap pMps;
	for (unsigned short i = 0; i < 256; i++)
		pMps.insert (FZ80::Z80PortsMap::value_type ((unsigned char) i, pM));
	static_cast <FZ80::CZ80*> (cpu ()) -> addPorts (pMps);

	_ula = dynamic_cast <ULA*> (chip (ULA::_ID));
	assert (_ula != nullptr);

	// Accepted INT responses reach the ULA directly, without CPU observers.
	static_cast <ZX81::CZ80*> (cpu ()) -> linkToULA (_ula);

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

	_A6 = MCHEmul::Pulse (false);

	setConfiguration (static_cast <ZX81::Memory*> 
		(memory ()) -> configuration (), type (), false /** Not restart. */);

	/** This memory has to know where the CPU is on to return
		either a value of other, but it is not the owner of it! */
	ZX81::MemoryVideoCode::_cpu = cpu ();
	/** And also where the ULA is. */
	ZX81::MemoryVideoCode::_ula = _ula;

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
	// Accepted timing limitation: in PerCycle mode, executeNextCycle() may already
	// have consumed a batch of T-states before this computer-cycle hook is reached.
	// If that batch crosses several instruction boundaries, the CPU may have started
	// later instructions before A6 is sampled here or the ULA generates its NMI requests.
	// These indicators retain only the latest relevant completion, not every boundary.
	// Earlier INT sampling opportunities cannot be reconstructed, acceptance can be
	// delayed, and multiple INT acknowledgements can overwrite the ULA's single latch
	// before it is simulated. Even a batch ending inside the next instruction can
	// delay the sample of the preceding one. These effects are explicitly accepted
	// with the current coarse CPU/chip scheduling; this hook does not replay the batch.
	// Full mode returns after one complete transaction and therefore avoids this
	// particular loss of intermediate boundaries. Cycle-exact coordination would
	// require interleaving the CPU and chips before each interrupt decision.
	ZX81::CZ80* z80 = static_cast <ZX81::CZ80*> (cpu ());

	// Read both indicators explicitly: each access consumes its value.
	bool instructionCompleted = z80 -> takeInstructionCompleted ();
	bool interruptCompleted = z80 -> takeInterruptCompleted ();

	// Intermediate cycles must not remove a request still being processed.
	if (!instructionCompleted && !interruptCompleted)
		return;

	// NMI may have left an older A6 request pending behind it.
	// A completed instruction also replaces the previous A6 sample.
	removeA6InterruptRequest ();

	// An interrupt response does not provide a new end-of-instruction sample.
	if (!instructionCompleted)
		return;

	_A6.set ((z80 -> lastINOUTAddress ().value () & 0b01000000) != 0);

	unsigned int cC = z80 -> clockCycles ();

	// INT is active low: continued assertion does not require another falling edge.
	// The CPU checks eligibility again before accepting the queued request.
	if (!_A6.value () &&
		z80 -> interrupt (FZ80::INTInterrupt::_ID) ->
			canBeExecutedOver (z80, cC) == MCHEmul::CPUInterrupt::_EXECUTIONALLOWED)
		z80 -> requestInterrupt
			(FZ80::INTInterrupt::_ID, cC, nullptr, 2);
}

// ---
void ZX81::SinclairZX81::removeA6InterruptRequest ()
{
	const MCHEmul::CPUInterruptRequests& requests = cpu () -> interruptsRequested ();

	// A6 uses the existing request signature: INT, no source and reason 2.
	// Reverse traversal preserves the remaining indices when entries are removed.
	for (size_t i = requests.size (); i > 0; --i)
	{
		const MCHEmul::CPUInterruptRequest& request = requests [i - 1];

		if (request.type () == FZ80::INTInterrupt::_ID &&
			request.from () == nullptr &&
			request.reason () == 2)
			cpu () -> removeInterruptRequest (request);
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
MCHEmul::Chips ZX81::SinclairZX81::standardChips 
	(ZX81::SinclairZX81::VisualSystem vS, ZX81::Type t)
{
	MCHEmul::Chips result;

	// The ULA
	// In the ZX80 like models there is no really a ULA but just digital circuits...
	// However the behaviour of those is simulated using a "ULA"....
	result.insert (MCHEmul::Chips::value_type (ZX81::ULA::_ID, 
		(vS == ZX81::SinclairZX81::VisualSystem::_PAL) // Will depend on the type of screen...
			? (ZX81::ULA*) new ZX81::ULA_PAL (t, ZX81::Memory::_ULA_VIEW)
			: (ZX81::ULA*) new ZX81::ULA_NTSC (t, ZX81::Memory::_ULA_VIEW)));

	return (result);
}

// ---
MCHEmul::IODevices ZX81::SinclairZX81::standardDevices 
	(ZX81::SinclairZX81::VisualSystem vS, ZX81::Type t)
{
	MCHEmul::IODevices result;

	// The very basic systems...
	// They are really part of the system, with no simulated connections at all!
	result.insert (MCHEmul::IODevices::value_type (ZX81::Screen::_ID, 
		(MCHEmul::IODevice*) ((vS == ZX81::SinclairZX81::VisualSystem::_NTSC) 
			? (ZX81::Screen*) new ZX81::ScreenNTSC : (ZX81::Screen*) new ZX81::ScreenPAL)));
	result.insert (MCHEmul::IODevices::value_type (ZX81::InputOSSystem::_ID, new ZX81::InputOSSystem));

	// The Edge Connector
	// The edge connector has different behaviour depending on whwther the compyter is ZX81 or ZX80...
	result.insert (MCHEmul::IODevices::value_type (ZX81::EdgeConnector::_ID, 
		new ZX81::EdgeConnector (t))); 
	// The IO port
	result.insert (MCHEmul::IODevices::value_type (ZX81::DatasetteIOPort::_ID, new ZX81::DatasetteIOPort));

	return (result);
}
