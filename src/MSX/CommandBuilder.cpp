#include <MSX/CommandBuilder.hpp>
#include <MSX/Commands.hpp>

// ---
MCHEmul::Command* MSX::CommandBuilder::createEmptyCommand (const std::string& cmdName) const
{
	MCHEmul::Command* result = nullptr;

	if (cmdName == "VDP" || cmdName == MSX::VDPStatusCommand::_NAME)
		result = new MSX::VDPStatusCommand;

	return (result);
}
