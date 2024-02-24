#include <C264/Screen.hpp>

// ---
C264::Screen::Screen (const std::string& tt, double hz, int w, int h, const MCHEmul::Attributes& attrs)
	: MCHEmul::Screen (tt, _ID, w, h, 2.0f, 2.0f, hz, attrs),
	  _drawBorder (false), _borderColor (0)
{ 
	bool e;
	MCHEmul::DataMemoryBlock dt = MCHEmul::DataMemoryBlock::loadBinaryFile 
		("././kernal.318005-05-FRA.bin", e, 0 /** no address, no needed */, true);
	// The file is longer than 4k, but the chars definition are at the beginning of the kernel file!
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
void C264::Screen::drawAdditional ()
{
	COMMODORE::TED* gC = static_cast <COMMODORE::TED*> (_graphicalChip);
	if (_drawBorder)
	{
		unsigned short x1, y1, x2, y2;

		// Draws the border of the display!
		gC -> raster ().displayPositions (x1, y1, x2, y2);
		// as C64 behaves, these values can never be wrong neither in order nor in value...
		drawRectangle ((size_t) (x1 - 1), (size_t) (y1 - 1), (size_t) (x2 + 1), (size_t) (y2 + 1), _borderColor);

		// Draw a cuadricula per line of chars...
		for (unsigned short i = y1 + 8; i <= y2; i += 8)
			drawHorizontalLineStep ((size_t) x1 - 1, (size_t) i, (size_t) x2 - x1 + 3, 2, _borderColor);
		for (unsigned short i = x1 + 8; i <= x2; i += 8)
			drawVerticalLineStep ((size_t) i, (size_t) y1 - 1, (size_t) y2 - y1 + 3, 2, _borderColor);

		// Draws the border of the screen!
		gC -> raster ().screenPositions (x1, y1, x2, y2);
		// as C64 behaves, these values can never be wrong neither in order nor in value...
		unsigned int bC = ((_borderColor + 1) > 15) ? 0 : _borderColor + 1; 
		drawRectangle ((size_t) (x1 - 1), (size_t) (y1 - 1), (size_t) (x2 + 1), (size_t) (y2 + 1), _borderColor);
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
		(int) COMMODORE::TED_NTSC::_HRASTERDATA.visiblePositions (), 
		(int) COMMODORE::TED_NTSC::_VRASTERDATA.visiblePositions (),
		{ { "Name", "Screen PAL" },
		  { "Type", "Output" },
		  { "Frequency", "60.0Hz" } })
{ 
	// Nothing else to do...
}
