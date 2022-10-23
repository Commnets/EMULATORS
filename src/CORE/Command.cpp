#include <CORE/Command.hpp>
#include <CORE/Computer.hpp>

// ---
bool MCHEmul::Command::execute (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	if (cE == nullptr || c == nullptr || !canBeExecuted ())
		return (false);

	executeImpl (cE, c, rst);

	return (true);
}

// ---
bool MCHEmul::ComplexCommand::canBeExecuted () const
{
	bool result = true;
	// If just one can not be exected, it can not be executed either.
	for (MCHEmul::Commands::const_iterator i = _commands.begin ();
		i != _commands.end () && result; result &= (*i++) -> canBeExecuted ()); 
	return (result);
}

// ---
bool MCHEmul::ComplexCommand::execute (MCHEmul::CommandExecuter* cE, MCHEmul::Computer* c, MCHEmul::InfoStructure& rst)
{
	bool result = true;

	// All of them are executed, 
	// but the result is false if just one of them couldn't be executed...
	for (const auto i : _commands)
		result &= i -> execute (cE, c, rst);

	return (result);
}

