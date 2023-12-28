#include <COMMODORE/CommandBuilder.hpp>
#include <COMMODORE/Commands.hpp>

// ---
MCHEmul::Command* COMMODORE::CommandBuilder::createEmptyCommand (const std::string& cmdName) const
{
	MCHEmul::Command* result = nullptr;

	if (cmdName == "VICI" || cmdName == COMMODORE::VICIStatusCommand::_NAME)
		result = new COMMODORE::VICIStatusCommand;
	else if (cmdName == "VICII" || cmdName == COMMODORE::VICIIStatusCommand::_NAME)
		result = new COMMODORE::VICIIStatusCommand;
	else if (cmdName == "VIA" || cmdName == COMMODORE::VIAStatusCommand::_NAME)
		result = new COMMODORE::VIAStatusCommand;
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
