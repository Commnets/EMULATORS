#include <VIC20/Screen.hpp>

// ---
void VIC20::Screen::drawAdditional ()
{
	COMMODORE::VICI* gC = static_cast <COMMODORE::VICI*> (_graphicalChip);
	if (_drawBorder)
	{
		unsigned short x1, y1, x2, y2;

		// Draws the border of the display!
		gC -> raster ().displayPositions (x1, y1, x2, y2);
		gC -> screenMemory () -> setHorizontalLine ((size_t) x1 - 1, (size_t) y1 - 1, (size_t) x2 - x1 + 3, _borderColor);
		gC -> screenMemory () -> setHorizontalLine ((size_t) x1 - 1, (size_t) y2 + 1, (size_t) x2 - x1 + 3, _borderColor);
		gC -> screenMemory () -> setVerticalLine ((size_t) x1 - 1, (size_t) y1 - 1, (size_t) y2 - y1 + 3, _borderColor);
		gC -> screenMemory () -> setVerticalLine ((size_t) x2 + 1, (size_t) y1 - 1, (size_t) y2 - y1 + 3, _borderColor);
		for (unsigned short i = y1 + 8; i <= y2; i += 8)
			gC -> screenMemory () -> setHorizontalLineStep ((size_t) x1 - 1, (size_t) i, (size_t) x2 - x1 + 3, _borderColor, 2);
		for (unsigned short i = x1 + 8; i <= x2; i += 8)
			gC -> screenMemory () -> setVerticalLineStep ((size_t) i, (size_t) y1 - 1, (size_t) y2 - y1 + 3, _borderColor, 2);

		// Draws the border of the screen!
		gC -> raster ().screenPositions (x1, y1, x2, y2);
		unsigned int bC = ((_borderColor + 1) > 15) ? 0 : _borderColor + 1; 
		gC -> screenMemory () -> setHorizontalLine ((size_t) x1 - 1, (size_t) y1 - 1, (size_t) x2 - x1 + 3, bC);
		gC -> screenMemory () -> setHorizontalLine ((size_t) x1 - 1, (size_t) y2 + 1, (size_t) x2 - x1 + 3, bC);
		gC -> screenMemory () -> setVerticalLine ((size_t) x1 - 1, (size_t) y1 - 1, (size_t) y2 - y1 + 3, bC);
		gC -> screenMemory () -> setVerticalLine ((size_t) x2 + 1, (size_t) y1 - 1, (size_t) y2 - y1 + 3, bC);
	}
}

// ---
VIC20::ScreenNTSC::ScreenNTSC ()
	: VIC20::Screen (60.03f, 
		(int) COMMODORE::VICI_NTSC::_HRASTERDATA.visiblePositions (), 
		(int) COMMODORE::VICI_NTSC::_VRASTERDATA.visiblePositions (),
		{ { "Name", "Screen NTSC" },
		  { "Type", "Output" },
		  { "Frequency", "60.03Hz" } })
{ 
	// Nothing else to do...
}

VIC20::ScreenPAL::ScreenPAL ()
	: VIC20::Screen (50.04f,
		(int) COMMODORE::VICI_PAL::_HRASTERDATA.visiblePositions (), 
		(int) COMMODORE::VICI_PAL::_VRASTERDATA.visiblePositions (),
		{ { "Name", "Screen PAL" },
		  { "Type", "Output" },
		  { "Frequency", "50.04Hz" } })
{ 
	// Nothing else to do
}
