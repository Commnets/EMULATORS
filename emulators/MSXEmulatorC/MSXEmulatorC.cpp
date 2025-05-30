#include "stdafx.h"

#include <MSX/incs.hpp>
#include <CONSOLE/incs.hpp>

int _tmain (int argc, _TCHAR *argv [])
{
	// Log to a file...
	MCHEmul::LogSystem::_LOGSYSTEM -> addLogChannel 
		(new MCHEmul::LogFileChannel (0, "./MSXE.log", 10000 /** events max. */));
	MCHEmul::LogSystem::_LOGSYSTEM -> setDefaultChannelId (0);

	std::cout << "MSXEmulator with Console" << std::endl;
	std::cout << "Copyright (C) 2025 by Ignacio Cea" << std::endl;
	std::cout << "The console controls directly the execution" << std::endl;
	std::cout << "/h\t\tJust for help" << std::endl << std::endl;

	// Sets the formatter for << outputs for this emulation...
	// This line of code has to be at the beginning of every emulation 
	// as it fixes the way the internal ino of the important classes will be formatted
	MCHEmul::FormatterBuilder::instance ({ "./MSX.fmt" });

	// Set up the emulator...
	MSX::MSXEmulator myEmulator (MCHEmul::CommandLineArguments (argc, argv), nullptr /** no commuications from outside. */);
	if (!myEmulator || !myEmulator.initialize ())
		return (1); // Not possible to run the emulation. Exit with error...

	// Set up the console that will run the emulation...
	// The console uses the same builder than the communication system...
	// The keys used are Win32 related.
	MCHEmul::LocalConsole myConsole (&myEmulator, 
		new MSX::CommandBuilder (new MCHEmul::StandardCommandBuilder),
		new MCHEmul::Win32ConsoleKeys);
	myConsole.run ();
	return (myEmulator.error ());
}

