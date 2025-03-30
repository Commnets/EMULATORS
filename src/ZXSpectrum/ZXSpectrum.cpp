#include <ZXSpectrum/ZXSpectrum.hpp>
#include <ZXSpectrum/PortManager.hpp>
#include <ZXSpectrum/ULA.hpp>
#include <ZXSpectrum/Screen.hpp>
#include <ZXSpectrum/Sound.hpp>
#include <ZXSpectrum/OSIO.hpp>
#include <ZXSpectrum/EdgeConnector.hpp>
#include <ZXSpectrum/Cartridge.hpp>
#include <ZXSpectrum/DatasettePort.hpp>
#include <FZ80/CZ80.hpp>
#include <FZ80/NMIInterrupt.hpp>

// ---
ZXSPECTRUM::SinclairZXSpectrum::SinclairZXSpectrum (ZXSPECTRUM::Memory::Configuration cfg, 
		ZXSPECTRUM::SinclairZXSpectrum::VisualSystem vS, 
		ZXSPECTRUM::Type t, const std::string& lang)
	: SINCLAIR::Computer 
		(new FZ80::CZ80 (0, 
			{ }), // Some other ports (the ones related with chips) are added later...
		 ZXSPECTRUM::SinclairZXSpectrum::standardChips (vS),
		 new ZXSPECTRUM::Memory (cfg, t, lang), // Depending on the configuration, the type, and the languaje!
		 ZXSPECTRUM::SinclairZXSpectrum::standardDevices (vS),
		 loadSystemVariablesFrom ("ZXSpectrumSysVars.txt"),
		 _CLOCK,
		 { }, { }, // The ZXSpectrum, emulation has been done without neither Buses nor Wires!
		 { { "Name", "ZXSpectrum" },
		   { "Manufacturer", "Sinclair Research/Timex Coporation" },
		   { "Year", "1982" }
		 }),
	  _visualSystem (vS),
	  _type (t),
	  _ula (nullptr)
{
	// Add the port manager for all ports!
	ZXSPECTRUM::PortManager* pM = new ZXSPECTRUM::PortManager;
	FZ80::Z80PortsMap pMps;
	for (unsigned short i = 0; i < 256; i++)
		pMps.insert (FZ80::Z80PortsMap::value_type ((unsigned char) i, pM));
	static_cast <FZ80::CZ80*> (cpu ()) -> addPorts (pMps);

	_ula = dynamic_cast <ULA*> (chip (ULA::_ID));
	assert (_ula != nullptr);

	// Assign the ULA to the PortManager...
	pM -> linkToULA (_ula);

	setConfiguration (cfg, false /** Not restart at initialization. */);
}

// ---
MCHEmul::Strings ZXSPECTRUM::SinclairZXSpectrum::charsDrawSnapshot (MCHEmul::CPU* cpu,
	const std::vector <size_t>& chrs) const
{
	MCHEmul::Strings result;
	for (size_t i = 0; i < 96; i++)
	{
		if (!chrs.empty () && 
			std::find (chrs.begin (), chrs.end (), i) == chrs.end ())
			continue;

		MCHEmul::Address chrAdd = 
			MCHEmul::Address ({ 0x00, 0x3d }, false) + (i << 3);
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
bool ZXSPECTRUM::SinclairZXSpectrum::initialize (bool iM)	
{
	bool result = SINCLAIR::Computer::initialize (iM);
	if (!result)
		return (false);

	setConfiguration (static_cast <ZXSPECTRUM::Memory*> 
		(memory ()) -> configuration (), false /** Not restart. */);

	// It is also needed to observe the edge connector...
	// Events when it is disonnected and connected are sent and with many implications
	// in the structure of the memory, and in the content of this...
	observe (dynamic_cast <ZXSPECTRUM::EdgeConnector*> (device (ZXSPECTRUM::EdgeConnector::_ID)));
	// Connect the casette port to the ULA...
	// ...as the ULA (ports) generates also the signal that goes to the casette throught out the IN/OUT port...
	device (ZXSPECTRUM::DatasetteIOPort::_ID) -> observe (_ula);

	// Check whether there is an expansion element inserted in the edge connector port
	// If it is, it's info is loaded...
	ZXSPECTRUM::Cartridge* cT = 
		dynamic_cast <ZXSPECTRUM::Cartridge*> (dynamic_cast <ZXSPECTRUM::EdgeConnector*> 
			(device (ZXSPECTRUM::EdgeConnector::_ID)) -> expansionElement ());
	if (cT != nullptr)
		cT -> dumpDataInto (dynamic_cast <ZXSPECTRUM::Memory*> (memory ()), memory () -> activeView ());

	return (true);
}

// ---
void ZXSPECTRUM::SinclairZXSpectrum::specificComputerCycle ()
{ 
	// When the screen memory is accesed...
	_ula -> setScreenMemoryAccessedFromCPU (
		cpu () -> stopped () 
			? false // If it is stopped, it is not accesed...
			: ((cpu () -> lastINOUTAddress ().value () >= 0x4000 &&
			    cpu () -> lastINOUTAddress ().value () < 0x8000) ||
			   (cpu () -> programCounter ().internalRepresentation () >= 0x4000 &&
				cpu () -> programCounter ().internalRepresentation () < 0x8000)));
}

// ---
void ZXSPECTRUM::SinclairZXSpectrum::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	// When a expansion element is inserted (or extracted) in (off) the edge connector, 
	// then everything has to restart...
	if (evnt.id () == ZXSPECTRUM::EdgeConnector::_EXPANSIONELEMENTIN ||
		evnt.id () == ZXSPECTRUM::EdgeConnector::_EXPANSIONELEMENTOUT)
	{
		setExit (true);
		setRestartAfterExit (true, 9999 /** Big enough */);
	}
}

// ---
void ZXSPECTRUM::SinclairZXSpectrum::setConfiguration (ZXSPECTRUM::Memory::Configuration cfg, bool rs) 
{
	static_cast <ZXSPECTRUM::Memory*> (memory ()) -> setConfiguration (cfg);

	// Restart?
	if (rs)
	{
		setExit (true);
		setRestartAfterExit (true, 9999 /** Big enough */);
	}
}

// ---
MCHEmul::Chips ZXSPECTRUM::SinclairZXSpectrum::standardChips (ZXSPECTRUM::SinclairZXSpectrum::VisualSystem vS)
{
	MCHEmul::Chips result;

	// The ULA
	ZXSPECTRUM::ULA* ula = nullptr;
	result.insert (MCHEmul::Chips::value_type (ZXSPECTRUM::ULA::_ID, 
		ula = ((vS == ZXSPECTRUM::SinclairZXSpectrum::VisualSystem::_PAL) // Will depend on the type of screen...
			? (ZXSPECTRUM::ULA*) new ZXSPECTRUM::ULA_PAL (ZXSPECTRUM::Memory::_ULA_VIEW, _CLOCK)
			: (ZXSPECTRUM::ULA*) new ZXSPECTRUM::ULA_NTSC (ZXSPECTRUM::Memory::_ULA_VIEW, _CLOCK))));

	// ...and the simulation of the sound function within the ULA!
	result.insert (MCHEmul::Chips::value_type (ZXSPECTRUM::ULA::SoundFunction::_ID, ula -> soundFunction ()));

	return (result);
}

// ---
MCHEmul::IODevices ZXSPECTRUM::SinclairZXSpectrum::standardDevices (ZXSPECTRUM::SinclairZXSpectrum::VisualSystem vS)
{
	MCHEmul::IODevices result;

	// The very basic systems...
	// They are really part of the system, with no simulated connections at all!
	result.insert (MCHEmul::IODevices::value_type (ZXSPECTRUM::Screen::_ID, 
		(MCHEmul::IODevice*) ((vS == ZXSPECTRUM::SinclairZXSpectrum::VisualSystem::_NTSC) 
			? (ZXSPECTRUM::Screen*) new ZXSPECTRUM::ScreenNTSC : (ZXSPECTRUM::Screen*) new ZXSPECTRUM::ScreenPAL)));
	result.insert (MCHEmul::IODevices::value_type (ZXSPECTRUM::SoundSystem::_ID, new ZXSPECTRUM::SoundSystem));
	result.insert (MCHEmul::IODevices::value_type (ZXSPECTRUM::InputOSSystem::_ID, new ZXSPECTRUM::InputOSSystem));

	// The Edge Connector
	result.insert (MCHEmul::IODevices::value_type 
		(ZXSPECTRUM::EdgeConnector::_ID, new ZXSPECTRUM::EdgeConnector));
	// The IO port
	result.insert (MCHEmul::IODevices::value_type 
		(ZXSPECTRUM::DatasetteIOPort::_ID, new ZXSPECTRUM::DatasetteIOPort));

	return (result);
}
