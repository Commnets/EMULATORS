#include <CORE/CmdExecuter.hpp>
#include <CORE/Command.hpp>

// ---
void MCHEmul::CommandExecuter::executeCommand (MCHEmul::Command* cmd, MCHEmul::Computer* c)
{
	if (cmd == nullptr)
		return; // No possible to execute ever...

	// The computer can be null, but take a lot of care, checking that before the execution...
	_commands.push (MCHEmul::CommandExecuter::ComputerPlusCommand (c, cmd));
}

// ---
bool MCHEmul::CommandExecuter::executeCommandNow (MCHEmul::Command* cmd, MCHEmul::Computer* c)
{
	bool result = false;

	MCHEmul::InfoStructure rst;
	if (result = cmd -> execute (this, c, rst))
	{
		if (!rst.empty ())
			manageAnswer (cmd, rst);
	}
	else
		manageErrorInExecution (cmd, rst);

	return (result);
}

// ---
bool MCHEmul::CommandExecuter::executePendingCommands ()
{
	bool result = true;

	while (!_commands.empty ())
	{
		result |= executeCommandNow (_commands.front ()._command, _commands.front ()._computer);

		_commands.pop ();
	}

	return (result);
}
