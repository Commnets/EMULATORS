#include <C64/VICII.hpp>
#include <C64/C64.hpp>
#include <F6500/incs.hpp>

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
	auto isBadRasterLine = [=]() -> bool
		{ return (_VICMemory -> currentRasterPosition () >= 0x0030 &&
				  _VICMemory -> currentRasterPosition () <= 0x007f &&
				  (_VICMemory -> currentRasterPosition () & 0x0007) == 9); };

	if (_VICMemory -> vicIItoGenerateIRQ ())
		cpu -> interrupt (F6500::IRQInterrupt::_ID) -> setActive (true);

	if (cpu -> clockCycles  () > _nextRasterCycle)
	{
		if (_VICMemory -> rasterIRQActive () && 
			_VICMemory -> currentRasterPosition () == _VICMemory -> IRQRasterPositionAt ())
		{

			_VICMemory -> setRasterAtPosition (true);

			cpu -> interrupt (F6500::IRQInterrupt::_ID) -> setActive (true);
		}

		if (_VICMemory -> currentRasterPosition () >= _FIRSTVISIBLELINE &&
			_VICMemory -> currentRasterPosition () < _LASTVISIBLELINE)
		{
			// TODO: Draw the border among those two lines...
			// TODO: Draw depenfing on the graphics system active...
			// TODO: Draw the sprites
		}

		_nextRasterCycle = isBadRasterLine () ? _BADLINERASTERCYCLES : _USUALRASTERCYCLES;

		_VICMemory -> setCurrentRasterPosition (_VICMemory -> currentRasterPosition () + 1);
		if (_VICMemory -> currentRasterPosition () >= _SCREENLINES)
		{
			// TODO: Refresh the screen...

			_VICMemory -> setCurrentRasterPosition (0);
		}
	}

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
