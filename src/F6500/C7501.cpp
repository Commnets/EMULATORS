#include <F6500/C7501.hpp>
#include <F6500/NMIInterrupt.hpp>

// ---
F6500::C7501::C7501 (int id)
	: C6510 (id,
		{ { "Code", "7501/8501" },
		{ "Manufacturer", "MOS Technology INC/Commodore Semiconductor Group (CBM)"},
		{ "Year", "1984" },
		{ "Speed Range", "0.985(PAL) - 1.023(NTSC) MHz" } })
{ 
	// Nothing else to do...
}
