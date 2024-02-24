#include <C64/Screen.hpp>

C64::Screen::Screen (double hz, int w, int h, const MCHEmul::Attributes& attrs)
	: MCHEmul::Screen ("C64", _ID, w, h, 2.0f, 2.0f, hz, attrs),
		_drawBorder (false), _borderColor (0)
{
	bool e;
	MCHEmul::DataMemoryBlock dt = MCHEmul::DataMemoryBlock::loadBinaryFile
		("./characters.901225-01-ENG.bin", e, 0 /** no address, no needed */, true);
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
void C64::Screen::drawAdditional ()
{
	COMMODORE::VICII* gC = static_cast <COMMODORE::VICII*> (_graphicalChip);
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
