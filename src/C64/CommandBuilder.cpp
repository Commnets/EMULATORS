#include <C64/CommandBuilder.hpp>
#include <C64/Commands.hpp>

// ---
MCHEmul::Command* C64::CommandBuilder::createEmptyCommand (const std::string& cmdName) const
{
	MCHEmul::Command* result = nullptr;

	if (cmdName == "HELP" || cmdName == MCHEmul::HelpCommand::_NAME)
		result = new MCHEmul::HelpCommand ("./C64commands.hlp"); // Adding the commands for a C64...
	else if (cmdName == "VICII" || cmdName == C64::VICStatusCommand::_NAME)
		result = new C64::VICStatusCommand;
	else if (cmdName == "CIA1" || cmdName == C64::CIA1StatusCommand::_NAME)
		result = new C64::CIA1StatusCommand;
	else if (cmdName == "CIA2" || cmdName == C64::CIA2StatusCommand::_NAME)
		result = new C64::CIA2StatusCommand;
	else if (cmdName == "SID" || cmdName == C64::SIDStatusCommand::_NAME)
		result = new C64::SIDStatusCommand;
	else
		result = MCHEmul::StandardCommandBuilder::createEmptyCommand (cmdName);

	return (result);
}
