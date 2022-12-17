#include "stdafx.h"

#include <C64/incs.hpp>

#ifndef _CONSOLE
#include <SDL.h>
#ifdef __cplusplus
#define C_LINKAGE "C"
#else
#define C_LINKAGE
#endif /* __cplusplus */
#if _MSC_VER >= 1900
extern C_LINKAGE FILE __iob_func[3] = { *stdin,*stdout,*stderr };
#endif
extern C_LINKAGE int main(int argc, char* argv[])
#else
int _tmain (int argc, _TCHAR* argv[])
#endif /* _CONSOLE */
{
	// Read the parameters of the command line...
	MCHEmul::CommandLineArguments cmdArgs (argc, argv);
	unsigned int listenPort = (cmdArgs.existsArgument ('p')) ? cmdArgs.argumentAsInt ('p') : 60000;

	std::cout << "C64Emulator" << std::endl;
	std::cout << "Copyright (C) 2022 by Ignacio Cea" << std::endl;
	std::cout << "It can receive commands from external tools through the port." << std::endl;
	std::cout << "/p[PORTNUMBER]\tPort where to listen external instructions (0 - 65535)." << std::endl; 
	std::cout << "\t\tAbove 1000 recommended" << std::endl << std::endl;

	// Sets the formatter for << outputs for this emulation...
	// This line of code has to be at the beginning of every emulation 
	// as it fixes the way the internal ino of the important classes will be formatted
	MCHEmul::FormatterBuilder::instance ({ "./C64formatters.fmt" });

	// Set up the emulator...
	C64::C64Emulator myEmulator (
		MCHEmul::CommandLineArguments (argc, argv),
		new MCHEmul::CommunicationSystem (
			new MCHEmul::PeerCommunicationChannel (listenPort, 10 /** simultaneous comms. */), new C64::CommandBuilder));
	if (!myEmulator || !myEmulator.initialize ())
		return (1); // Not possible to run the emulation. Exit with error...

	return (myEmulator.run () ? 0 /** no error. */ : 1);
}
