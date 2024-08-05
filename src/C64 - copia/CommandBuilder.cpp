#include <C64/CommandBuilder.hpp>
#include <C64/Commands.hpp>

// ---
MCHEmul::Command* C64::CommandBuilder::createEmptyCommand (const std::string& cmdName) const
{
	MCHEmul::Command* result = nullptr;

	if (cmdName == "HELP" || cmdName == MCHEmul::HelpCommand::_NAME)
		result = new MCHEmul::HelpCommand ("./C64.hlp"); // Adding the commands for a C64...
	else if (cmdName == "CIA1" || cmdName == C64::CIA1StatusCommand::_NAME)
		result = new C64::CIA1StatusCommand;
	else if (cmdName == "CIA2" || cmdName == C64::CIA2StatusCommand::_NAME)
		result = new C64::CIA2StatusCommand;
	else if (cmdName == "PLA" || cmdName == C64::PLAStatusCommand::_NAME)
		result = new C64::PLAStatusCommand;
	else if (cmdName == "SCREENDUMP" || cmdName == C64::ScreenMemoryDUMPCommand::_NAME)
		result = new C64::ScreenMemoryDUMPCommand;
	else if (cmdName == "COLORDUMP" || cmdName == C64::ColorMemoryDUMPCommand::_NAME)
		result = new C64::ColorMemoryDUMPCommand;
	else if (cmdName == "BITMAPDUMP" || cmdName == C64::BitmapMemoryDUMPCommand::_NAME)
		result = new C64::BitmapMemoryDUMPCommand;
	else if (cmdName == "SPRITESDUMP" || cmdName == C64::SpritesMemoryDUMPCommand::_NAME)
		result = new C64::SpritesMemoryDUMPCommand;
	else if (cmdName == "SPRITESDRAW" || cmdName == C64::SpritesDrawCommand::_NAME)
		result = new C64::SpritesDrawCommand;
	else if (cmdName == "CHARSDRAW" || cmdName == C64::CharactersDrawCommand::_NAME)
		result = new C64::CharactersDrawCommand;

	return (result);
}
