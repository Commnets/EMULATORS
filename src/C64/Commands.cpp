#include <C64/Commands.hpp>
#include <C64/C64.hpp>
#include <sstream>

// ---
void C64::VICStatusCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::Attributes& rst)
{
	std::stringstream ss;
	ss << *dynamic_cast <C64::Commodore64*> (c) -> vicII ();
	rst.insert (std::pair <std::string, std::string>  ("1", ss.str ()));
}

// ---
void C64::CIA1StatusCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::Attributes& rst)
{
	std::stringstream ss;
	ss << *dynamic_cast <C64::Commodore64*> (c) -> cia1 ();
	rst.insert (std::pair <std::string, std::string>  ("1", ss.str ()));
}

// ---
void C64::CIA2StatusCommand::executeImpl (MCHEmul::Computer* c, MCHEmul::Attributes& rst)
{
	std::stringstream ss;
	ss << *dynamic_cast <C64::Commodore64*> (c) -> cia2 ();
	rst.insert (std::pair <std::string, std::string>  ("1", ss.str ()));
}
