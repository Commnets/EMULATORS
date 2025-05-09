#include "stdafx.h"

#include <C264/incs.hpp>

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
	// Log to a file...
	MCHEmul::LogSystem::_LOGSYSTEM -> addLogChannel 
		(new MCHEmul::LogFileChannel (0, "./C264E.log", 10000 /** events max. */));
	MCHEmul::LogSystem::_LOGSYSTEM -> setDefaultChannelId (0);

	// Read the parameters of the command line...
	MCHEmul::CommandLineArguments cmdArgs (argc, argv);
	bool eOpen = (cmdArgs.existsArgument ('o')) ? cmdArgs.argumentAsBool ('o') : false;
	unsigned int listenPort = (cmdArgs.existsArgument ('p')) ? cmdArgs.argumentAsInt ('p') : 60000;

	std::cout << "C264Emulator" << std::endl;
	std::cout << "Copyright (C) 2024 by Ignacio Cea" << std::endl;
	std::cout << "It can receive commands from external tools through a port" << std::endl;
	std::cout << "Several computers of the 264 series can be emulated" << std::endl;
	std::cout << "/h\t\tJust for help" << std::endl; 
	std::cout << "/o\t\tWhen the emulator wanted to hear instructions from outside" << std::endl; 
	std::cout << "\t\tBy default NOT allowed." << std::endl;
	std::cout << "/p[PORTNUMBER]\tPort where to listen external instructions (0 - 65535)" << std::endl; 
	std::cout << "\t\tBy default 60000. Above 1000 recommended" << std::endl << std::endl;

	// Sets the formatter for << outputs for this emulation...
	// This line of code has to be at the beginning of every emulation 
	// as it fixes the way the internal ino of the important classes will be formatted
	MCHEmul::FormatterBuilder::instance ({ "./C264.fmt" });

	// Set up the emulator...
	C264::C264Emulator myEmulator (
		MCHEmul::CommandLineArguments (argc, argv),
		eOpen 
			? new MCHEmul::CommunicationSystem (
				new MCHEmul::PeerCommunicationChannel (listenPort, 10 /** simultaneous comms. */), 
				new C264::CommandBuilder (new MCHEmul::StandardCommandBuilder)) 
			: nullptr);
	if (!myEmulator || !myEmulator.initialize ())
		return (1); // Not possible to run the emulation. Exit with error...

	return (myEmulator.run () ? 0 /** no error. */ : 1);
}
