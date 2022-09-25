#include <CORE/Command.hpp>
#include <CORE/Computer.hpp>

// ---
bool MCHEmul::Command::execute (MCHEmul::Computer* c, MCHEmul::Attributes& rst)
{
	if (c == nullptr || !canBeExecuted ())
		return (false);

	rst = { };

	executeImpl (c, rst);

	return (true);
}
