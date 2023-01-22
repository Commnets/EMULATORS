#include <C64/Screen.hpp>

// ---
C64::ScreenNTSC::ScreenNTSC ()
	: C64::Screen (59.940f, 
		(int) COMMODORE::VICII_NTSC::_HRASTERDATA.visiblePositions (), 
		(int) COMMODORE::VICII_NTSC::_VRASTERDATA.visiblePositions (),
		{ { "Type", "Screen" },
		  { "Frequency", "59.940Hz" },
		  { "System", "NTSC" } })
{ 
	// Nothing else to do...
}

C64::ScreenPAL::ScreenPAL ()
	: C64::Screen (50.125f,
		(int) COMMODORE::VICII_PAL::_HRASTERDATA.visiblePositions (), 
		(int) COMMODORE::VICII_PAL::_VRASTERDATA.visiblePositions (),
		{ { "Type", "Screen" },
		  { "Frequency", "50.125Hz" },
		  { "System", "PAL" } })
{ 
	// Nothing else to do
}
