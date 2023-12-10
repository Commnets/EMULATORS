#include "stdafx.h"

#include <CONSOLE/incs.hpp>
#include <VIC20/incs.hpp>

int _tmain (int argc, _TCHAR *argv [])
{
	// Read the parameters of the command line...
	MCHEmul::CommandLineArguments cmdArgs (argc, argv);
	MCHEmul::IPAddress consoleAddress = 
		(cmdArgs.existsArgument ('i')) 
			? MCHEmul::IPAddress (cmdArgs.argumentAsString ('i')) : MCHEmul::IPAddress (127, 0, 0, 1, 60000);
	unsigned int listenPort = 
		(cmdArgs.existsArgument ('p')) ? cmdArgs.argumentAsInt ('p') : 60001;

	std::cout << "RVIC20Console" << std::endl;
	std::cout << "Copyright (C) 2023 by Ignacio Cea" << std::endl;
	std::cout << "It can send instructions to a C64 emulator and receive the answers from it" << std::endl;
	std::cout << "/h\t\tJust for help" << std::endl; 
	std::cout << "/i[IPADDRESS]\tAddress of the C64 emulator to monitor (xxx (1 to 255).xxx.xxx.xxx:xxxxx (0 - 65535)" << std::endl;
	std::cout << "\t\tBy default 60000. Above 1000 recommended" << std::endl;
	std::cout << "/p[PORTNUMBER]\tPort where to listen the answers from it (0 - 65535)" << std::endl;
	std::cout << "\t\tBy default 60001. Above 1000 recommended" << std::endl << std::endl;

	// The formatter for the messages related with the C64...
	MCHEmul::FormatterBuilder::instance ({ "./VIC20.fmt" });

	// This remote console is only for C64 systems
	// Notice that the command builder passed as parameter is the one done for its parameters...
	MCHEmul::RemoteConsole myConsole (
		new VIC20::CommandBuilder (new MCHEmul::StandardCommandBuilder), 
		new MCHEmul::Win32ConsoleKeys,
		new MCHEmul::CommunicationSystem (
			new MCHEmul::PeerCommunicationChannel (listenPort, 10 /** simultaneous comms. */), 
			new MCHEmul::CommsSystemAnswerCommandBuilder), 
		consoleAddress);
	myConsole.run ();

	return (0);
}
