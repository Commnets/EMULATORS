#include <C264/CommandBuilder.hpp>
#include <C264/Commands.hpp>

// ---
MCHEmul::Command* C264::CommandBuilder::createEmptyCommand (const std::string& cmdName) const
{
	MCHEmul::Command* result = nullptr;

	if (cmdName == "HELP" || cmdName == MCHEmul::HelpCommand::_NAME)
		result = new MCHEmul::HelpCommand ("./C264.hlp"); // Adding the commands for a C264...

	return (result);
}
