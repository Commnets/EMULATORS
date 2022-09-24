#include <F6500/C6510.hpp>

// ---
F6500::C6510::C6510 ()
	: F6500::C6500 (F6500::C6510::createArchitecture ())
{
	// Nothing else to do...
}

// ---
MCHEmul::CPUArchitecture F6500::C6510::createArchitecture ()
{
	return (
		MCHEmul::CPUArchitecture 
			(2 /** 2 bytes = 16 bites */, 
			 1 /** bytes per instruction */, 
			 false /** Little endian. */, 
			 { { "Code", "6510/6150-1/6510-2" },
			   { "Manufacturer", "MOS Technology INC/Commodore Semiconductor Group (CBM)"},
			   { "Year", "1980" },
			   { "Speed", "0.985(PAL)/1.023(NTSC) MHz" } }));
}
