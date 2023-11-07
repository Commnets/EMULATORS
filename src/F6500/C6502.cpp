#include <F6500/C6502.hpp>

// ---
MCHEmul::CPUArchitecture F6500::C6502::createArchitecture ()
{
	return (
		MCHEmul::CPUArchitecture 
			(2 /** 2 bytes = 16 bites */, 
			 1 /** bytes per instruction */, 
			 false /** Little endian. */, 
			 { { "Code", "6502" },
			   { "Manufacturer", "MOS Technology INC/Commodore Semiconductor Group (CBM)"},
			   { "Year", "1975" },
			   { "Speed Range", "1 - 3 MHz" } }));
}
