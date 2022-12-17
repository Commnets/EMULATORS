#include "stdafx.h"

#include <CONSOLE/incs.hpp>

int _tmain (int argc, _TCHAR *argv [])
{
	// Read the parameters of the command line...
	MCHEmul::CommandLineArguments cmdArgs (argc, argv);
	MCHEmul::IPAddress consoleAddress = 
		(cmdArgs.existsArgument ('i')) 
		? MCHEmul::IPAddress (cmdArgs.argumentAsString ('i')) : MCHEmul::IPAddress (127, 0, 0, 1, 60000);
	unsigned int listenPort = 
		(cmdArgs.existsArgument ('p')) ? cmdArgs.argumentAsInt ('p') : 60001;

	std::cout << "RConsole" << std::endl;
	std::cout << "Copyright (C) 2022 by Ignacio Cea" << std::endl;
	std::cout << "It can send instructions to a CPU emulator and receive the answers from it." << std::endl;
	std::cout << "/i[IPADDRESS]\tAddress of the emulator to monitor (xxx (1 to 255).xxx.xxx.xxx:xxxxx (0 - 65535)." << std::endl;
	std::cout << "/p[PORTNUMBER]\tPort where to listen the answers from it (0 - 65535)." << std::endl;
	std::cout << "\t\tAbove 1000 recommended" << std::endl << std::endl;

	// Formatters for the messages received mainly...
	MCHEmul::FormatterBuilder::instance ({ "./conformatters.fmt" });

	// It this case the console doesn't know anything about the remote emulator being executeD
	// ...So only the standard command are understood!
	MCHEmul::RemoteConsole myConsole (new MCHEmul::StandardCommandBuilder, new MCHEmul::Win32ConsoleKeys,
		new MCHEmul::CommunicationSystem (
			new MCHEmul::PeerCommunicationChannel (listenPort, 10 /** simultaneous comms. */), 
				new MCHEmul::CommsSystemAnswerCommandBuilder), consoleAddress);
	myConsole.run ();

	return (0);
}
