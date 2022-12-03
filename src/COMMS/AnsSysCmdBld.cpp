#include <COMMS/AnsSysCmdBld.hpp>
#include <COMMS/AnsSysCommand.hpp>

// ---
MCHEmul::Command* MCHEmul::CommsSystemAnswerCommandBuilder::createEmptyCommand (const std::string& cmdName) const
{
	MCHEmul::Command* result = nullptr;

	if (cmdName == MCHEmul::CommsSystemAnswerCommand::_NAME)
		result = new MCHEmul::CommsSystemAnswerCommand;

	return (result);
}
