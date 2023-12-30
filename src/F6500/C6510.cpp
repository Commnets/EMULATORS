#include <F6500/C6510.hpp>

// ---
F6500::C6510::C6510 (int id)
	: C6500 (id,
		{ { "Code", "6510/6510-1/6510-2" },
		{ "Manufacturer", "MOS Technology INC/Commodore Semiconductor Group (CBM)"},
		{ "Year", "1980" },
		{ "Speed Range", "0.985(PAL) - 1.023(NTSC) MHz" } })
{ 
	// Nothing else
}
