#include <VIC20/Screen.hpp>

// ---
VIC20::Screen::Screen (double hz, int w, int h, const MCHEmul::Attributes& attrs)
	: MCHEmul::Screen ("VIC20", _ID, w, h, 2.0f, 2.0f, hz, attrs),
	  _drawBorder (false), _borderColor (0)
{
	bool e;
	MCHEmul::DataMemoryBlock dt = MCHEmul::DataMemoryBlock::loadBinaryFile 
		("./characters.901460-03-ENG.bin", e, 0 /** no address, no needed */, true);
	if (!e)
	{
		for (size_t i = 0; i < 512; i += 8)
		{
			std::vector <MCHEmul::UBytes> chrdt;
			for (size_t j = 0; j < 8; j++)
				chrdt.push_back (MCHEmul::UBytes ({ dt.byte ((i << 3) + j) }));
			_graphicsDef.emplace_back (std::move (chrdt));
		}
	}

	// Always the same...that is nothing!
	_defaultGraphicDef = 
		std::vector <MCHEmul::UBytes> (8, MCHEmul::UBytes ({ MCHEmul::UByte::_0 }));
}

// ---
void VIC20::Screen::drawAdditional ()
{
	COMMODORE::VICI* gC = static_cast <COMMODORE::VICI*> (_graphicalChip);
	if (_drawBorder)
	{
		// Where is the screen...
		short x1, y1, x2, y2;
		gC -> screenPositions (x1, y1, x2, y2);
		if (x1 == 0 && y1 == 0 && x2 == 0 && y2 == 0)
			return; // The screen could be defined out of the visible zone...

		// Adjust the rectangle to draw to the limits of the scren...
		short xl = x2 - x1 + 1;
		short yl = y2 - y1 + 1;
		if (x1 > 0) { x1--; xl++; }
		if (x2 < (gC -> raster ().visibleColumns () - 1)) { x2++; xl++; }
		if (y1 > 0) { y1--; yl++; }
		if (y2 < (gC -> raster ().visibleLines () - 1)) { y2++; yl++; }
		
		// The color...
		unsigned int bC = ((_borderColor) > 15) ? 0 : _borderColor;

		// ...and finally draws the rectangle!
		drawRectangle ((size_t) x1, (size_t) y1, (size_t) (x1 + xl), (size_t) (y2 + yl), bC);
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
