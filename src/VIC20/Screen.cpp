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
		for (size_t i = 0; i < 4096; i += 8)
		{
			std::vector <MCHEmul::UBytes> chrdt;
			for (size_t j = 0; j < 8; j++)
				chrdt.push_back (MCHEmul::UBytes ({ dt.byte (i + j) }));
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
	// The color...
	unsigned int bC = ((_borderColor + 1) > 15) ? 0 : _borderColor + 1;

	if (_drawBorder)
	{
		// Where is the screen...
		COMMODORE::VICI* gC = static_cast <COMMODORE::VICI*> (_graphicalChip);
		short x1, y1, x2, y2;
		gC -> screenPositions (x1, y1, x2, y2);
		if (x1 == 0 && y1 == 0 && x2 == 0 && y2 == 0)
			return; // The screen could be defined out of the visible zone...

		// Adjust the rectangle to draw to the limits of the scren...
		if (x1 > 0) x1--;
		if (x2 < (gC -> raster ().visibleColumns () - 1)) x2++;
		if (y1 > 0) y1--;
		if (y2 < (gC -> raster ().visibleLines () - 1)) y2++;
		// ...and finally draws the rectangle!
		drawRectangle ((size_t) x1, (size_t) y1, (size_t) x2, (size_t) y2, bC);

		// The reference line can not be drwa from the previous rectangle
		// as the drawable zone could be out of the visible window and not be real boxes...
		short x1p, y1p, x2p, y2p;
		gC -> originalScreenPositions (x1p, y1p, x2p, y2p);
		for (short i = y1p + 8; i <= y2p; i += 8)
			if (i >= 0 && i < gC -> raster ().visibleLines ())
				drawHorizontalLineStep ((size_t) x1, (size_t) i, (size_t) (x2 - x1 + 1), 2, bC);
		for (short i = x1p + 8; i <= x2p; i += 8)
			if (i >= 0 && i < gC -> raster ().visibleColumns ())
				drawVerticalLineStep ((size_t) i, (size_t) y1, (size_t) (y2 - y1 + 1), 2, bC);

		// Draws a reference to the owner to the simulator!
		drawTextHorizontal (8, 8, "(C)ICF 2024", 0 /** not used. */, bC, true);
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
