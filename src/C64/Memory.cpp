#include <C64/Memory.hpp>
#include <C64/ColorMemory.hpp>
#include <C64/CIA1Registers.hpp>
#include <C64/CIA2Registers.hpp>

// ---
C64::Memory::Memory (const std::string& lang)
	: MCHEmul::Memory (C64::Memory::standardMemoryContent ())
{
	// In the content...
	if (error () != MCHEmul::_NOERROR)
		return;

	// The default ROMS...
	// They might change depending on the language
	std::string ROMFILE = "./basic.901226-01.bin";
	std::string CHARROMFILE = "./characters.901225-01-ENG.bin";
	std::string KERNELFILE = "./kernal.901227-03-ENG.bin";

	// If the languaje selected is not ENG...
	// then anither char rom file is selected!
	if (lang == "ESP") { CHARROMFILE = "characters.325056-03-ESP.bin"; }
	else if (lang == "JAP") { CHARROMFILE = "characters.906143-02-JAP.bin"; KERNELFILE = "kernal.906145-02-JAP.bin"; }
	else if (lang == "SWE") { CHARROMFILE = "characters.325018-02-SWE.bin"; KERNELFILE = "kernal.325017-02-SWE.bin"; }
	else if (lang == "DKA") { CHARROMFILE = "characters.901225-01-DKA.bin"; KERNELFILE = "kernal.901227-03-DKA.bin"; }

	bool ok = true;
	ok &= subset (_BASICROM_SUBSET) -> loadInto (ROMFILE);
	subset (_BASICROM_SUBSET) -> fixDefaultValues (); // Fix the values for further initializations...
	ok &= subset (_CHARROM_SUBSET) -> loadInto (CHARROMFILE);
	subset (_CHARROM_SUBSET) -> fixDefaultValues ();
	ok &= subset (_BANK0CHARROM_SUBSET) -> loadInto (CHARROMFILE);
	subset (_BANK0CHARROM_SUBSET) -> fixDefaultValues ();
	ok &= subset (_BANK2CHARROM_SUBSET) -> loadInto (CHARROMFILE);
	subset (_BANK2CHARROM_SUBSET) -> fixDefaultValues ();
	ok &= subset (_KERNELROM_SUBSET) -> loadInto (KERNELFILE);
	subset (_KERNELROM_SUBSET) -> fixDefaultValues ();

	if (!ok)
		_error = MCHEmul::_INIT_ERROR;
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
	subset (_PAGEZERO_SUBSET)	-> set (MCHEmul::Address ({ 0x00, 0x00 }, false), MCHEmul::UByte (0xef));
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
	MCHEmul::PhysicalStorage* RAM = 
		new MCHEmul::PhysicalStorage (_RAM, MCHEmul::PhysicalStorage::Type::_RAM, 0x10000);				// 64k
	MCHEmul::PhysicalStorage* BASICROM = 
		new MCHEmul::PhysicalStorage (_BASICROM, MCHEmul::PhysicalStorage::Type::_ROM, 0x2000);			// 8k
	MCHEmul::PhysicalStorage* CHARROM = 
		new MCHEmul::PhysicalStorage (_CHARROM, MCHEmul::PhysicalStorage::Type::_ROM, 0x1000);			// 4k
	MCHEmul::PhysicalStorage* KERNELROM	= 
		new MCHEmul::PhysicalStorage (_KERNELROM, MCHEmul::PhysicalStorage::Type::_ROM, 0x2000);		// 8k

	// The map of phisical storages, used later...
	MCHEmul::PhysicalStorages storages (
		{
			{ _RAM, RAM },
			{ _BASICROM, BASICROM },
			{ _CHARROM, CHARROM },
			{ _KERNELROM, KERNELROM }
		});

	// Subsets
	// ...Over the RAM and ROM as the CPU sees it
	// Page 0
	MCHEmul::PhysicalStorageSubset* PageZero = new MCHEmul::PhysicalStorageSubset 
		(_PAGEZERO_SUBSET, RAM, 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 0x0100);
	// Stack
	MCHEmul::Stack*  Stack = new MCHEmul::Stack 
		(_STACK_SUBSET, RAM, 0x0100, MCHEmul::Address ({ 0x00, 0x01 }, false), 0x0100);
	// Pure RAM. A piece used by BASIC (40k)
	MCHEmul::PhysicalStorageSubset* RAM0 = new MCHEmul::PhysicalStorageSubset 
		(_RAM0_SUBSET, RAM, 0x0200, MCHEmul::Address ({ 0x00, 0x02 }, false), 0x9e00); 					// 40k pure (a bit used by BASIC)
	// Where the basic is can be either ROM o RAM (depends on bits in position 1 of the page 0)	
	MCHEmul::PhysicalStorageSubset* BasicROM = new MCHEmul::PhysicalStorageSubset
		(_BASICROM_SUBSET, BASICROM, 0x0000, MCHEmul::Address ({ 0x00, 0xa0 }, false), 0x2000);	
	MCHEmul::PhysicalStorageSubset* BasicRAM = new MCHEmul::PhysicalStorageSubset
		(_BASICRAM_SUBSET, RAM, 0x0a000, MCHEmul::Address ({ 0x00, 0xa0 }, false), 0x2000);				// 8k (over BasicROM)
	// 	Pure RAM (4k)
	MCHEmul::PhysicalStorageSubset* RAM1 = new MCHEmul::PhysicalStorageSubset
		(_RAM1_SUBSET, RAM, 0x0c000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x1000);					// 4k
	// Where the CharROM is defined we have also the access to the chips (VIC, SID,...)
	MCHEmul::PhysicalStorageSubset* CharROM = new MCHEmul::PhysicalStorageSubset
		(_CHARROM_SUBSET, CHARROM, 0x0000, MCHEmul::Address ({ 0x00, 0xd0 }, false), 0x1000);
	MCHEmul::PhysicalStorageSubset* VICIIRegisters = new COMMODORE::VICIIRegisters 
		(/** id = C64::VICIIRegisters::_VICREGS_SUBSET */ RAM, 0xd000, MCHEmul::Address ({ 0x00, 0xd0 }, false), 0x0400);
	MCHEmul::PhysicalStorageSubset* SIDRegisters = new COMMODORE::SIDRegisters 
		(/** id = C64::SIDRegisters::_SIDREGS_SUBSET */ RAM, 0xd400, MCHEmul::Address ({ 0x00, 0xd4 }, false), 0x0400);
	MCHEmul::PhysicalStorageSubset* ColorRAM = new C64::ColorRAMMemory (_COLOR_SUBSET, RAM); // at d800, 0x400 long bytes long
	MCHEmul::PhysicalStorageSubset* CIA1 = new C64::CIA1Registers 
		(/** id = C64::CIA1Registers::_CIA1_SUBSET */ RAM, 0xdc00, MCHEmul::Address ({ 0x00, 0xdc }, false), 0x0100);
	MCHEmul::PhysicalStorageSubset* CIA2 = new C64::CIA2Registers 
		(/** id = C64::CIA2Registers::_CIA2_SUBSET */ RAM, 0xdd00, MCHEmul::Address ({ 0x00, 0xdd }, false), 0x0100);
	MCHEmul::PhysicalStorageSubset* IO1 = new MCHEmul::PhysicalStorageSubset 
		(_IO1_SUBSET, RAM, 0xde00, MCHEmul::Address ({ 0x00, 0xde }, false), 0x0100); 
	MCHEmul::PhysicalStorageSubset* IO2 = new MCHEmul::PhysicalStorageSubset 
		(_IO2_SUBSET, RAM, 0xdf00, MCHEmul::Address ({ 0x00, 0xdf }, false), 0x0100); 
	// Where the kernel is defined can be eiher RAM or ROM (
	MCHEmul::PhysicalStorageSubset* KernelROM = new MCHEmul::PhysicalStorageSubset 
		(_KERNELROM_SUBSET, KERNELROM, 0x0000, MCHEmul::Address ({ 0x00, 0xe0 }, false), 0x2000);
	MCHEmul::PhysicalStorageSubset* KernelRAM = new MCHEmul::PhysicalStorageSubset 
		(_KERNELRAM_SUBSET, RAM, 0xe000, MCHEmul::Address ({ 0x00, 0xe0 }, false), 0x2000);				// 8k (over KernelROM)

	// A map with the subsets swwn from the CPU perspective
	MCHEmul::PhysicalStorageSubsets cpusubsets (
		{
			{ _PAGEZERO_SUBSET,									PageZero }, 
			{ _STACK_SUBSET,									Stack }, 
			{ _RAM0_SUBSET,										RAM0 }, 
			{ _BASICROM_SUBSET,									BasicROM }, 
			{ _BASICRAM_SUBSET,									BasicRAM }, 
			{ _RAM1_SUBSET,										RAM1 }, 
			{ _CHARROM_SUBSET,									CharROM }, 
			{ COMMODORE::VICIIRegisters::_VICREGS_SUBSET,		VICIIRegisters }, 
			{ COMMODORE::SIDRegisters::_SIDREGS_SUBSET,			SIDRegisters }, 
			{ _COLOR_SUBSET,									ColorRAM }, 
			{ C64::CIA1Registers::_CIA1_SUBSET,					CIA1 }, 
			{ C64::CIA2Registers::_CIA2_SUBSET,					CIA2 }, 
			{ _IO1_SUBSET,										IO1}, 
			{ _IO2_SUBSET,										IO2}, 
			{ _KERNELROM_SUBSET,								KernelROM }, 
			{ _KERNELRAM_SUBSET,								KernelRAM }
		});

	// And same like the VICII chips sees it...
	// Bank 0
	MCHEmul::PhysicalStorageSubset* Bank0RAM0 = new MCHEmul::PhysicalStorageSubset 
		(_BANK0RAM0_SUBSET, RAM, 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 0x1000);
	MCHEmul::PhysicalStorageSubset* Bank0CharROM = new MCHEmul::PhysicalStorageSubset 
		(_BANK0CHARROM_SUBSET, CHARROM, 0x0000, MCHEmul::Address ({ 0x00, 0x10 }, false), 0x1000); // 2048 bytes per char set...
	MCHEmul::PhysicalStorageSubset* Bank0RAM1 = new MCHEmul::PhysicalStorageSubset 
		(_BANK0RAM1_SUBSET, RAM, 0x2000, MCHEmul::Address ({ 0x00, 0x20 }, false), 0x2000);
	// Bank 1
	MCHEmul::PhysicalStorageSubset* Bank1RAM = new MCHEmul::PhysicalStorageSubset 
		(_BANK1RAM_SUBSET, RAM, 0x4000, MCHEmul::Address ({ 0x00, 0x40 }, false), 0x4000);
	// Bank 2
	MCHEmul::PhysicalStorageSubset* Bank2RAM0 = new MCHEmul::PhysicalStorageSubset 
		(_BANK2RAM0_SUBSET, RAM, 0x8000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x1000);
	MCHEmul::PhysicalStorageSubset* Bank2CharROM = new MCHEmul::PhysicalStorageSubset 
		(_BANK2CHARROM_SUBSET, CHARROM, 0x0000, MCHEmul::Address ({ 0x00, 0x90 }, false), 0x1000); // 2048 bytes per char set...
	MCHEmul::PhysicalStorageSubset* Bank2RAM1 = new MCHEmul::PhysicalStorageSubset 
		(_BANK2RAM1_SUBSET, RAM, 0xa000, MCHEmul::Address ({ 0x00, 0xa0 }, false), 0x2000);
	// Bank 3
	MCHEmul::PhysicalStorageSubset* Bank3RAM = new MCHEmul::PhysicalStorageSubset 
		(_BANK3RAM_SUBSET, RAM, 0xc000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x4000);

	// The map with the subsets from the VICII perspective
	MCHEmul::PhysicalStorageSubsets vicIIsubsets (
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
	MCHEmul::PhysicalStorageSubsets allsubsets (
		{
			{ _PAGEZERO_SUBSET,									PageZero }, 
			{ _STACK_SUBSET,									Stack }, 
			{ _RAM0_SUBSET,										RAM0 }, 
			{ _BASICROM_SUBSET,									BasicROM }, 
			{ _BASICRAM_SUBSET,									BasicRAM }, 
			{ _RAM1_SUBSET,										RAM1 }, 
			{ _CHARROM_SUBSET,									CharROM }, 
			{ COMMODORE::VICIIRegisters::_VICREGS_SUBSET,		VICIIRegisters }, 
			{ COMMODORE::SIDRegisters::_SIDREGS_SUBSET,			SIDRegisters }, 
			{ _COLOR_SUBSET,									ColorRAM }, 
			{ C64::CIA1Registers::_CIA1_SUBSET,					CIA1 }, 
			{ C64::CIA2Registers::_CIA2_SUBSET,					CIA2 }, 
			{ _IO1_SUBSET,										IO1}, 
			{ _IO2_SUBSET,										IO2}, 
			{ _KERNELROM_SUBSET,								KernelROM }, 
			{ _KERNELRAM_SUBSET,								KernelRAM },
			{ _BANK0RAM0_SUBSET,								Bank0RAM0 },
			{ _BANK0CHARROM_SUBSET,								Bank0CharROM},
			{ _BANK0RAM1_SUBSET,								Bank0RAM1 },
			{ _BANK1RAM_SUBSET,									Bank1RAM },
			{ _BANK2RAM0_SUBSET,								Bank2RAM0 },
			{ _BANK2CHARROM_SUBSET,								Bank2CharROM},
			{ _BANK2RAM1_SUBSET,								Bank2RAM1 },
			{ _BANK3RAM_SUBSET,									Bank3RAM }
		});

	// Then the views...
	// From the CPU side...
	MCHEmul::MemoryView* CPUView = new MCHEmul::MemoryView (_CPU_VIEW, cpusubsets);
	// ...and from the VICII
	MCHEmul::MemoryView* VICIIView = new MCHEmul::MemoryView (_VICII_VIEW, vicIIsubsets);

	// ...and finally the memory that is the result...
	MCHEmul::Memory::Content result;
	result._physicalStorages = storages;
	result._subsets = allsubsets;
	result._views = MCHEmul::MemoryViews (
		{
			{ _CPU_VIEW, CPUView },
			{ _VICII_VIEW, VICIIView }
		});

	return (result);
}
