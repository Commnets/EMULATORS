#include <C64/Commands.hpp>
#include <C64/C64.hpp>
#include <sstream>

// ---
void C64::VICStatusCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	rst = dynamic_cast <C64::Commodore64*> (c) -> vicII () -> getInfoStructure ();
}

// ---
void C64::CIA1StatusCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	rst = dynamic_cast <C64::Commodore64*> (c) -> cia1 () -> getInfoStructure ();
}

// ---
void C64::CIA2StatusCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	rst = dynamic_cast <C64::Commodore64*> (c) -> cia2 () -> getInfoStructure ();
}
