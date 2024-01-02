#include <VIC20/Screen.hpp>

// ---
void VIC20::Screen::drawAdditional ()
{
	COMMODORE::VICI* gC = static_cast <COMMODORE::VICI*> (_graphicalChip);
	if (_drawBorder)
	{
		// Draws the border of the screen!
		short x1, y1, x2, y2;
		gC -> screenPositions (x1, y1, x2, y2);
		if (x1 == 0 && y1 == 0 && x2 == 0 && y2 == 0)
			return; // There is no border yet!...
		unsigned int bC = ((_borderColor + 1) > 15) ? 0 : _borderColor + 1; 
		gC -> screenMemory () -> setHorizontalLine ((size_t) x1, (size_t) y1, (size_t) (x2 - x1 + 1), bC);
		gC -> screenMemory () -> setHorizontalLine ((size_t) x1, (size_t) y2, (size_t) (x2 - x1 + 1), bC);
		gC -> screenMemory () -> setVerticalLine   ((size_t) x1, (size_t) y1, (size_t) (y2 - y1 + 1), bC);
		gC -> screenMemory () -> setVerticalLine   ((size_t) x2, (size_t) y1, (size_t) (y2 - y1 + 1), bC);
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
