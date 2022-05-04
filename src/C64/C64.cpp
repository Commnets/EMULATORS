#include <C64/C64.hpp>
#include <F6500/C6510.hpp>
#include <core/Stack.hpp>

// ---
C64::Commodore64::Commodore64 ()
	: MCHEmul::Computer 
		(new F6500::C6510 (),
		 C64::Commodore64::standardChips (),
		 C64::Commodore64::standardMemory (),
		 { { "Name", "Commodore 64" },
		   { "Manufacturer", "Commodore Business Machines CBM" },
		   { "Year", "1980" }
		 })
{
	bool result = true;
	result &= memory () -> block (_BASIC_MEMORY) -> loadInto ("./basic.901226-01.bin");
	result &= memory () -> block (_IO_MEMORY) -> loadInto ("./characters.901225-01.bin");
	result &= memory () -> block (_ROM_MEMORY) -> loadInto ("./kernal.901227-03.bin");

	// After loading the info, it is taken as the default...
	memory () -> fixDefaultValues ();

	if (!result)
		_lastError = MCHEmul::_INIT_ERROR;
}

// ---
MCHEmul::Chips C64::Commodore64::standardChips ()
{
	MCHEmul::Chips result;

	// TODO

	return (result);
}

// ---
MCHEmul::Memory* C64::Commodore64::standardMemory ()
{
	/** All dirs in Little-endian format. All RAM except the last. */
	MCHEmul::Memory* DDR			= new MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0x00 }, { 0xff, 0xff }, false), 0x0001);
	MCHEmul::Memory* IOP			= new MCHEmul::Memory (MCHEmul::Address ({ 0x01, 0x00 }, { 0xff, 0xff }, false), 0x0001);
	MCHEmul::Memory* PageZero		= new MCHEmul::Memory (MCHEmul::Address ({ 0x02, 0x00 }, { 0xff, 0xff }, false), 0x00fd);
	MCHEmul::Stack*  Stack			= new MCHEmul::Stack  (MCHEmul::Address ({ 0x00, 0x01 }, { 0xff, 0xff }, false), 0x0100);
	MCHEmul::Memory* RAMMemory0		= new MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0x02 }, { 0xff, 0xff }, false), 0x9e00); // 40k
	MCHEmul::Memory* Basic			= new MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0xa0 }, { 0xff, 0xff }, false), 0x2000);
	MCHEmul::Memory* RAMMemory1		= new MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0xc0 }, { 0xff, 0xff }, false), 0x1000); // 4k
	MCHEmul::Memory* InputOutput	= new MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0xd0 }, { 0xff, 0xff }, false), 0x1000);
	MCHEmul::Memory* ROM			= new MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0xe0 }, { 0xff, 0xff }, false), 0x2000, true); // ROM

	MCHEmul::Memory* result = 
		new MCHEmul::Memory (MCHEmul::Address ({ 0x00, 0x00 }, { 0xff, 0xff }, false), 0 /** The length is in the blocks */, false,
			{ 
				{ _DDR_MEMORY		, DDR },
				{ _IOP_MEMORY		, IOP },
				{ _PAGEZERO_MEMORY	, PageZero },
				{ _STACK_MEMORY		, Stack },
				{ _RAM0_MEMORY		, RAMMemory0 },
				{ _BASIC_MEMORY		, Basic },
				{ _RAM1_MEMORY		, RAMMemory1 },
				{ _IO_MEMORY		, InputOutput },
				{ _ROM_MEMORY		, ROM }		
			}, Stack);

	return (result);
}
