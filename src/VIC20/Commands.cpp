#include <VIC20/Commands.hpp>
#include <VIC20/VIC20.hpp>
#include <sstream>

const std::string VIC20::VIA1StatusCommand::_NAME = "CVIA1";
const std::string VIC20::VIA2StatusCommand::_NAME = "CVIA2";
const std::string VIC20::ScreenMemoryDUMPCommand::_NAME = "CSCREENDUMP";
const std::string VIC20::ColorMemoryDUMPCommand::_NAME = "CCOLORDUMP";

// ---
void VIC20::VIA1StatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a VIC20 one. 
	if (c == nullptr || 
		dynamic_cast <VIC20::CommodoreVIC20*> (c) == nullptr ||
		static_cast <VIC20::CommodoreVIC20*> (c) -> via1 () == nullptr)
		return;

	rst.add ("VIA1", std::move (static_cast <VIC20::CommodoreVIC20*> (c) -> via1 () -> getInfoStructure ()));
}

// ---
void VIC20::VIA2StatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a VIC20 one. 
	if (c == nullptr || 
		dynamic_cast <VIC20::CommodoreVIC20*> (c) == nullptr ||
		static_cast <VIC20::CommodoreVIC20*> (c) -> via2 () == nullptr)
		return;

	// Only with a valid computer, but also a C64 one. 
	rst.add ("VIA2", std::move (static_cast <VIC20::CommodoreVIC20*> (c) -> via2 () -> getInfoStructure ()));
}

// ---
void VIC20::ScreenMemoryDUMPCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <VIC20::CommodoreVIC20*> (c) == nullptr ||
		static_cast <VIC20::CommodoreVIC20*> (c) -> vicI () == nullptr)
		return;

	rst.add ("BYTES", 
		static_cast <VIC20::CommodoreVIC20*> (c) -> vicI () -> screenMemorySnapShot (c -> cpu ()));
}

// ---
void VIC20::ColorMemoryDUMPCommand::executeImpl (MCHEmul::CommandExecuter* cE,
	MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr || 
		dynamic_cast <VIC20::CommodoreVIC20*> (c) == nullptr ||
		static_cast <VIC20::CommodoreVIC20*> (c) -> vicI () == nullptr)
		return;

	rst.add ("BYTES", 
		static_cast <VIC20::CommodoreVIC20*> (c) -> vicI () -> colorMemorySnapShot (c -> cpu ()));
}
