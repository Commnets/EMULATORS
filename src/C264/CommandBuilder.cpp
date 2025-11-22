#include <C264/CommandBuilder.hpp>
#include <C264/Commands.hpp>

// ---
MCHEmul::Command* C264::CommandBuilder::createEmptyCommand (const std::string& cmdName) const
{
	MCHEmul::Command* result = nullptr;

	if (cmdName == "HELP" || cmdName == MCHEmul::HelpCommand::_NAME)
		result = new MCHEmul::HelpCommand ("./C264.hlp"); // Adding the commands for a C264...
	else if (cmdName == "SCREENDUMP" || cmdName == C264::ScreenMemoryDUMPCommand::_NAME)
		result = new C264::ScreenMemoryDUMPCommand;
	else if (cmdName == "ATTRIBUTEDUMP" || cmdName == C264::AttributeMemoryDUMPCommand::_NAME)
		result = new C264::AttributeMemoryDUMPCommand;
	else if (cmdName == "BITMAPDUMP" || cmdName == C264::BitmapMemoryDUMPCommand::_NAME)
		result = new C264::BitmapMemoryDUMPCommand;
	else if (cmdName == "CHARSDRAW" || cmdName == C264::CharactersDrawCommand::_NAME)
		result = new C264::CharactersDrawCommand;
	else if (cmdName == "GRIDON" || cmdName == C264::GridOnCommand::_NAME)
		result = new C264::GridOnCommand;
	else if (cmdName == "GRIDOFF" || cmdName == C264::GridOffCommand::_NAME)
		result = new C264::GridOffCommand;

	return (result);
}
