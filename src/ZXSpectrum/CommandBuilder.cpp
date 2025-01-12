#include <ZXSpectrum/CommandBuilder.hpp>
#include <ZXSpectrum/Commands.hpp>

// ---
MCHEmul::Command* ZXSPECTRUM::CommandBuilder::createEmptyCommand (const std::string& cmdName) const
{
	MCHEmul::Command* result = nullptr;

	if (cmdName == "HELP" || cmdName == MCHEmul::HelpCommand::_NAME)
		result = new MCHEmul::HelpCommand ("./ZXSpectrum.hlp"); // Adding the commands for a ZX81...
	else if (cmdName == "ULA" || cmdName == ZXSPECTRUM::ULAStatusCommand::_NAME)
		result = new ZXSPECTRUM::ULAStatusCommand;
	else if (cmdName == "ULAEVENTS" || cmdName == ZXSPECTRUM::ULAShowEventsCommand::_NAME)
		result = new ZXSPECTRUM::ULAShowEventsCommand;
	else if (cmdName == "SCREENDUMP" || cmdName == ZXSPECTRUM::ScreenMemoryDUMPCommand::_NAME)
		result = new ZXSPECTRUM::ScreenMemoryDUMPCommand;
	else if (cmdName == "COLORDUMP" || cmdName == ZXSPECTRUM::ColorMemoryDUMPCommand::_NAME)
		result = new ZXSPECTRUM::ColorMemoryDUMPCommand;
	else if (cmdName == "CHARSDRAW" || cmdName == ZXSPECTRUM::CharactersDrawCommand::_NAME)
		result = new ZXSPECTRUM::CharactersDrawCommand;

	return (result);
}
