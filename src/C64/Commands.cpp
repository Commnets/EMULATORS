#include <C64/Commands.hpp>
#include <C64/C64.hpp>
#include <sstream>

const std::string C64::VICStatusCommand::_NAME = "CVICII";
const std::string C64::CIA1StatusCommand::_NAME = "CCIA1";
const std::string C64::CIA2StatusCommand::_NAME = "CCIA2";


// ---
void C64::VICStatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	rst.add ("VICII", dynamic_cast <C64::Commodore64*> (c) -> vicII () -> getInfoStructure ());
}

// ---
void C64::CIA1StatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	rst.add ("CIA1", dynamic_cast <C64::Commodore64*> (c) -> cia1 () -> getInfoStructure ());
}

// ---
void C64::CIA2StatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (c == nullptr)
		return;

	rst.add ("CIA2", dynamic_cast <C64::Commodore64*> (c) -> cia2 () -> getInfoStructure ());
}
