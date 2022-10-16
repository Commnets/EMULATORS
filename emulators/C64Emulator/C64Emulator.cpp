#include "stdafx.h"

#include <iostream>
#include <locale>
#include <codecvt>

#include <C64/incs.hpp>
#include <CONSOLE/incs.hpp>

using namespace C64;

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

	std::shared_ptr <MCHEmul::FormatterBuilder> fmtBld = MCHEmul::FormatterBuilder::instance ({ "C64formatters.fmt" });
	std::cout << "---- CPU ---" << std::endl << *myEmulator.computer () -> cpu () << std::endl;
	std::cout << "---- STACK -" << std::endl << *myEmulator.computer () -> memory () -> stack () << std::endl;
	std::cout << "---- VICII -" << std::endl << *dynamic_cast <C64::Commodore64*> (myEmulator.computer ()) -> vicII () << std::endl;
	std::cout << "---- CIA1 --" << std::endl << *dynamic_cast <C64::Commodore64*> (myEmulator.computer ()) -> cia1 () << std::endl;
	std::cout << "---- CIA2 --" << std::endl << *dynamic_cast <C64::Commodore64*> (myEmulator.computer ()) -> cia2 () << std::endl;

	// The emulation is done using a console...
	MCHEmul::Win32Console myConsole (&myEmulator, new C64::CommandBuilder);
	myConsole.run ();
	return (myEmulator.lastError ());
}
