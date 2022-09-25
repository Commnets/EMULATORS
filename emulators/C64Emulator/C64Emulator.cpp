#include "stdafx.h"

#include <iostream>
#include <locale>
#include <codecvt>

#include <EMULATORS/incs.hpp>
#include <CONSOLE/Console.hpp>

using namespace Emuls;

// To get the parameters from the input line...
std::vector <std::string> generateParamsFrom (int argc, _TCHAR *argv [])
{
	std::wstring_convert <std::codecvt_utf8 <wchar_t>, wchar_t> converter;
	std::vector <std::string> result;
	for (int i = 0; i < argc; i++)
		result.push_back (converter.to_bytes (argv[i]));

	return (result);
}

int _tmain (int argc, _TCHAR *argv [])
{
	C64Emulator myEmulator (generateParamsFrom (argc, argv));
	if (!myEmulator)
		return (1); // No creation possible...

	myEmulator.setCommunicationSystem (new MCHEmul::CommunicationSystem 
		(new MCHEmul::PeerCommunicationChannel (100, 2), new MCHEmul::StandardMessageBuilder ()));
	if (!myEmulator.initialize ())
		return (1);

	// Emulation thought out a console...
	Console::Win32Console myConsole (&myEmulator, new MCHEmul::StandardCommandBuilder ());
	myConsole.run ();

	return (myEmulator.lastError ());
}
