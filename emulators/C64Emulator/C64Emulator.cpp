#include "stdafx.h"

#include <C64/incs.hpp>

using namespace C64;

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
	// Sets the formatter for << outputs for this emulation...
	// This line of code has to be at the beginning of every emulation 
	// as it fixes the way the internal ino of the important classes will be formatted
	MCHEmul::FormatterBuilder::instance ({ "defformatters.fmt" });

	// Set up the emulator...
	C64Emulator myEmulator (
		MCHEmul::convertIntoStrings (argc, argv),
		new MCHEmul::CommunicationSystem (
			new MCHEmul::PeerCommunicationChannel (60000 /** port. */, 10 /** simultaneous comms. */), new C64::CommandBuilder));
	if (!myEmulator || !myEmulator.initialize ())
		return (1); // Not possible to run the emulation. Exit with error...

	return (myEmulator.run () ? 0 /** no error. */ : 1);
}
