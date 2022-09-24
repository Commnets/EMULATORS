#include <C64/VICII.hpp>
#include <C64/C64.hpp>

// ---
bool C64::VICII::initialize ()
{
	assert (memoryRef () != nullptr);

	bool result = MCHEmul::Chip::initialize ();
	if (!result)
		return (false); // _lastError variable has been already set at this point...

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
bool C64::VICII::simulate (MCHEmul::CPU*)
{
	// TODO

	return (true);
}

// ---
C64::VICII_NTSC::VICII_NTSC ()
	: C64::VICII (
		 { { "Name", "VICII for NTSC Systems" },
		   { "Manufacturer", "Commodore Business Machines CBM" },
		   { "Year", "1980" } })
{
	// Nothing else to do...
}

// ---
C64::VICII_PAL::VICII_PAL ()
	: C64::VICII (
		 { { "Name", "VICII for PAL Systems" },
		   { "Manufacturer", "Commodore Business Machines CBM" },
		   { "Year", "1980" } })
{
	// Nothing else to do...
}
