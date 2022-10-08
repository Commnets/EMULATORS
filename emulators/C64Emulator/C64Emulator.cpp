#include "stdafx.h"

#include <iostream>
#include <locale>
#include <codecvt>

#include <EMULATORS/incs.hpp>
#include <CONSOLE/Console.hpp>

using namespace Emuls;

// To get the parameters from the input line...
MCHEmul::Strings generateParamsFrom (int argc, _TCHAR *argv [])
{
	std::wstring_convert <std::codecvt_utf8 <wchar_t>, wchar_t> converter;
	MCHEmul::Strings result;
	for (int i = 0; i < argc; i++)
		result.push_back (converter.to_bytes (argv[i]));

	return (result);
}

int _tmain (int argc, _TCHAR *argv [])
{
	// Create the communicator using the parameters received!
	C64Emulator myEmulator (generateParamsFrom (argc, argv));
	if (!myEmulator)
		return (1); // No creation possible...
	// The communication system is optional, but we have decided to include it here...
	myEmulator.setCommunicationSystem (new MCHEmul::CommunicationSystem 
		(new MCHEmul::PeerCommunicationChannel (100, 2), new MCHEmul::StandardMessageBuilder));
	// If the emulator can not be initialized, no need to continue..
	if (!myEmulator.initialize ()) 
		return (1); // Exit with an error...

	// The emulation is done using a console...
	Console::Win32Console myConsole (&myEmulator, new C64::CommandBuilder);
	myConsole.run ();
	return (myEmulator.lastError ());
}
