#include "stdafx.h"

#include <C64/incs.hpp>
#include <CONSOLE/incs.hpp>

using namespace C64;

int _tmain (int argc, _TCHAR *argv [])
{
	// Sets the formatter for << outputs for this emulation...
	// This line of code has to be at the beginning of every emulation 
	// as it fixes the way the internal ino of the important classes will be formatted
	MCHEmul::FormatterBuilder::instance ({ "C64formatters.fmt" });

	// Set up the emulator...
	C64Emulator myEmulator (
		MCHEmul::convertIntoStrings (argc, argv),
		new MCHEmul::CommunicationSystem (
			new MCHEmul::PeerCommunicationChannel (60000 /** port. */, 10 /** simultaneous comms. */), new C64::CommandBuilder));
	if (!myEmulator || !myEmulator.initialize ())
		return (1); // Not possible to run the emulation. Exit with error...

	// Set up the console that will run the emulation...
	MCHEmul::Win32Console myConsole (&myEmulator, new C64::CommandBuilder);
	myConsole.run ();
	return (myEmulator.lastError ());
}
