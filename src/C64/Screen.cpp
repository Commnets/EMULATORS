#include <C64/Screen.hpp>

// ---
C64::Screen::Screen (double hz, int x, int y, const MCHEmul::Attributes& attrs)
	: MCHEmul::Screen ("C64", _ID, x, y, 2, hz, attrs)
{ 
	// Nothing else to do
}
