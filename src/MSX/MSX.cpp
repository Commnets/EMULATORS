#include <MSX/MSX.hpp>
#include <MSX/PortManager.hpp>
#include <FZ80/CZ80.hpp>

// ---
MSX::MSXComputer::MSXComputer (MSX::MSXModel* m, unsigned int cfg, 
		const std::string& lang)
	: MCHEmul::Computer 
		(new FZ80::CZ80 (0, 
			{ }), // Some other ports (the ones related with chips) are added later...
		 // "m" must be something, 
		 // however this is not checked until later, so the initilization is a bit complicated...
		 (m == nullptr) ? MCHEmul::Chips () : m -> chips (),
		// The memory depends on the configuration, the type, and the languaje!
		 (m == nullptr) ? nullptr : m -> memory (m -> configurationAdjusted (cfg), lang), 
		 (m == nullptr) ? MCHEmul::IODevices ()	: m -> ioDevices (),
		 (m == nullptr) ? 0 : m -> clockSpeed (),
		 { }, { }, // The MSX, emulation has been done without neither Buses nor Wires!
		 (m == nullptr) ? MCHEmul::Attributes () : m -> attributes ()),
	  _model (m)
{
	// The model can not be null...
	assert (_model != nullptr);

	// Create the type of ports...
	// The one used for any port not linked...
	MSX::GeneralPortManager* gPM	= new MSX::GeneralPortManager;
	// The one used to communicate with the VDP (whatever chip finally is)...
	MSX::VDPPortManager* vdpPM		= new MSX::VDPPortManager;
	// This port has to be linked to the graphical chip...
	vdpPM -> linkVDP (_model -> vdp ()); // ...and that VDP can not be nullptr...

	FZ80::Z80PortsMap pMps;
	for (unsigned short i = 0; i < 256; i++)
		pMps.insert (FZ80::Z80PortsMap::value_type ((unsigned char) i, 
			(i >= 0x98 && i < 0xa0) 
				? (FZ80::Z80Port*) vdpPM	// When the port is related with the VDP...
				: (FZ80::Z80Port*) gPM));	// In any other case...
	static_cast <FZ80::CZ80*> (cpu ()) -> addPorts (pMps);

	// Adapt the configuration again, and it doesn't initialize back...
	setConfiguration (_model -> configurationAdjusted (cfg), false /** Not restart at initialization. */);
}

// ---
bool MSX::MSXComputer::initialize (bool iM)	
{
	bool result = MCHEmul::Computer::initialize (iM);
	if (!result)
		return (false);

	setConfiguration (static_cast <MSX::Memory*> 
		(memory ()) -> configuration (), false /** Not restart. */);

	// TODO: Observe directly devices, peripherals, chips...
	// TODO: Manage the conexion of the cartridge

	return (true);
}

// ---
void MSX::MSXComputer::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	// TODO: Normally when a cartrodge is connected then the 
	// memory configuration changes and everything has to start up back
}

// ---
void MSX::MSXComputer::setConfiguration (unsigned int cfg, bool rs) 
{
	static_cast <MSX::Memory*> (memory ()) -> setConfiguration (cfg);

	// Restart?
	if (rs)
	{
		setExit (true);
		setRestartAfterExit (true, 9999 /** Big enough */);
	}
}
