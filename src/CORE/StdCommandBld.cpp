#include <CORE/StdCommandBld.hpp>
#include <CORE/StdCommands.hpp>

// ---
MCHEmul::Command* MCHEmul::StandardCommandBuilder::createEmptyCommand (const std::string& cmdName) const
{
	MCHEmul::Command* result = nullptr;

	if (cmdName == "HELP" || cmdName == MCHEmul::HelpCommand::_NAME)
		result = new MCHEmul::HelpCommand;
	else if (cmdName == "AUTHOR" || cmdName == MCHEmul::AuthorInfoCommand::_NAME)
		result = new MCHEmul::AuthorInfoCommand;
	else if (cmdName == "STATUS" || cmdName == MCHEmul::StatusRegisterStatusCommand::_NAME)
		result = new MCHEmul::StatusRegisterStatusCommand;
	else if (cmdName == "PC" || cmdName == MCHEmul::ProgramCounterStatusCommand::_NAME)
		result = new MCHEmul::ProgramCounterStatusCommand;
	else if (cmdName == "REGISTERS" || cmdName == MCHEmul::RegistersStatusCommand::_NAME)
		result = new MCHEmul::RegistersStatusCommand;
	else if (cmdName == "STACK" || cmdName == MCHEmul::StackStatusCommand::_NAME)
		result = new MCHEmul::StackStatusCommand;
	else if (cmdName == "CPUSTATUS" || cmdName == MCHEmul::CPUStatusCommand::_NAME)
		result = new MCHEmul::CPUStatusCommand;
	else if (cmdName == "CPUSSTATUS" || cmdName == MCHEmul::CPUSimpleStatusCommand::_NAME)
		result = new MCHEmul::CPUSimpleStatusCommand;
	else if (cmdName == "CPUINFO" || cmdName == MCHEmul::CPUInfoCommand::_NAME)
		result = new MCHEmul::CPUInfoCommand;
	else if (cmdName == "MEMORY" || cmdName == MCHEmul::MemoryStatusCommand::_NAME)
		result = new MCHEmul::MemoryStatusCommand;
	else if (cmdName == "SETMEMORY" || cmdName == MCHEmul::SetMemoryValueCommand::_NAME)
		result = new MCHEmul::SetMemoryValueCommand;
	else if (cmdName == "STOP" || cmdName == MCHEmul::StopCPUCommand::_NAME)
		result = new MCHEmul::StopCPUCommand;
	else if (cmdName == "RUN" || cmdName == "R" || cmdName == MCHEmul::RunCPUCommand::_NAME)
		result = new MCHEmul::RunCPUCommand;
	else if (cmdName == "NEXT" || cmdName == "N" || cmdName == MCHEmul::NextInstructionCommand::_NAME)
		result = new MCHEmul::NextInstructionCommand;
	else if (cmdName == "INST" || cmdName == MCHEmul::LastIntructionCPUCommand::_NAME)
		result = new MCHEmul::LastIntructionCPUCommand;
	else if (cmdName == "BREAKS" || cmdName == MCHEmul::ListOfBreakPointsCommand::_NAME)
		result = new MCHEmul::ListOfBreakPointsCommand;
	else if (cmdName == "SETBREAK" || cmdName == MCHEmul::SetBreakPointCommand::_NAME)
		result = new MCHEmul::SetBreakPointCommand;
	else if (cmdName == "REMOVEBREAK" || cmdName == MCHEmul::RemoveBreakPointCommand::_NAME)
		result = new MCHEmul::RemoveBreakPointCommand;
	else if (cmdName == "REMOVEBREAKS" || cmdName == MCHEmul::RemoveAllBreakPointsCommand::_NAME)
		result = new MCHEmul::RemoveAllBreakPointsCommand;
	else if (cmdName == "SPEED" || cmdName == MCHEmul::CPUSpeedCommand::_NAME)
		result = new MCHEmul::CPUSpeedCommand;
	else if (cmdName == "LOADBIN" || cmdName == MCHEmul::LoadBinCommand::_NAME)
		result = new MCHEmul::LoadBinCommand;
	else if (cmdName == MCHEmul::MoveParametersToAnswerCommand::_NAME)
		result = new MCHEmul::MoveParametersToAnswerCommand;

	return (result);
}
