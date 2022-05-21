#include <C64/C64.hpp>
#include <C64/Memory.hpp>
#include <C64/VICII.hpp>
#include <F6500/C6510.hpp>
#include <core/Stack.hpp>

// ---
C64::Commodore64::Commodore64 (C64::Commodore64::VisualSystem vS)
	: MCHEmul::Computer 
		(new F6500::C6510 (),
		 C64::Commodore64::standardChips (vS),
		 C64::Commodore64::standardMemory (),
		 { { "Name", "Commodore 64" },
		   { "Manufacturer", "Commodore Business Machines CBM" },
		   { "Year", "1980" }
		 })
{
	bool result = true;
	result &= memory () -> block (_BASIC_MEMORY) -> loadInto ("./basic.901226-01.bin");
	result &= memory () -> block (_CHAROM_MEMORY) -> loadInto ("./characters.901225-01.bin");
	result &= memory () -> block (_KERNELROM_MEMORY) -> loadInto ("./kernal.901227-03.bin");

	// After loading the info, it is taken as the default...
	memory () -> fixDefaultValues ();

	if (!result)
		_lastError = MCHEmul::_INIT_ERROR;
}

// ---
bool C64::Commodore64::initialize ()
{
	bool result = MCHEmul::Computer::initialize ();
	if (!result)
		return (false); // _lastError variable has already been set at this point...

	// TODO

	return (true);
}

// ---
MCHEmul::Chips C64::Commodore64::standardChips (C64::Commodore64::VisualSystem vS)
{
	MCHEmul::Chips result;

	// The vicII created will depend on whether the visualization is PAL or NTSC...
	result.insert (MCHEmul::Chips::value_type (VICII::_ID, 
		(MCHEmul::Chip*) ((vS == C64::Commodore64::VisualSystem::_NTSC) 
			? (C64::VICII*) new C64::VICII_NTSC : (C64::VICII*) new C64::VICII_PAL)));

	// TODO

	return (result);
}

// ---
MCHEmul::Memory* C64::Commodore64::standardMemory ()
{
	/** All dirs in Little - endian format. */

	MCHEmul::Memory* DDR			= new MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0x00 }, false), 0x0001);
	MCHEmul::Memory* IOP			= new MCHEmul::Memory (MCHEmul::Address ({ 0x01, 0x00 }, false), 0x0001);
	MCHEmul::Memory* PageZero		= new MCHEmul::Memory (MCHEmul::Address ({ 0x02, 0x00 }, false), 0x00fd);
	MCHEmul::Stack*  Stack			= new MCHEmul::Stack  (MCHEmul::Address ({ 0x00, 0x01 }, false), 0x0100);
	MCHEmul::Memory* RAMMemory0		= new MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0x02 }, false), 0x9e00);		// 40k
	MCHEmul::Memory* BasicRAM		= new MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0xa0 }, false), 0x2000);		// When the BASIC is not used...
	MCHEmul::Memory* BasicROM		= new MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0xa0 }, false), 0x2000, true); // Where the basic is located 
	MCHEmul::Memory* RAMMemory1		= new MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x1000);		// 4k
	MCHEmul::Memory* VICRegisters	= new C64::VICMemory; 
	MCHEmul::Memory* SIDRegisters	= new MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0xd4 }, false), 0x0400); 
	MCHEmul::Memory* ColorRAM		= new C64::ColorRAMMemory;	
	MCHEmul::Memory* CIA1			= new MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0xdc }, false), 0x0100); 
	MCHEmul::Memory* CIA2			= new MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0xdd }, false), 0x0100); 
	MCHEmul::Memory* IO1			= new MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0xde }, false), 0x0100); 
	MCHEmul::Memory* IO2			= new MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0xdf }, false), 0x0100); 
	MCHEmul::Memory* CHARROM		= new MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0xd0 }, false), 0x1000, true); // ROM
	MCHEmul::Memory* KernalROM		= new MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0xe0 }, false), 0x2000, true); // ROM

	MCHEmul::Memory* result = 
		new MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0x00 }, false), 0 /** The length is in the blocks */, false,
			{ 
				{ _DDR_MEMORY		, DDR },
				{ _IOP_MEMORY		, IOP },
				{ _PAGEZERO_MEMORY	, PageZero },
				{ _STACK_MEMORY		, Stack },
				{ _RAM0_MEMORY		, RAMMemory0 },
				{ _BASIC_MEMORY		, BasicROM },
				{ _BASIC_MEMORY		, BasicRAM },
				{ _RAM1_MEMORY		, RAMMemory1 },
				{ _VICREGS_MEMORY	, VICRegisters },
				{ _SIDREGS_MEMORY	, SIDRegisters },
				{ _COLOR_MEMORY		, ColorRAM },
				{ _CIA1_MEMORY		, CIA1 },
				{ _CIA2_MEMORY		, CIA2 },
				{ _IO1_MEMORY		, IO1 },
				{ _IO2_MEMORY		, IO2 },
				{ _CHAROM_MEMORY	, CHARROM },
				{ _KERNELROM_MEMORY	, KernalROM }		
			}, Stack);

	return (result);
}
