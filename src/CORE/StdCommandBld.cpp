#include <CORE/StdCommandBld.hpp>
#include <CORE/StdCommands.hpp>

// ---
MCHEmul::Command* MCHEmul::StandardCommandBuilder::createEmptyCommand (const std::string& cmdName) const
{
	MCHEmul::Command* result = nullptr;

	if (cmdName == "HELP")
		result = new MCHEmul::HelpCommand;
	else if (cmdName == "STATUS")
		result = new MCHEmul::StatusRegisterStatusCommand;
	else if (cmdName == "PC")
		result = new MCHEmul::ProgramCounterStatusCommand;
	else if (cmdName == "REGISTERS")
		result = new MCHEmul::RegistersStatusCommand;
	else if (cmdName == "STACK")
		result = new MCHEmul::StackStatusCommand;
	else if (cmdName == "CPUSTATUS")
		result = new MCHEmul::CPUStatusCommand;
	else if (cmdName == "CPUINFO")
		result = new MCHEmul::CPUInfoCommand;

	return (result);
}
