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
bool MCHEmul::ComplexCommand::execute (MCHEmul::Computer* c, MCHEmul::Attributes& rst)
{
	bool result = true;

	int ct = 1;
	// All of them are executed, but the result is false if just one of them couldn't be executed...
	for (MCHEmul::Commands::const_iterator i = _commands.begin (); i != _commands.end (); i++)
	{
		MCHEmul::Attributes rstP;
		result &= (*i) -> execute (c, rstP);
		for (const auto j : rstP)
			rst.insert (std::pair <std::string, std::string> (std::to_string (ct++), j.second));
	}

	return (result);
}

