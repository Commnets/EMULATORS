#include <C64/Memory.hpp>
#include <C64/ColorMemory.hpp>
#include <C64/VICIIRegisters.hpp>
#include <C64/CIARegisters.hpp>

// ---
C64::Memory::Memory ()
	: MCHEmul::Memory (C64::Memory::standardMemoryContent ())
{
	// In the content...
	if (lastError () != MCHEmul::_NOERROR)
		return;

	bool ok = true;
	ok &= subset (_BASICROM_SUBSET) -> loadInto ("./basic.901226-01.bin");
	subset (_BASICROM_SUBSET) -> fixDefaultValues (); // Fix the values for further initializations...
	ok &= subset (_CHARROM_SUBSET) -> loadInto ("./characters.901225-01.bin");
	subset (_CHARROM_SUBSET) -> fixDefaultValues ();
	ok &= subset (_KERNELROM_SUBSET) -> loadInto ("./kernal.901227-03.bin");
	subset (_KERNELROM_SUBSET) -> fixDefaultValues ();

	if (!ok)
		_lastError = MCHEmul::_INIT_ERROR;
}

// ---
bool C64::Memory::initialize ()
{
	bool result = MCHEmul::Memory::initialize ();
	if (!result)
		return (false);

	// The active view has to be initially the CPU vire...
	setCPUView ();

	// Very basic values as the memory starts...
	// The address 0 controls Chip I/O Data Direction Register: D6510
	subset (_PAGEZERO_SUBSET)	-> set (MCHEmul::Address ({ 0x00, 0x00 }, false), MCHEmul::UByte (0x2f));
	// Not all Subsets are active for reading activities...
	// The address 1 keeps that info, about how the 6510 reads the different zones of the memory...
	subset (_PAGEZERO_SUBSET)	-> set (MCHEmul::Address ({ 0x01, 0x00 }, false), MCHEmul::UByte (0x07));

	return (true);
}

// ---
MCHEmul::Memory::Content C64::Memory::standardMemoryContent ()
{
	/** All dirs in Little - endian format. */

	// Phisical storages
	MCHEmul::PhisicalStorage* RAM = 
		new MCHEmul::PhisicalStorage (_RAM, MCHEmul::PhisicalStorage::Type::_RAM, 0x10000);				// 64k
	MCHEmul::PhisicalStorage* BASICROM = 
		new MCHEmul::PhisicalStorage (_BASICROM, MCHEmul::PhisicalStorage::Type::_ROM, 0x2000);			// 8k
	MCHEmul::PhisicalStorage* CHARROM = 
		new MCHEmul::PhisicalStorage (_CHARROM, MCHEmul::PhisicalStorage::Type::_ROM, 0x1000);			// 4k
	MCHEmul::PhisicalStorage* KERNELROM	= 
		new MCHEmul::PhisicalStorage (_KERNELROM, MCHEmul::PhisicalStorage::Type::_ROM, 0x2000);		// 8k

	// The map of phisical storages, used later...
	MCHEmul::PhisicalStorages storages (
		{
			{ _RAM, RAM },
			{ _BASICROM, BASICROM },
			{ _CHARROM, CHARROM },
			{ _KERNELROM, KERNELROM }
		});

	// Subsets
	// ...Over the RAM and ROM as the CPU sees it
	// Page 0
	MCHEmul::PhisicalStorageSubset* PageZero = new MCHEmul::PhisicalStorageSubset 
		(_PAGEZERO_SUBSET, RAM, 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 0x0100);
	// Stack
	MCHEmul::Stack*  Stack = new MCHEmul::Stack 
		(_STACK_SUBSET, RAM, 0x0100, MCHEmul::Address ({ 0x01, 0x01 }, false), 0x0100);
	// Pure RAM. A piece used by BASIC (40k)
	MCHEmul::PhisicalStorageSubset* RAM0 = new MCHEmul::PhisicalStorageSubset 
		(_RAM0_SUBSET, RAM, 0x0200, MCHEmul::Address ({ 0x00, 0x02 }, false), 0x9e00); 					// 40k pure (a bit used by BASIC)
	// Where the basic is can be either ROM o RAM (depends on bits in position 1 of the page 0)	
	MCHEmul::PhisicalStorageSubset* BasicROM = new MCHEmul::PhisicalStorageSubset
		(_BASICROM_SUBSET, BASICROM, 0x0000, MCHEmul::Address ({ 0x00, 0xa0 }, false), 0x2000);	
	MCHEmul::PhisicalStorageSubset* BasicRAM = new MCHEmul::PhisicalStorageSubset
		(_BASICRAM_SUBSET, RAM, 0x0a000, MCHEmul::Address ({ 0x00, 0xa0 }, false), 0x2000);				// 8k (over BasicROM)
	// 	Pure RAM (4k)
	MCHEmul::PhisicalStorageSubset* RAM1 = new MCHEmul::PhisicalStorageSubset
		(_RAM1_SUBSET, RAM, 0x0c000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x1000);					// 4k
	// Where the CharROM is defined we have also the access to the chips (VIC, SID,...)
	MCHEmul::PhisicalStorageSubset* CharROM = new MCHEmul::PhisicalStorageSubset
		(_CHARROM_SUBSET, CHARROM, 0x0000, MCHEmul::Address ({ 0x00, 0xd0 }, false), 0x1000);
	MCHEmul::PhisicalStorageSubset* VICIIRegisters = new C64::VICIIRegisters (_VICREGS_SUBSET, RAM);
	MCHEmul::PhisicalStorageSubset* SIDRegisters = new MCHEmul::PhisicalStorageSubset
		(_SIDREGS_SUBSET, RAM, 0xd400, MCHEmul::Address ({ 0x00, 0xd4 }, false), 0x0400);
	MCHEmul::PhisicalStorageSubset* ColorRAM = new C64::ColorRAMMemory (_COLOR_SUBSET, RAM);
	MCHEmul::PhisicalStorageSubset* CIA1 = new C64::CIA1Registers (_CIA1_SUBSET, RAM);
	MCHEmul::PhisicalStorageSubset* CIA2 = new C64::CIA2Registers (_CIA2_SUBSET, RAM);
	MCHEmul::PhisicalStorageSubset* IO1 = new MCHEmul::PhisicalStorageSubset 
		(_IO1_SUBSET, RAM, 0xde00, MCHEmul::Address ({ 0x00, 0xde }, false), 0x0100); 
	MCHEmul::PhisicalStorageSubset* IO2 = new MCHEmul::PhisicalStorageSubset 
		(_IO2_SUBSET, RAM, 0xdf00, MCHEmul::Address ({ 0x00, 0xdf }, false), 0x0100); 
	// Where the kernel is defined can be eiher RAM or ROM (
	MCHEmul::PhisicalStorageSubset* KernelROM = new MCHEmul::PhisicalStorageSubset 
		(_KERNELROM_SUBSET, KERNELROM, 0x0000, MCHEmul::Address ({ 0x00, 0xe0 }, false), 0x2000);
	MCHEmul::PhisicalStorageSubset* KernelRAM = new MCHEmul::PhisicalStorageSubset 
		(_KERNELRAM_SUBSET, RAM, 0xe000, MCHEmul::Address ({ 0x00, 0xe0 }, false), 0x2000);				// 8k (over KernelROM)

	// A map with the subsets swwn from the CPU perspective
	MCHEmul::PhisicalStorageSubsets cpusubsets (
		{
			{ _PAGEZERO_SUBSET,		PageZero }, 
			{ _STACK_SUBSET,		Stack }, 
			{ _RAM0_SUBSET,			RAM0 }, 
			{ _BASICROM_SUBSET,		BasicROM }, 
			{ _BASICRAM_SUBSET,		BasicRAM }, 
			{ _RAM1_SUBSET,			RAM1 }, 
			{ _CHARROM_SUBSET,		CharROM }, 
			{ _VICREGS_SUBSET,		VICIIRegisters }, 
			{ _SIDREGS_SUBSET,		SIDRegisters }, 
			{ _COLOR_SUBSET,		ColorRAM }, 
			{ _CIA1_SUBSET,			CIA1 }, 
			{ _CIA2_SUBSET,			CIA2 }, 
			{ _IO1_SUBSET,			IO1}, 
			{ _IO2_SUBSET,			IO2}, 
			{ _KERNELROM_SUBSET,	KernelROM }, 
			{ _KERNELRAM_SUBSET,	KernelRAM }
		});

	// And same like the VICII chips sees it...
	// Bank 0
	MCHEmul::PhisicalStorageSubset* Bank0RAM0 = new MCHEmul::PhisicalStorageSubset 
		(_BANK0RAM0_SUBSET, RAM, 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 0x1000);
	MCHEmul::PhisicalStorageSubset* Bank0CharROM = new MCHEmul::PhisicalStorageSubset 
		(_BANK0CHARROM_SUBSET, CHARROM, 0x0000, MCHEmul::Address ({ 0x00, 0x10 }, false), 0x1000);
	MCHEmul::PhisicalStorageSubset* Bank0RAM1 = new MCHEmul::PhisicalStorageSubset 
		(_BANK0RAM1_SUBSET, RAM, 0x2000, MCHEmul::Address ({ 0x00, 0x20 }, false), 0x2000);
	// Bank 1
	MCHEmul::PhisicalStorageSubset* Bank1RAM = new MCHEmul::PhisicalStorageSubset 
		(_BANK1RAM_SUBSET, RAM, 0x4000, MCHEmul::Address ({ 0x00, 0x40 }, false), 0x4000);
	// Bank 2
	MCHEmul::PhisicalStorageSubset* Bank2RAM0 = new MCHEmul::PhisicalStorageSubset 
		(_BANK2RAM0_SUBSET, RAM, 0x8000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x1000);
	MCHEmul::PhisicalStorageSubset* Bank2CharROM = new MCHEmul::PhisicalStorageSubset 
		(_BANK2CHARROM_SUBSET, CHARROM, 0x0000, MCHEmul::Address ({ 0x00, 0x90 }, false), 0x1000);
	MCHEmul::PhisicalStorageSubset* Bank2RAM1 = new MCHEmul::PhisicalStorageSubset 
		(_BANK2RAM1_SUBSET, RAM, 0xa000, MCHEmul::Address ({ 0x00, 0xa0 }, false), 0x2000);
	// Bank 3
	MCHEmul::PhisicalStorageSubset* Bank3RAM = new MCHEmul::PhisicalStorageSubset 
		(_BANK3RAM_SUBSET, RAM, 0xc000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x4000);

	// The map with the subsets from the VICII perspective
	MCHEmul::PhisicalStorageSubsets vicIIsubsets (
		{
			{ _BANK0RAM0_SUBSET,	Bank0RAM0 },
			{ _BANK0CHARROM_SUBSET,	Bank0CharROM},
			{ _BANK0RAM1_SUBSET,	Bank0RAM1 },
			{ _BANK1RAM_SUBSET,		Bank1RAM },
			{ _BANK2RAM0_SUBSET,	Bank2RAM0 },
			{ _BANK2CHARROM_SUBSET,	Bank2CharROM},
			{ _BANK2RAM1_SUBSET,	Bank2RAM1 },
			{ _BANK3RAM_SUBSET,		Bank3RAM }
		});

	// A map with all the subsets possible...
	MCHEmul::PhisicalStorageSubsets allsubsets (
		{
			{ _PAGEZERO_SUBSET,		PageZero }, 
			{ _STACK_SUBSET,		Stack }, 
			{ _RAM0_SUBSET,			RAM0 }, 
			{ _BASICROM_SUBSET,		BasicROM }, 
			{ _BASICRAM_SUBSET,		BasicRAM }, 
			{ _RAM1_SUBSET,			RAM1 }, 
			{ _CHARROM_SUBSET,		CharROM }, 
			{ _VICREGS_SUBSET,		VICIIRegisters }, 
			{ _SIDREGS_SUBSET,		SIDRegisters }, 
			{ _COLOR_SUBSET,		ColorRAM }, 
			{ _CIA1_SUBSET,			CIA1 }, 
			{ _CIA2_SUBSET,			CIA2 }, 
			{ _IO1_SUBSET,			IO1}, 
			{ _IO2_SUBSET,			IO2}, 
			{ _KERNELROM_SUBSET,	KernelROM }, 
			{ _KERNELRAM_SUBSET,	KernelRAM },
			{ _BANK0RAM0_SUBSET,	Bank0RAM0 },
			{ _BANK0CHARROM_SUBSET,	Bank0CharROM},
			{ _BANK0RAM1_SUBSET,	Bank0RAM1 },
			{ _BANK1RAM_SUBSET,		Bank1RAM },
			{ _BANK2RAM0_SUBSET,	Bank2RAM0 },
			{ _BANK2CHARROM_SUBSET,	Bank2CharROM},
			{ _BANK2RAM1_SUBSET,	Bank2RAM1 },
			{ _BANK3RAM_SUBSET,		Bank3RAM }
		});

	// Then the views...
	// From the CPU side...
	MCHEmul::MemoryView* CPUView = new MCHEmul::MemoryView (_CPU_VIEW, cpusubsets);
	// ...and from the VICII
	MCHEmul::MemoryView* VICIIView = new MCHEmul::MemoryView (_VICII_VIEW, vicIIsubsets);

	// ...and finally the memory that is the result...
	MCHEmul::Memory::Content result;
	result._phisicalStorages = storages;
	result._subsets = allsubsets;
	result._views = MCHEmul::MemoryViews (
		{
			{ _CPU_VIEW, CPUView },
			{ _VICII_VIEW, VICIIView }
		});

	return (result);
}
