#include <COMMODORE/Commands.hpp>
#include <COMMODORE/Computer.hpp>
#include <sstream>

const std::string COMMODORE::VICStatusCommand::_NAME = "CVICII";
const std::string COMMODORE::CIAStatusCommand::_NAME = "CCIA";
const std::string COMMODORE::SIDStatusCommand::_NAME = "CSID";

// ---
void COMMODORE::VICStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a COMMODORE one. 
	if (c == nullptr || 
		dynamic_cast <COMMODORE::Computer*> (c) == nullptr ||
		static_cast <COMMODORE::Computer*> (c) -> vicII () == nullptr)
		return;

	rst.add ("VICII", static_cast <COMMODORE::Computer*> (c) -> vicII () -> getInfoStructure ());
}

// ---
void COMMODORE::CIAStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a COMMODORE one. 
	if (c == nullptr || 
		dynamic_cast <COMMODORE::Computer*> (c) == nullptr ||
		static_cast <COMMODORE::Computer*> (c) -> cia () == nullptr)
		return;

	rst.add ("CIA", static_cast <COMMODORE::Computer*> (c) -> cia () -> getInfoStructure ());
}

// ---
void COMMODORE::SIDStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a COMMODORE one. 
	if (c == nullptr || 
		dynamic_cast <COMMODORE::Computer*> (c) == nullptr ||
		static_cast <COMMODORE::Computer*> (c) -> sid () == nullptr)
		return;

	rst.add ("SID", static_cast <COMMODORE::Computer*> (c) -> sid () -> getInfoStructure ());
}
