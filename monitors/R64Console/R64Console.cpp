#include "stdafx.h"

#include <CONSOLE/incs.hpp>
#include <C64/incs.hpp>

int _tmain (int argc, _TCHAR *argv [])
{
	// The formatter for the messages related with the C64...
	MCHEmul::FormatterBuilder::instance ({ "./C64formatters.fmt" });

	// This remote console is only for C64 systems
	// Notice that the command builder passed as parameter is the one done for its parameters...
	MCHEmul::RemoteConsole myConsole (new C64::CommandBuilder, new MCHEmul::Win32ConsoleKeys,
		new MCHEmul::CommunicationSystem (
			new MCHEmul::PeerCommunicationChannel (60001 /** port. */, 10 /** simultaneous comms. */), 
				new MCHEmul::CommsSystemAnswerCommandBuilder),
		MCHEmul::IPAddress (127, 0, 0, 1, 60000 /** Where the simulations is being executed. */));
	myConsole.run ();

	return (0);
}
