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
	// The one used to communicate with the PSG (whatever chip finally is)...
	MSX::PSGPortManager* psgPM		= new MSX::PSGPortManager;
	// This port has to be linked to the sound chip...
	psgPM -> linkPSG (_model -> psg ()); // ...and that PSG can not be nullptr...
	// This port is to select the sctructure of the memory (the PPI8255)...
	MSX::PPIPortManager* ppiPM		= new MSX::PPIPortManager;
	// This port has to be linked to the ppi...
	assert (chip (MSX::PPI8255::_ID) != nullptr); // it has to exist...
	ppiPM -> linkPPI (dynamic_cast <MSX::PPI8255*> (chip (MSX::PPI8255::_ID)));

	FZ80::Z80PortsMap pMps;
	for (unsigned short i = 0; i < 256; i++)
		pMps.insert (FZ80::Z80PortsMap::value_type ((unsigned char) i, 
			(i >= 0x98 && i < 0xa0)					// 8 positions...
				? (FZ80::Z80Port*) vdpPM			// ...when the port is related with the VDP...
			: ((i >= 0xa0 && i < 0xa8)				// 8 positions...	
					? (FZ80::Z80Port*) psgPM		// ...when the port is related with the PSG...
					: ((i >= 0xa8 && i < 0xb0)		// 8 position...
						? (FZ80::Z80Port*) ppiPM	// ...when the port is related with the PPI...
						: (FZ80::Z80Port*) gPM))));	// In any other case...
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

	// Observe the PPI that manages, among other thigs, 
	// the changes in the configuration of the memory...
	observe (chip (MSX::PPI8255::_ID));

	return (true);
}

// ---
void MSX::MSXComputer::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	MCHEmul::Computer::processEvent (evnt, n);

	// Manage the changes in the configuration of the memory....
	if (evnt.id () == MSX::PPI8255::_SLOTCHANGED)
	{
		static_cast <MSX::Memory*> (memory ()) -> 
			activeteSlotsPerBank 
				((evnt.value () & 0x03),		// Slot active in bank 0
				 (evnt.value () & 0x0c) >> 2,	// Slot active in bank 1 
				 (evnt.value () & 0x30) >> 4,	// Slot active in bank 2
				 (evnt.value () & 0xc0) >> 6);	// Slot active in bank 3
	}
}

// ---
void MSX::MSXComputer::specificComputerCycle ()
{
	if (MSX::SubSlotRegisters::instance () -> changed ()) // put it back to false when tested...
		static_cast <MSX::Memory*> (memory ()) -> reactivateSlotsPerBank ();
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
