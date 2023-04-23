#include <COMMODORE/CommandBuilder.hpp>
#include <COMMODORE/Commands.hpp>

// ---
MCHEmul::Command* COMMODORE::CommandBuilder::createEmptyCommand (const std::string& cmdName) const
{
	MCHEmul::Command* result = nullptr;

	if (cmdName == "VICII" || cmdName == COMMODORE::VICStatusCommand::_NAME)
		result = new COMMODORE::VICStatusCommand;
	else if (cmdName == "CIA" || cmdName == COMMODORE::CIAStatusCommand::_NAME)
		result = new COMMODORE::CIAStatusCommand;
	else if (cmdName == "SID" || cmdName == COMMODORE::SIDStatusCommand::_NAME)
		result = new COMMODORE::SIDStatusCommand;
	else if (cmdName == "SIDW" || cmdName == COMMODORE::SIDWrapperCommand::_NAME)
		result = new COMMODORE::SIDWrapperCommand;
	else if (cmdName == "DATASETTE" || cmdName == COMMODORE::DatasetteStatusCommand::_NAME)
		result = new COMMODORE::DatasetteStatusCommand;

	return (result);
}
