#include <MSX/Screen.hpp>
#include <MSX/VDP.hpp>

// ---
MSX::Screen::Screen (double hz, int w, int h, MSX::VDP* vdp, 
		const MCHEmul::Attributes& attrs)
	: MCHEmul::Screen ("MSX", _ID, w, h, 2.0f, 2.0f, hz, attrs),
	  _vdp (vdp)
{
	assert (_vdp != nullptr);

	bool e;
	MCHEmul::DataMemoryBlock dt = MCHEmul::DataMemoryBlock::loadBinaryFile 
		("./bios/svi728_basic-bios_ENG.rom", e, 0 /** no address needed */, true);
	if (!e)
	{
		for (size_t i = 0x3d00; i < 0x4000; i += 8) // 96 characters (8 bytes each) = 768 bytes = 0x0300 bytes
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
void MSX::Screen::drawAdditional ()
{
	if (drawGrid ())
	{
		// The color...
		// Only 4 are available...
		unsigned int bC = (gridColor () > 15) ? 0 : gridColor ();

		// Where is the screen...
		unsigned short x1, y1, x2, y2;
		x1 = y1 = x2 = y2 = 0;
		_vdp -> screenPositions (x1, y1, x2, y2);

		// Draws rectangles and reference lines...
		drawRectangle ((size_t) (x1 - 1), (size_t) (y1 - 1), 
			(size_t) (x2 + 1), (size_t) (y2 + 1), 3);
		for (short i = y1 + 8; i <= y2; i += 8)
			drawHorizontalLineStep ((size_t) x1, (size_t) i, (size_t) (x2 - x1 + 1), 2, bC);
		for (short i = x1 + 8; i <= x2; i += 8)
			drawVerticalLineStep ((size_t) i, (size_t) y1, (size_t) (y2 - y1 + 1), 2, bC);

		// Draws a reference to the owner to the simulator!
		drawTextHorizontal (8, 8, "ICF 2025", 0 /** not used. */, bC, true);
	}
}
