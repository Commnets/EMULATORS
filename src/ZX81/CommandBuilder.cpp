#include <ZX81/CommandBuilder.hpp>
#include <ZX81/Commands.hpp>

// ---
MCHEmul::Command* ZX81::CommandBuilder::createEmptyCommand (const std::string& cmdName) const
{
	MCHEmul::Command* result = nullptr;

	if (cmdName == "HELP" || cmdName == MCHEmul::HelpCommand::_NAME)
		result = new MCHEmul::HelpCommand ("./ZX81.hlp"); // Adding the commands for a ZX81...
	else if (cmdName == "ULA" || cmdName == ZX81::ULAStatusCommand::_NAME)
		result = new ZX81::ULAStatusCommand;

	return (result);
}
