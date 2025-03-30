#include <MSX/MSX.hpp>
#include <MSX/PortManager.hpp>
#include <FZ80/CZ80.hpp>

// ---
MSX::MSXComputer::MSXComputer (MSX::MSXModel* m, unsigned int cfg, 
		const std::string& lang)
	: MCHEmul::Computer 
		(new FZ80::CZ80 (0, 
			{ }), // Some other ports (the ones related with chips) are added later...
		 m -> chips (),
		 m -> memory (cfg, lang), // Depending on the configuration, the type, and the languaje!
		 m -> ioDevices (),
		 m -> clockSpeed (),
		 { }, { }, // The MSX, emulation has been done without neither Buses nor Wires!
		 m -> attributes ()),
	  _model (m)
{
	// Add the port manager for all ports!
	MSX::PortManager* pM = new MSX::PortManager;
	FZ80::Z80PortsMap pMps;
	for (unsigned short i = 0; i < 256; i++)
		pMps.insert (FZ80::Z80PortsMap::value_type ((unsigned char) i, pM));
	static_cast <FZ80::CZ80*> (cpu ()) -> addPorts (pMps);

	// Adapt the configuration again, and it doesn't initialize back...
	setConfiguration (cfg, false /** Not restart at initialization. */);
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
