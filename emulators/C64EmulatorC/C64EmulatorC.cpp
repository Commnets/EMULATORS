#include "stdafx.h"

#include <C64/incs.hpp>
#include <CONSOLE/incs.hpp>

int _tmain (int argc, _TCHAR *argv [])
{
	// Sets the formatter for << outputs for this emulation...
	// This line of code has to be at the beginning of every emulation 
	// as it fixes the way the internal ino of the important classes will be formatted
	MCHEmul::FormatterBuilder::instance ({ "./C64formatters.fmt" });

	// Set up the emulator...
	C64::C64Emulator myEmulator (MCHEmul::convertIntoStrings (argc, argv), nullptr /** no commuications from outside. */);
	if (!myEmulator || !myEmulator.initialize ())
		return (1); // Not possible to run the emulation. Exit with error...

	// Set up the console that will run the emulation...
	// The console uses the same builder than the communication system...
	// The keys used are Win32 related.
	MCHEmul::LocalConsole myConsole (&myEmulator, new C64::CommandBuilder, new MCHEmul::Win32ConsoleKeys);
	myConsole.run ();
	return (myEmulator.lastError ());
}
