#include "stdafx.h"

#include <C264/incs.hpp>
#include <CONSOLE/incs.hpp>

int _tmain (int argc, _TCHAR *argv [])
{
	// Log to a file...
	MCHEmul::LogSystem::_LOGSYSTEM -> addLogChannel 
		(new MCHEmul::LogFileChannel (0, "./C264E.log", 10000 /** events max. */));
	MCHEmul::LogSystem::_LOGSYSTEM -> setDefaultChannelId (0);

	std::cout << "C264Emulator with Console" << std::endl;
	std::cout << "Copyright (C) 2024 by Ignacio Cea" << std::endl;
	std::cout << "The console controls directly the execution" << std::endl;
	std::cout << "Several computers of the 264 series can be emulated" << std::endl;
	std::cout << "/h\t\tJust for help" << std::endl << std::endl;

	// Sets the formatter for << outputs for this emulation...
	// This line of code has to be at the beginning of every emulation 
	// as it fixes the way the internal ino of the important classes will be formatted
	MCHEmul::FormatterBuilder::instance ({ "./C264.fmt" });

	// Set up the emulator...
	C264::C264Emulator myEmulator (MCHEmul::CommandLineArguments (argc, argv), nullptr /** no commuications from outside. */);
	if (!myEmulator || !myEmulator.initialize ())
		return (1); // Not possible to run the emulation. Exit with error...

	// Set up the console that will run the emulation...
	// The console uses the same builder than the communication system...
	// The keys used are Win32 related.
	MCHEmul::LocalConsole myConsole (&myEmulator, 
		new C264::CommandBuilder (new MCHEmul::StandardCommandBuilder),
		new MCHEmul::Win32ConsoleKeys);
	myConsole.run ();
	return (myEmulator.error ());
}

