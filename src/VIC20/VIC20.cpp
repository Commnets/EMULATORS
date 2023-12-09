#include <VIC20/VIC20.hpp>
#include <F6500/C6502.hpp>

// ---
VIC20::CommodoreVIC20::CommodoreVIC20 (VIC20::CommodoreVIC20::VisualSystem vS, const std::string& lg)
	: COMMODORE::Computer 
		(new F6500::C6502 (),
		 VIC20::CommodoreVIC20::standardChips (),
		 nullptr, // The memory TODO
		 VIC20::CommodoreVIC20::standardDevices (),
		 vS == VIC20::CommodoreVIC20::VisualSystem::_PAL ? _PALCLOCK : _NTSCCLOCK,
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

	return (true);
}

// ---
void VIC20::CommodoreVIC20::processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n)
{
	// TODO
}

// ---
MCHEmul::Chips VIC20::CommodoreVIC20::standardChips ()
{
	MCHEmul::Chips result;

	// TODO

	return (result);
}

// ---
MCHEmul::IODevices VIC20::CommodoreVIC20::standardDevices ()
{
	MCHEmul::IODevices result;

	// TODO

	return (result);
}
