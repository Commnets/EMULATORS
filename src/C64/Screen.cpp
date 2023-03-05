#include <C64/Screen.hpp>

// ---
void C64::Screen::drawAdditional ()
{
	COMMODORE::VICII* gC = static_cast <COMMODORE::VICII*> (_graphicalChip);
	if (_drawBorder)
	{
		unsigned short x1, y1, x2, y2;
		gC -> raster ().screenPositions (x1, y1, x2, y2);
		gC -> screenMemory () -> setHorizontalLine ((size_t) x1 - 1, (size_t) y1 - 1, (size_t) x2 - x1 + 3, _borderColor);
		gC -> screenMemory () -> setHorizontalLine ((size_t) x1 - 1, (size_t) y2 + 1, (size_t) x2 - x1 + 3, _borderColor);
		gC -> screenMemory () -> setVerticalLine ((size_t) x1 - 1, (size_t) y1 - 1, (size_t) y2 - y1 + 3, _borderColor);
		gC -> screenMemory () -> setVerticalLine ((size_t) x2 + 1, (size_t) y1 - 1, (size_t) y2 - y1 + 3, _borderColor);
	}
}

// ---
C64::ScreenNTSC::ScreenNTSC ()
	: C64::Screen (59.940f, 
		(int) COMMODORE::VICII_NTSC::_HRASTERDATA.visiblePositions (), 
		(int) COMMODORE::VICII_NTSC::_VRASTERDATA.visiblePositions (),
		{ { "Name", "Screen NTSC" },
		  { "Type", "Output" },
		  { "Frequency", "59.940Hz" } })
{ 
	// Nothing else to do...
}

C64::ScreenPAL::ScreenPAL ()
	: C64::Screen (50.125f,
		(int) COMMODORE::VICII_PAL::_HRASTERDATA.visiblePositions (), 
		(int) COMMODORE::VICII_PAL::_VRASTERDATA.visiblePositions (),
		{ { "Name", "Screen PAL" },
		  { "Type", "Output" },
		  { "Frequency", "50.125Hz" } })
{ 
	// Nothing else to do
}
