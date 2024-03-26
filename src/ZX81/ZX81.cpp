#include <ZX81/ZX81.hpp>
#include <ZX81/ULA.hpp>
#include <ZX81/Screen.hpp>
#include <ZX81/OSIO.hpp>
#include <ZX81/EdgeConnector.hpp>
#include <ZX81/Cartridge.hpp>
#include <FZ80/CZ80.hpp>
#include <FZ80/NMIInterrupt.hpp>

const std::string ZX81::PortFE::_NAME = "FE";

// ---
MCHEmul::UByte ZX81::PortFE::value () const
{
	MCHEmul::UByte result = 0b0010000 |		// But 5 is always set...
		(_NTSC ? 0b00000000 : 0b01000000);	// Bit 6 set when 50Hz = PAL = !NTSC...

	// What row to read is determined by the value of the register B!
	MCHEmul::UByte bVal = 
		static_cast <const FZ80::CZ80*> (cpu ()) -> bRegister ().values ()[0];
	for (size_t i = 0; i < 7; i++) // Can be pressed simultaneously...
		if (!bVal.bit (i)) result |= _keyboardStatus [i];

	return (result);
}

// ---
void ZX81::PortFE::setValue (const MCHEmul::UByte& v)
{
	static_cast <FZ80::CZ80*> 
		(cpu ()) -> interrupt (FZ80::NMIInterrupt::_ID) -> setActive (true);
}

// ---
void ZX81::PortFE::initialize ()
{
	FZ80::Z80Port::initialize ();

	initializeInternalValues ();
}

// ---
void ZX81::PortFE::initializeInternalValues ()
{
	for (size_t i = 0; i < 8;
		_keyboardStatus [i++] = MCHEmul::UByte::_0);
}

// ---
ZX81::SinclairZX81::SinclairZX81 (ZX81::Memory::Configuration cfg, 
		ZX81::SinclairZX81::VisualSystem vS, unsigned char tc)
	: SINCLAIR::Computer 
		(new FZ80::CZ80 (0,
			{ { ZX81::PortFE::_ID, new ZX81::PortFE (vS == ZX81::SinclairZX81::VisualSystem::_NTSC) } }),
		 ZX81::SinclairZX81::standardChips (vS),
		 new ZX81::Memory (cfg, tc), // Depending on the configuration...
		 ZX81::SinclairZX81::standardDevices (vS),
		 vS == ZX81::SinclairZX81::VisualSystem::_PAL ? _PALCLOCK : _NTSCCLOCK,
		 { }, { }, // The ZX81 emulation has been done without neither Buses nor Wires!
		 { { "Name", "ZX81" },
		   { "Manufacturer", "Sinclair Reserach/Timex Coporation" },
		   { "Year", "1981" }
		 }),
	  _visualSystem (vS)
{
	// The ULA has to be "linked" to the PortFE...always...
	static_cast <ZX81::ULA*> (chip (ZX81::ULA::_ID)) -> 
		linkToPortFE (static_cast <ZX81::PortFE*> (static_cast <FZ80::CZ80*> (cpu ()) -> port (ZX81::PortFE::_ID)));

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
			? (ZX81::ULA*) new ZX81::ULA_PAL ()
			: (ZX81::ULA*) new ZX81::ULA_NTSC ()));

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
