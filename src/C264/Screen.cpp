#include <C264/Screen.hpp>

// ---
C264::Screen::Screen (const std::string& tt, double hz, int w, int h, const MCHEmul::Attributes& attrs)
	: MCHEmul::Screen (tt, _ID, w, h, 2.0f, 2.0f, hz, attrs)
{ 
	bool e;
	MCHEmul::DataMemoryBlock dt = MCHEmul::DataMemoryBlock::loadBinaryFile 
		("./kernal.318004-05-ENG.bin", e, 0 /** no address needed */, true);
	// The file is longer than 4k, 
	// but the chars definition are from the posicion 0x1000 of the kernel file!
	if (!e)
	{
		for (size_t i = 0; i < 4096; i += 8)
		{
			std::vector <MCHEmul::UBytes> chrdt;
			for (size_t j = 0; j < 8; j++)
				chrdt.push_back (MCHEmul::UBytes ({ dt.byte (0x1000 + i + j) }));
			_graphicsDef.emplace_back (std::move (chrdt));
		}
	}

	// Always the same...that is nothing!
	_defaultGraphicDef = 
		std::vector <MCHEmul::UBytes> (8, MCHEmul::UBytes ({ MCHEmul::UByte::_0 }));
}

// ---
void C264::Screen::drawAdditional ()
{
	if (gridColor ())
	{
		// The color...
		// Only 128 are available...
		unsigned int clr = (gridColor () > 128) ? 0 : gridColor ();
		unsigned int bC = ((clr + 1) > 128) ? 0 : clr + 1; 

		COMMODORE::TED* gC = static_cast <COMMODORE::TED*> (_graphicalChip);

		unsigned short x1, y1, x2, y2;
		// Draws the border of the display!
		gC -> raster ().displayPositions (x1, y1, x2, y2);
		// as C64 behaves, these values can never be wrong neither in order nor in value...
		drawRectangle ((size_t) (x1 - 1), (size_t) (y1 - 1), (size_t) (x2 + 1), (size_t) (y2 + 1), clr);

		// Draw a cuadricula per line of chars...
		for (unsigned short i = y1 + 8; i <= y2; i += 8)
			drawHorizontalLineStep ((size_t) (x1 - 1), (size_t) i, (size_t) (x2 - x1 + 3), 2, clr);
		for (unsigned short i = x1 + 8; i <= x2; i += 8)
			drawVerticalLineStep ((size_t) i, (size_t) (y1 - 1), (size_t) (y2 - y1 + 3), 2, clr);

		// Draws the border of the screen!
		gC -> raster ().screenPositions (x1, y1, x2, y2);
		// as C64 behaves, these values can never be wrong neither in order nor in value...
		drawRectangle ((size_t) (x1 - 1), (size_t) (y1 - 1), (size_t) (x2 + 1), (size_t) (y2 + 1), bC);

		// Draws a reference to the owner to the simulator!
		drawTextHorizontal (8, 8, "(C)ICF 2024", 0 /** not used. */, bC, true);
	}
}

// ---
C264::ScreenNTSC::ScreenNTSC (const std::string& tt)
	: C264::Screen (tt, 60.03f, 
		(int) COMMODORE::TED_NTSC::_HRASTERDATA.visiblePositions (), 
		(int) COMMODORE::TED_NTSC::_VRASTERDATA.visiblePositions (),
		{ { "Name", "Screen NTSC" },
		  { "Type", "Output" },
		  { "Frequency", "60.0Hz" } })
{ 
	// Nothing else to do...
}

// ---
C264::ScreenPAL::ScreenPAL (const std::string& tt)
	: C264::Screen (tt, 50.04f,
		(int) COMMODORE::TED_PAL::_HRASTERDATA.visiblePositions (), 
		(int) COMMODORE::TED_PAL::_VRASTERDATA.visiblePositions (),
		{ { "Name", "Screen PAL" },
		  { "Type", "Output" },
		  { "Frequency", "60.0Hz" } })
{ 
	// Nothing else to do...
}
