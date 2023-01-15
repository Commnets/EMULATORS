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

	return (result);
}
