#include <MSX/Commands.hpp>
#include <MSX/MSX.hpp>
#include <MSX/Screen.hpp>

// ---
const std::string MSX::VDPStatusCommand::_NAME = "CVDP";

// ---
void MSX::VDPStatusCommand::executeImpl 
	(MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a TEXASINSTRUMENTS one. 
	if (c == nullptr || 
		dynamic_cast <MSX::MSXComputer*> (c) == nullptr ||
		static_cast <MSX::MSXComputer*> (c) -> vdp () == nullptr)
		return;

	MSX::VDP* vdp = static_cast <MSX::MSXComputer*> (c) -> vdp ();

	// The structure is generated for the right chip!
	rst.add (vdp -> graphicalChip () -> className (), 
		std::move (static_cast <MSX::MSXComputer*> (c) -> vdp () -> getInfoStructure ()));
}
