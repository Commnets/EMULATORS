#include <C64/CommandBuilder.hpp>
#include <C64/Commands.hpp>

// ---
MCHEmul::Command* C64::CommandBuilder::createEmptyCommand (const std::string& cmdName) const
{
	MCHEmul::Command* result = nullptr;
	if (cmdName == "VICII")
		result = new C64::VICStatusCommand;
	else
		result = MCHEmul::StandardCommandBuilder::createEmptyCommand (cmdName);

	return (result);
}
