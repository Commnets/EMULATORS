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
	// In this version of the ROM, the graphics are in the address 0x1bbf....
	if (!e)
	{
		for (size_t i = 0x1bbf; i < 0x23bf; i += 8) // 256 characters (8 bytes each) = 2048 bytes = 0x0800 bytes
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
		// 16 are available...
		unsigned int bC = (gridColor () > 15) ? 0 : gridColor ();
		unsigned int gC = ((bC + 1) > 15) ? 0 : (bC + 1);

		// Where is the screen...
		unsigned short x1, y1, x2, y2;
		x1 = y1 = x2 = y2 = 0;
		_vdp -> screenPositions (x1, y1, x2, y2);

		// Draws rectangles and reference lines...
		drawRectangle ((size_t) (x1 - 1), (size_t) (y1 - 1), 
			(size_t) (x2 + 1), (size_t) (y2 + 1), 3);
		for (unsigned short i = y1 + 8; i <= y2; i += 8)
			drawHorizontalLineStep ((size_t) x1, (size_t) i, (size_t) (x2 - x1 + 1), 2, gC);
		unsigned short stp = (_vdp -> graphicMode () == 1 /** Text Mode. */) ? 6 : 8;
		unsigned short x2n = (_vdp -> graphicMode () == 1 /** Text Mode. */) ? ((x2 - 8) + 1) : x2;
		for (unsigned short i = x1 + 8; i <= x2n; i += stp)
			drawVerticalLineStep ((size_t) i, (size_t) y1, (size_t) (y2 - y1 + 1), 2, gC);

		// Draws a reference to the owner to the simulator!
		drawTextHorizontal (8, 8, "ICF 2025", 0 /** not used. */, bC, true);
	}
}
