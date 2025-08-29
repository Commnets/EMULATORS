#include <CORE/ComputerAction.hpp>
#include <CORE/Computer.hpp>

// ---
bool MCHEmul::CompositeComputerAction::execute (MCHEmul::Computer* c)
{
	bool result = true;
	for (auto& i : _actions)
		result &= i -> execute (c);
	return (result);
}
