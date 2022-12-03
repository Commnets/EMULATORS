#include <C64/CommandBuilder.hpp>
#include <C64/Commands.hpp>

// ---
MCHEmul::Command* C64::CommandBuilder::createEmptyCommand (const std::string& cmdName) const
{
	MCHEmul::Command* result = nullptr;

	if (cmdName == "HELP")
		result = new MCHEmul::HelpCommand ("./C64commands.hlp"); // Adding the commands for a C64...
	if (cmdName == "VICII")
		result = new C64::VICStatusCommand;
	else if (cmdName == "CIA1")
		result = new C64::CIA1StatusCommand;
	else if (cmdName == "CIA2")
		result = new C64::CIA2StatusCommand;
	else
		result = MCHEmul::StandardCommandBuilder::createEmptyCommand (cmdName);

	return (result);
}
