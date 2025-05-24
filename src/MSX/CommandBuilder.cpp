#include <MSX/CommandBuilder.hpp>
#include <MSX/Commands.hpp>

// ---
MCHEmul::Command* MSX::CommandBuilder::createEmptyCommand (const std::string& cmdName) const
{
	MCHEmul::Command* result = nullptr;

	if (cmdName == "HELP" || cmdName == MCHEmul::HelpCommand::_NAME)
		result = new MCHEmul::HelpCommand ("./MSX.hlp"); // Adding the commands for a MSX...
	else if (cmdName == "VDP" || cmdName == MSX::VDPStatusCommand::_NAME)
		result = new MSX::VDPStatusCommand;
	else if (cmdName == "PNAME" || cmdName == MSX::PatternNameTableDUMPCommand::_NAME)
		result = new MSX::PatternNameTableDUMPCommand;
	else if (cmdName == "PGEN" || cmdName == MSX::PatternGenerationTableDUMPCommand::_NAME)
		result = new MSX::PatternGenerationTableDUMPCommand;
	else if (cmdName == "CNAME" || cmdName == MSX::ColorNameTableDUMPCommand::_NAME)
		result = new MSX::ColorNameTableDUMPCommand;
	else if (cmdName == "SPRITESDRAW" || cmdName == MSX::SpritesDrawCommand::_NAME)
		result = new MSX::SpritesDrawCommand;
	else if (cmdName == "VDPMEMORY" || cmdName == MSX::VDPMemoryStatusCommand::_NAME)
		result = new MSX::VDPMemoryStatusCommand;

	return (result);
}
