#include "stdafx.h"

#include <CONSOLE/incs.hpp>

int _tmain (int argc, _TCHAR *argv [])
{
	// Formatters for the messages received mainly...
	MCHEmul::FormatterBuilder::instance ({ "./conformatters.fmt" });

	// The console doesn't know anything about the remote emulator being executeD
	// ...So only the standard command are understood!
	MCHEmul::RemoteConsole myConsole (new MCHEmul::StandardCommandBuilder, new MCHEmul::Win32ConsoleKeys,
		new MCHEmul::CommunicationSystem (
			new MCHEmul::PeerCommunicationChannel (60001 /** port. */, 10 /** simultaneous comms. */), 
				new MCHEmul::StandardCommandBuilder),
		MCHEmul::IPAddress (127, 0, 0, 1, 60000 /** Where the simulations is baeing executed. */));
	myConsole.run ();

	return (0);
}
