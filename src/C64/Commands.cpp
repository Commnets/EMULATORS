#include <C64/Commands.hpp>
#include <C64/C64.hpp>
#include <sstream>

const std::string C64::CIA1StatusCommand::_NAME = "CCIA1";
const std::string C64::CIA2StatusCommand::_NAME = "CCIA2";

// ---
void C64::CIA1StatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a C64 one. 
	if (c == nullptr || 
		dynamic_cast <C64::Commodore64*> (c) == nullptr ||
		static_cast <C64::Commodore64*> (c) -> cia1 () == nullptr)
		return;

	rst.add ("CIA1", std::move (static_cast <C64::Commodore64*> (c) -> cia1 () -> getInfoStructure ()));
}

// ---
void C64::CIA2StatusCommand::executeImpl (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	// Only with a valid computer, but also a C64 one. 
	if (c == nullptr || 
		dynamic_cast <C64::Commodore64*> (c) == nullptr ||
		static_cast <C64::Commodore64*> (c) -> cia2 () == nullptr)
		return;

	// Only with a valid computer, but also a C64 one. 
	rst.add ("CIA2", std::move (static_cast <C64::Commodore64*> (c) -> cia2 () -> getInfoStructure ()));
}
