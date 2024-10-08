#include <CONSOLE/RemoteConsole.hpp>

// ---
void MCHEmul::RemoteConsole::run ()
{
	if (!_communicationSystem -> initialize ())
	{
		outputStream () << "Error initializating communciations" << std::endl;

		return; // No possible to run the remote console...
	}

	// The communication system is liked to this one...
	_communicationSystem -> deriveAnswerCommandTo (this);

	// Then, after initializing communciations, 
	// the remote console can start to work!
	MCHEmul::Console::run ();
}

// ---
bool MCHEmul::RemoteConsole::runPerCycle ()
{
	if (!_communicationSystem -> processMessagesOn (nullptr))
		outputStream () << "Error processing messages" << std::endl;

	setConsoleHold (_communicationSystem -> waitingForAnswer ());

	return (false); // The main loop doesn't finish even if there is processing errors...
}

// ---
void MCHEmul::RemoteConsole::createAndExecuteCommand ()
{
	// The command has to be defined...
	// This quickest the way to tested, reusing the code defined in other part!
	MCHEmul::Command* cmd = _commandBuilder -> command (_command);
	if (cmd == nullptr)
	{
		outputStream () << _command << ":" << _commandDoesnExitTxt << std::endl;

		return;
	}

	// When the command can be executed locally, means that no computer is needed behind...
	// This is guaranteed by the instruction "localCommand".
	if (localCommand (cmd))
	{ 
		MCHEmul::InfoStructure rst;
		executeCommandNow (cmd, nullptr /** No computer needed. */);
	}
	else
	{ 
		if (!_communicationSystem -> send (_command, _to))
			outputStream () << "Error sending command:" << _command << " to:" << _to << std::endl;
		else
		{ 
			// The console is hold until the answer comes...
			setConsoleHold (true);

			outputStream () << _command << " sent to:" << _to << std::endl;
		}
	}
}
