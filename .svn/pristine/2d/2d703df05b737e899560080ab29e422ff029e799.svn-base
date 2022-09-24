#include <C64/VICII.hpp>
#include <C64/C64.hpp>

// ---
bool C64::VICII::initialize ()
{
	assert (memoryRef () != nullptr);

	if (!MCHEmul::Chip::initialize ())
		return (false);

	// Gets the memory block dedicated to the VICII
	if (!(_VICMemory = 
		dynamic_cast <C64::VICMemory*> (memoryRef () -> block (C64::Commodore64::_VICREGS_MEMORY))))
	{
		_lastError = MCHEmul::_INIT_ERROR;

		return (false);
	}

	return (true);
}

// ---
bool C64::VICII::simulate (MCHEmul::CPU* cpu)
{
	// TODO

	return (true);
}

// ---
C64::VICII_NTSC::VICII_NTSC ()
	: C64::VICII (
		 { { "Name", "VIC-II (NTSC) Video Chip Interface II" },
		   { "Code", "6567/8562/8564" },
		   { "Manufacturer", "MOS Technology INC/Commodore Semiconductor Group (CBM)"},
		   { "Year", "1980" } })
{
	// Nothing else to do...
}

// ---
C64::VICII_PAL::VICII_PAL ()
	: C64::VICII (
		 { { "Name", "VIC-II (PAL) Video Chip Interface II" },
		   { "Code", "6569/8565/8566" },
		   { "Manufacturer", "MOS Technology INC/Commodore Semiconductor Group (CBM)"},
		   { "Year", "1980" } })
{
	// Nothing else to do...
}
