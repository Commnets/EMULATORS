#include <VIC20/CommandBuilder.hpp>
#include <VIC20/Commands.hpp>

// ---
MCHEmul::Command* VIC20::CommandBuilder::createEmptyCommand (const std::string& cmdName) const
{
	MCHEmul::Command* result = nullptr;

	if (cmdName == "HELP" || cmdName == MCHEmul::HelpCommand::_NAME)
		result = new MCHEmul::HelpCommand ("./VIC20.hlp"); // Adding the commands for a VIC20...
	else if (cmdName == "VIA1" || cmdName == VIC20::VIA1StatusCommand::_NAME)
		result = new VIC20::VIA1StatusCommand;
	else if (cmdName == "VIA2" || cmdName == VIC20::VIA2StatusCommand::_NAME)
		result = new VIC20::VIA2StatusCommand;
	else if (cmdName == "SCREENDUMP" || cmdName == VIC20::ScreenMemoryDUMPCommand::_NAME)
		result = new VIC20::ScreenMemoryDUMPCommand;
	else if (cmdName == "COLORDUMP" || cmdName == VIC20::ColorMemoryDUMPCommand::_NAME)
		result = new VIC20::ColorMemoryDUMPCommand;
	else if (cmdName == "CHARSDRAW" || cmdName == VIC20::CharactersDrawCommand::_NAME)
		result = new VIC20::CharactersDrawCommand;

	return (result);
}
