#include <F6500/C6502.hpp>

// ---
F6500::C6502::C6502 (int id)
	: F6500::C6500 (id,
		{ { "Code", "6502" },
		{ "Manufacturer", "MOS Technology INC/Commodore Semiconductor Group (CBM)"},
		{ "Year", "1975" },
		{ "Speed Range", "1 - 3 MHz" } })
{ 
	// Nothing else
}
