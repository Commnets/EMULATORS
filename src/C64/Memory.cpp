#include <C64/Memory.hpp>
#include <C64/ColorMemory.hpp>
#include <C64/IO6510Registers.hpp>
#include <C64/CIA1Registers.hpp>
#include <C64/CIA2Registers.hpp>
#include <C64/IOExpansionMemory.hpp>
#include <C64/Cartridge.hpp>

const MCHEmul::Address C64::Memory::_POS0_ADDRESS = MCHEmul::Address ({ 0x00, 0x00 }, false);
const MCHEmul::Address C64::Memory::_POS1_ADDRESS = MCHEmul::Address ({ 0x01, 0x00 }, false);
const MCHEmul::Address C64::Memory::_BASICROMINIT_ADDRESS = MCHEmul::Address ({ 0x00, 0xa0 }, false);
const MCHEmul::Address C64::Memory::_BASICROMEND_ADDRESS = MCHEmul::Address ({ 0xff, 0xbf }, false);
const MCHEmul::Address C64::Memory::_CHARROMINIT_ADDRESS = MCHEmul::Address ({ 0x00, 0xd0 }, false);
const MCHEmul::Address C64::Memory::_CHARROMEND_ADDRESS = MCHEmul::Address ({ 0xff, 0xdf }, false);
const MCHEmul::Address C64::Memory::_KERNELROMINIT_ADDRESS = MCHEmul::Address ({ 0x00, 0xe0 }, false);
const MCHEmul::Address C64::Memory::_KERNELROMEND_ADDRESS = MCHEmul::Address ({ 0xff, 0xff }, false);

// ---
C64::Memory::Memory (const std::string& lang)
	: MCHEmul::Memory (C64::Memory::standardMemoryContent ()),
	  _VICIIView (nullptr),
	  _cartridge (nullptr),
	  _basicROM (nullptr),
	  _basicRAM (nullptr),
	  _kernelROM (nullptr),
	  _kernelRAM (nullptr),
	  _charROM (nullptr),
	  _vicIIRegisters (nullptr),
	  _sidRegisters (nullptr),
	  _colorRAM (nullptr),
	  _cia1Registers (nullptr),
	  _cia2registers (nullptr),
	  _io1Registers (nullptr),
	  _io2registers (nullptr),
	  _expansionRAMLO (nullptr)
{
	// In the content...
	if (error () != MCHEmul::_NOERROR)
		return;

	_basicROM			= subset (_BASICROM_SUBSET);
	_basicRAM			= subset (_BASICRAM_SUBSET);
	_kernelROM			= subset (_KERNELROM_SUBSET);
	_kernelRAM			= subset (_KERNELRAM_SUBSET);
	_charROM			= subset (_CHARROM_SUBSET);
	_vicIIRegisters		= subset (COMMODORE::VICIIRegisters::_VICREGS_SUBSET);
	_sidRegisters		= subset (COMMODORE::SIDRegisters::_SIDREGS_SUBSET);
	_colorRAM			= subset (_COLOR_SUBSET);
	_cia1Registers		= subset (C64::CIA1Registers::_CIA1_SUBSET);
	_cia2registers		= subset (C64::CIA2Registers::_CIA2_SUBSET);
	_io1Registers		= subset (C64::IOExpansionMemoryI::_IO1_SUBSET);
	_io2registers		= subset (C64::IOExpansionMemoryII::_IO2_SUBSET);
	_expansionRAMLO		= subset (_RAM01_SUBSET);

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

	// The initial basic configuration of the memory is the standard
	// BASIC, KERNEL connected, CHARROM not connected, and no cartridge extension...
	configureMemoryStructure (true, true, false, false, false, false);

	return (true);
}

// ---
void C64::Memory::configureMemoryStructure (bool BASIC, bool KERNEL, bool CHARROM, 
	bool ROML, bool ROMH1, bool ROMH2)
{
	_expansionRAMLO		-> setActiveForReading (!ROML);

	_basicROM			-> setActiveForReading (BASIC);
	_basicRAM			-> setActiveForReading (!ROMH1 && !BASIC);

	_kernelROM			-> setActiveForReading (KERNEL);
	_kernelRAM			-> setActiveForReading (!ROMH2 && !KERNEL);

	_charROM			-> setActiveForReading (CHARROM);
	_vicIIRegisters		-> setActiveForReading (!CHARROM);
	_sidRegisters		-> setActiveForReading (!CHARROM);
	_colorRAM			-> setActiveForReading (!CHARROM);
	_cia1Registers		-> setActiveForReading (!CHARROM);
	_cia2registers		-> setActiveForReading (!CHARROM);
	_io1Registers		-> setActiveForReading (!CHARROM);
	_io2registers		-> setActiveForReading (!CHARROM);

	if (_cartridge != nullptr)
		_cartridge -> configureMemoryStructure (ROML, ROMH1, ROMH2);
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
	// First two bytes of the memory that are used by the 6510.
	MCHEmul::PhysicalStorageSubset* IO6510 = new C64::IO6510Registers
		(/** id = C64::IO6510Registers::_IO6510REGISTERS_SUBSET */ RAM, 
			0x0000, MCHEmul::Address ( { 0x00, 0x00 }, false), 0x0002);
	// Page 0 (except the two first bytes)
	MCHEmul::PhysicalStorageSubset* PageZero = new MCHEmul::PhysicalStorageSubset 
		(_PAGEZERO_SUBSET, RAM, 0x0002, MCHEmul::Address ({ 0x02, 0x00 }, false), 0x00fe);
	// Stack
	MCHEmul::Stack*  Stack = new MCHEmul::Stack 
		(_STACK_SUBSET, RAM, 0x0100, MCHEmul::Address ({ 0x00, 0x01 }, false), 0x0100);
	// Pure RAM. A piece used by BASIC (40k)
	MCHEmul::PhysicalStorageSubset* RAM00 = new MCHEmul::PhysicalStorageSubset 
		(_RAM00_SUBSET, RAM, 0x0200, MCHEmul::Address ({ 0x00, 0x02 }, false), 0x7e00); 					// 31,5k pure (a bit used by BASIC)
	// In the last part of the RAM a 8k Expansion ROM is located (no active unless a expansion element uses it)
	MCHEmul::PhysicalStorageSubset* RAM01 = new MCHEmul::PhysicalStorageSubset
		(_RAM01_SUBSET, RAM, 0x8000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x2000);						// 2k pure a (a bit used by basic, but can be shared with expansion port)
	// Where the basic is can be either ROM or RAM (depends on bits in position 1 of the page 0) or EXPANSION ROM (when uses it)
	MCHEmul::PhysicalStorageSubset* BasicROM = new MCHEmul::PhysicalStorageSubset
		(_BASICROM_SUBSET, BASICROM, 0x0000, MCHEmul::Address ({ 0x00, 0xa0 }, false), 0x2000);	
	MCHEmul::PhysicalStorageSubset* BasicRAM = new MCHEmul::PhysicalStorageSubset
		(_BASICRAM_SUBSET, RAM, 0x0a000, MCHEmul::Address ({ 0x00, 0xa0 }, false), 0x2000);				// 8k (over Basic ROM)
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
	// Used by the expansion cartridges...
	MCHEmul::PhysicalStorageSubset* IO1 = new C64::IOExpansionMemoryI
		(/** id = C64::IOExpansionMemoryI::_IO1_SUBSET */ RAM, 0xde00, MCHEmul::Address ({ 0x00, 0xde }, false), 0x0100);
	MCHEmul::PhysicalStorageSubset* IO2 = new C64::IOExpansionMemoryII
		(/** id = C64::IOExpansionMemoryII::_IO2_SUBSET */ RAM, 0xdf00, MCHEmul::Address ({ 0x00, 0xdf }, false), 0x0100);
	// Where the kernel is defined can be either RAM or ROM (depending on the bits 0, 1, 2 at 0x01 position)
	MCHEmul::PhysicalStorageSubset* KernelROM = new MCHEmul::PhysicalStorageSubset 
		(_KERNELROM_SUBSET, KERNELROM, 0x0000, MCHEmul::Address ({ 0x00, 0xe0 }, false), 0x2000);
	MCHEmul::PhysicalStorageSubset* KernelRAM = new MCHEmul::PhysicalStorageSubset 
		(_KERNELRAM_SUBSET, RAM, 0xe000, MCHEmul::Address ({ 0x00, 0xe0 }, false), 0x2000);				// 8k (over Kernel ROM)

	// A map with the subsets swwn from the CPU perspective
	MCHEmul::PhysicalStorageSubsets cpusubsets (
		{
			{ C64::IO6510Registers::_IO6510REGISTERS_SUBSET,		IO6510}, 
			{ _PAGEZERO_SUBSET,										PageZero }, 
			{ _STACK_SUBSET,										Stack }, 
			{ _RAM00_SUBSET,										RAM00 }, 
			{ _RAM01_SUBSET,										RAM01 }, 
			{ _BASICROM_SUBSET,										BasicROM }, 
			{ _BASICRAM_SUBSET,										BasicRAM }, 
			{ _RAM1_SUBSET,											RAM1 }, 
			{ _CHARROM_SUBSET,										CharROM }, 
			{ COMMODORE::VICIIRegisters::_VICREGS_SUBSET,			VICIIRegisters }, 
			{ COMMODORE::SIDRegisters::_SIDREGS_SUBSET,				SIDRegisters }, 
			{ _COLOR_SUBSET,										ColorRAM }, 
			{ C64::CIA1Registers::_CIA1_SUBSET,						CIA1 }, 
			{ C64::CIA2Registers::_CIA2_SUBSET,						CIA2 }, 
			{ C64::IOExpansionMemoryI::_IO1_SUBSET,					IO1}, 
			{ C64::IOExpansionMemoryII::_IO2_SUBSET,				IO2}, 
			{ _KERNELROM_SUBSET,									KernelROM }, 
			{ _KERNELRAM_SUBSET,									KernelRAM }
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
			{ C64::IO6510Registers::_IO6510REGISTERS_SUBSET,	IO6510}, 
			{ _PAGEZERO_SUBSET,									PageZero }, 
			{ _STACK_SUBSET,									Stack }, 
			{ _RAM00_SUBSET,									RAM00 }, 
			{ _RAM01_SUBSET,									RAM01 }, 
			{ _BASICROM_SUBSET,									BasicROM }, 
			{ _BASICRAM_SUBSET,									BasicRAM }, 
			{ _RAM1_SUBSET,										RAM1 }, 
			{ _CHARROM_SUBSET,									CharROM }, 
			{ COMMODORE::VICIIRegisters::_VICREGS_SUBSET,		VICIIRegisters }, 
			{ COMMODORE::SIDRegisters::_SIDREGS_SUBSET,			SIDRegisters }, 
			{ _COLOR_SUBSET,									ColorRAM }, 
			{ C64::CIA1Registers::_CIA1_SUBSET,					CIA1 }, 
			{ C64::CIA2Registers::_CIA2_SUBSET,					CIA2 }, 
			{ C64::IOExpansionMemoryI::_IO1_SUBSET,				IO1}, 
			{ C64::IOExpansionMemoryII::_IO2_SUBSET,			IO2}, 
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
