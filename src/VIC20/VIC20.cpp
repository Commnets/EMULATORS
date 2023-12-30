#include <VIC20/VIC20.hpp>
#include <VIC20/VIA1.hpp>
#include <VIC20/VIA2.hpp>
#include <COMMODORE/VICI.hpp>
#include <F6500/C6502.hpp>

// ---
VIC20::CommodoreVIC20::CommodoreVIC20 (VIC20::Memory::Configuration cfg, 
		VIC20::CommodoreVIC20::VisualSystem vS, const std::string& lg)
	: COMMODORE::Computer 
		(new F6500::C6502 (0 /** Only one micro. */),
		 VIC20::CommodoreVIC20::standardChips (vS, lg),
		 new VIC20::Memory (cfg, lg),
		 VIC20::CommodoreVIC20::standardDevices (),
		 vS == VIC20::CommodoreVIC20::VisualSystem::_PAL ? _PALCLOCK : _NTSCCLOCK,
		 { }, { }, // The VIC20 emulation has been done without neither Buses nor Wires!
		 { { "Name", "VIC20" },
		   { "Manufacturer", "Commodore Business Machines CBM" },
		   { "Year", "1980" }
		 }),
	  _visualSystem (vS)
{
	// Nothing else to do...
}

// ---
bool VIC20::CommodoreVIC20::initialize (bool iM)
{
	bool result = MCHEmul::Computer::initialize (iM);
	if (!result)
		return (false);

	// Depending on the memory configuration, 
	// the registers that point out to the char and color memory must be adapted!
	setConfiguration (static_cast <VIC20::Memory*> (memory ()) -> configuration ());

	return (true);
}

// ---
void VIC20::CommodoreVIC20::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	// TODO
}

// ---
void VIC20::CommodoreVIC20::setConfiguration (VIC20::Memory::Configuration cfg)
{
	// TODO
}

// ---
MCHEmul::Chips VIC20::CommodoreVIC20::standardChips 
	(VIC20::CommodoreVIC20::VisualSystem vS, const std::string& sS)
{
	MCHEmul::Chips result;

	// The VIA1 & 2
	result.insert (MCHEmul::Chips::value_type (VIC20::VIA1::_ID, (MCHEmul::Chip*) new VIC20::VIA1));
	result.insert (MCHEmul::Chips::value_type (VIC20::VIA2::_ID, (MCHEmul::Chip*) new VIC20::VIA2));

	// The VICI created will depend on whether the visualization is PAL or NTSC...
	// Some how it is also controlled by VIA II
	result.insert (MCHEmul::Chips::value_type (COMMODORE::VICI::_ID, 
		(MCHEmul::Chip*) ((vS == VIC20::CommodoreVIC20::VisualSystem::_NTSC) 
			? (COMMODORE::VICI*) new COMMODORE::VICI_NTSC 
				(VIC20::Memory::_CPU_VIEW, new COMMODORE::VICISoundSimpleLibWrapper 
					(VIC20::CommodoreVIC20::_NTSCCLOCK, COMMODORE::VICI::_SOUNDSAMPLINGCLOCK)) 
			: (COMMODORE::VICI*) new COMMODORE::VICI_PAL
				(VIC20::Memory::_CPU_VIEW, new COMMODORE::VICISoundSimpleLibWrapper 
					(VIC20::CommodoreVIC20::_PALCLOCK, COMMODORE::VICI::_SOUNDSAMPLINGCLOCK))))); 

	return (result);
}

// ---
MCHEmul::IODevices VIC20::CommodoreVIC20::standardDevices ()
{
	MCHEmul::IODevices result;

	// TODO

	return (result);
}
