#include <SINCLAIR/CommandBuilder.hpp>
#include <SINCLAIR/Commands.hpp>

// ---
MCHEmul::Command* SINCLAIR::CommandBuilder::createEmptyCommand (const std::string& cmdName) const
{
	MCHEmul::Command* result = nullptr;
	
	if (cmdName == "SYSVARS" || cmdName == SINCLAIR::SystemVariablesCommand::_NAME)
		result = new SINCLAIR::SystemVariablesCommand;
	else if (cmdName == "SYSVAR" || cmdName == SINCLAIR::SystemVariableCommand::_NAME)
		result = new SINCLAIR::SystemVariableCommand;

	return (result);
}
