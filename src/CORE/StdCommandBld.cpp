#include <CORE/StdCommandBld.hpp>
#include <CORE/StdCommands.hpp>

// ---
MCHEmul::Command* MCHEmul::StandardCommandBuilder::createEmptyCommand (const std::string& cmdName) const
{
	MCHEmul::Command* result = nullptr;

	if (cmdName == "HELP")
		result = new MCHEmul::HelpCommand;
	else if (cmdName == "AUTHOR")
		result = new MCHEmul::AuthorInfoCommand;
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
	else if (cmdName == "CPUSSTATUS")
		result = new MCHEmul::CPUSimpleStatusCommand;
	else if (cmdName == "CPUINFO")
		result = new MCHEmul::CPUInfoCommand;
	else if (cmdName == "MEMORY")
		result = new MCHEmul::MemoryStatusCommand;
	else if (cmdName == "SETMEMORY")
		result = new MCHEmul::SetMemoryValueCommand;
	else if (cmdName == "STOP")
		result = new MCHEmul::StopCPUCommand;
	else if (cmdName == "RUN")
		result = new MCHEmul::RunCPUCommand;
	else if (cmdName == "NEXT" || cmdName == "N")
		result = new MCHEmul::NextInstructionCommand;
	else if (cmdName == "INST")
		result = new MCHEmul::LastIntructionCPUCommand;
	else if (cmdName == "BREAKS")
		result = new MCHEmul::ListOfBreakPointsCommand;
	else if (cmdName == "SETBREAK")
		result = new MCHEmul::SetBreakPointCommand;
	else if (cmdName == "REMOVEBREAK")
		result = new MCHEmul::RemoveBreakPointCommand;
	else if (cmdName == "REMOVEBREAKS")
		result = new MCHEmul::RemoveAllBreakPointsCommand;
	else if (cmdName == "SPEED")
		result = new MCHEmul::CPUSpeedCommand;
	else if (cmdName == "LOADBIN")
		result = new MCHEmul::LoadBinCommand;

	return (result);
}
