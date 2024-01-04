#include <VIC20/Memory.hpp>
#include <VIC20/VIA1Registers.hpp>
#include <VIC20/VIA2Registers.hpp>
#include <COMMODORE/VICIRegisters.hpp>

// ---
VIC20::Memory::Memory (VIC20::Memory::Configuration cfg, const std::string& lang)
	: MCHEmul::Memory (0, VIC20::Memory::standardMemoryContent (), { }),
	  _configuration (cfg),
	  _BANK0 (nullptr), _BANK0UC (nullptr),
	  _BANK1 (nullptr), _BANK1UC (nullptr),
	  _BANK2 (nullptr), _BANK2UC (nullptr),
	  _BANK3 (nullptr), _BANK3UC (nullptr),
	  _BANK5 (nullptr), _BANK5UC (nullptr)
{
	// In the content...
	if (error () != MCHEmul::_NOERROR)
		return;

	// The default ROMS...
	// They might change depending on the language
	std::string ROMFILE = "./basic.901486-01.bin";
	std::string CHARROMFILE = "./characters.901460-03-ENG.bin";
	std::string KERNELFILE = "./kernal.901486-07-ENG.bin";

	// If the languaje selected is not ENG...
	// then anither char rom file is selected!
	if (lang == "JAP") { CHARROMFILE = "characters.901460-02-JAP.bin"; KERNELFILE = "kernal.901486-02-JAP.bin"; }
	else if (lang == "SWE") { CHARROMFILE = "characters.NecP22101-207-SWE.bin"; KERNELFILE = "kernal.NecP22081-206-SWE.bin"; }
	else if (lang == "DKA") { CHARROMFILE = "characters.DK901460-03-DKK.bin"; KERNELFILE = "kernal.901486-07-DKK.bin"; }

	bool ok = true;
	ok &= subset (_BASICROM_SUBSET) -> loadInto (ROMFILE);
	subset (_BASICROM_SUBSET) -> fixDefaultValues (); // Fix the values for further initializations...
	ok &= subset (_CHARROM_SUBSET) -> loadInto (CHARROMFILE);
	subset (_CHARROM_SUBSET) -> fixDefaultValues ();
	ok &= subset (_KERNELROM_SUBSET) -> loadInto (KERNELFILE);
	subset (_KERNELROM_SUBSET) -> fixDefaultValues ();

	if (!ok)
		_error = MCHEmul::_INIT_ERROR;

	// Gets a pointer to the main pieces of the memory that are actibable...
	_BANK0		= subset (_BANK0_SUBSET);
	_BANK0UC	= dynamic_cast <VIC20::NotConnectedPhysicalStorageSubset*> (subset (_BANK0UC_SUBSET));
	_BANK1		= subset (_BANK1_SUBSET);
	_BANK1UC	= dynamic_cast <VIC20::NotConnectedPhysicalStorageSubset*> (subset (_BANK1UC_SUBSET));
	_BANK2		= subset (_BANK2_SUBSET);
	_BANK2UC	= dynamic_cast <VIC20::NotConnectedPhysicalStorageSubset*> (subset (_BANK2UC_SUBSET));
	_BANK3		= subset (_BANK3_SUBSET);
	_BANK3UC	= dynamic_cast <VIC20::NotConnectedPhysicalStorageSubset*> (subset (_BANK3UC_SUBSET));
	_BANK5		= subset (_BANK5_SUBSET);
	_BANK5UC	= dynamic_cast <VIC20::NotConnectedPhysicalStorageSubset*> (subset (_BANK5UC_SUBSET));
	assert (_BANK0 != nullptr && _BANK0UC != nullptr &&
			_BANK1 != nullptr && _BANK1UC != nullptr &&
			_BANK2 != nullptr && _BANK2UC != nullptr &&
			_BANK3 != nullptr && _BANK3UC != nullptr &&
			_BANK5 != nullptr && _BANK5UC != nullptr); // None of the can be nullptr...

	// Sets the configuration of the memory...
	setConfiguration (_configuration);
}

// ---
void VIC20::Memory::setConfiguration (Configuration cfg)
{
	// Attending to the configuration different options are active or not active!
	switch (_configuration = cfg)
	{
		case VIC20::Memory::Configuration::_NOEXPANDED: 
			{
				// None of offical banks are active...
				// BASIC space is very short...
				_BANK0			-> setActive (false);
				_BANK0UC		-> setActive (true);
				_BANK1			-> setActive (false);
				_BANK1UC		-> setActive (true);
				_BANK2			-> setActive (false);
				_BANK2UC		-> setActive (true);
				_BANK3			-> setActive (false);
				_BANK3UC		-> setActive (true);
				_BANK5			-> setActive (false);
				_BANK5UC		-> setActive (true);
			}

			break;

		case VIC20::Memory::Configuration::_3KEXPANSION: 
			{
				// Only the first bank is active...
				_BANK0			-> setActive (true);		// 3k, BASIC longer...
				_BANK0UC		-> setActive (false);

				_BANK1			-> setActive (false);
				_BANK1UC		-> setActive (true);
				_BANK2			-> setActive (false);
				_BANK2UC		-> setActive (true);
				_BANK3			-> setActive (false);
				_BANK3UC		-> setActive (true);
				_BANK5			-> setActive (false);
				_BANK5UC		-> setActive (true);
			}

			break;

		case VIC20::Memory::Configuration::_8KEXPANSION:
			{
				// The first two banks are active...
				_BANK0			-> setActive (true);		// 3k, BASIC longer...
				_BANK0UC		-> setActive (false);
				_BANK1			-> setActive (true);		// 8k.
				_BANK1UC		-> setActive (false);

				_BANK2			-> setActive (false);
				_BANK2UC		-> setActive (true);
				_BANK3			-> setActive (false);
				_BANK3UC		-> setActive (true);
				_BANK5			-> setActive (false);
				_BANK5UC		-> setActive (true);
			}

			break;

		case VIC20::Memory::Configuration::_16KEXPANSION:
			{
				// 3 banks are active...
				_BANK0			-> setActive (true);		// 3k, BASIC longer...
				_BANK0UC		-> setActive (false);
				_BANK1			-> setActive (true);		// 8k.
				_BANK1UC		-> setActive (false);
				_BANK2			-> setActive (true);		// 8k.
				_BANK2UC		-> setActive (false);

				_BANK3			-> setActive (false);
				_BANK3UC		-> setActive (true);
				_BANK5			-> setActive (false);
				_BANK5UC		-> setActive (true);
			}

			break;

		case VIC20::Memory::Configuration::_24KEXPANSION:
			{
				// The main 4 bank area actve...
				_BANK0			-> setActive (true);		// 3k. BASIC longer
				_BANK0UC		-> setActive (false);
				_BANK1			-> setActive (true);		// 8k.
				_BANK1UC		-> setActive (false);
				_BANK2			-> setActive (true);		// 8k.
				_BANK2UC		-> setActive (false);
				_BANK3			-> setActive (true);		// 8k.
				_BANK3UC		-> setActive (false);

				_BANK5			-> setActive (false);
				_BANK5UC		-> setActive (true);
			}

			break;

		// The BANK5 is usually active when autorun cartridges 8k are introduced...

		default:
			// It shouldn't exist, but just in case...
			assert (false);
			break;
	}
}

// ---
bool VIC20::Memory::initialize ()
{
	bool result = MCHEmul::Memory::initialize ();
	if (!result)
		return (false);

	// The active view has to be initially the CPU view...
	setCPUView ();

	return (true);
}

// ---
MCHEmul::Memory::Content VIC20::Memory::standardMemoryContent ()
{
	/** All dirs in Little - endian format. */

	// Phisical storages
	MCHEmul::PhysicalStorage* RAM = 
		new MCHEmul::PhysicalStorage (_RAM, MCHEmul::PhysicalStorage::Type::_RAM, 0x10000);				// 64k
	MCHEmul::PhysicalStorage* CHARROM = 
		new MCHEmul::PhysicalStorage (_CHARROM, MCHEmul::PhysicalStorage::Type::_ROM, 0x1000);			// 4k
	MCHEmul::PhysicalStorage* BASICROM = 
		new MCHEmul::PhysicalStorage (_BASICROM, MCHEmul::PhysicalStorage::Type::_ROM, 0x2000);			// 8k
	MCHEmul::PhysicalStorage* KERNELROM	= 
		new MCHEmul::PhysicalStorage (_KERNELROM, MCHEmul::PhysicalStorage::Type::_ROM, 0x2000);		// 8k

	// The map of phisical storages, used later...
	MCHEmul::PhysicalStorages storages (
		{
			{ _RAM, RAM },
			{ _CHARROM, CHARROM },
			{ _BASICROM, BASICROM },
			{ _KERNELROM, KERNELROM }
		});

	// Subsets
	// Page 0: The quickest possible access...
	MCHEmul::PhysicalStorageSubset* PageZero = new MCHEmul::PhysicalStorageSubset 
		(_PAGEZERO_SUBSET, RAM, 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 0x0100);
	// Page 1: Stack, Where the CPU stores info...
	MCHEmul::Stack* Stack = new MCHEmul::Stack 
		(_STACK_SUBSET, RAM, 0x0100, MCHEmul::Address ({ 0x00, 0x01 }, false), 0x0100);
	// Page 2 & 3:
	MCHEmul::PhysicalStorageSubset* Page2And3 = new MCHEmul::PhysicalStorageSubset 
		(_PAGE2AND3_SUBSET, RAM, 0x0200, MCHEmul::Address ({ 0x00, 0x02 }, false), 0x0200);

	// Block 0 of RAM. Expansion: 3 k. 
	MCHEmul::PhysicalStorageSubset* BANK0 = new MCHEmul::PhysicalStorageSubset 
		(_BANK0_SUBSET, RAM, 0x0400, MCHEmul::Address ({ 0x00, 0x04 }, false), 0x0c00);
	VIC20::NotConnectedPhysicalStorageSubset* BANK0_UC = new VIC20::NotConnectedPhysicalStorageSubset
		(_BANK0UC_SUBSET, MCHEmul::UByte (0x04), RAM, 0x0400, MCHEmul::Address ({ 0x00, 0x04 }, false), 0x0c00);

	// Very BASIC RAM; Usually where the BASIC area is...and also the Screen RAM in not expanded systems (or 3k)
	MCHEmul::PhysicalStorageSubset* BASICAREA = new MCHEmul::PhysicalStorageSubset 
		(_BASICAREA_SUBSET, RAM, 0x1000, MCHEmul::Address ({ 0x00, 0x10 }, false), 0x1000);
	
	// Block 1 of RAM. Expansion: 8 k
	MCHEmul::PhysicalStorageSubset* BANK1 = new MCHEmul::PhysicalStorageSubset 
		(_BANK1_SUBSET, RAM, 0x2000, MCHEmul::Address ({ 0x00, 0x20 }, false), 0x2000); // The connected one...
	VIC20::NotConnectedPhysicalStorageSubset* BANK1_UC = new VIC20::NotConnectedPhysicalStorageSubset
		(_BANK1UC_SUBSET, MCHEmul::UByte (0x20), RAM, 0x2000, MCHEmul::Address ({ 0x00, 0x20 }, false), 0x2000); // The not connected...
	
	// Block 2 of RAM. Expansion: 8 K
	MCHEmul::PhysicalStorageSubset* BANK2 = new MCHEmul::PhysicalStorageSubset 
		(_BANK2_SUBSET, RAM, 0x4000, MCHEmul::Address ({ 0x00, 0x40 }, false), 0x2000); // The connected one...
	VIC20::NotConnectedPhysicalStorageSubset* BANK2_UC = new VIC20::NotConnectedPhysicalStorageSubset
		(_BANK2UC_SUBSET, MCHEmul::UByte (0x40), RAM, 0x4000, MCHEmul::Address ({ 0x00, 0x40 }, false), 0x2000); // The not connected...
	
	// Block 3 of RAM. Expansion: 8 k
	MCHEmul::PhysicalStorageSubset* BANK3 = new MCHEmul::PhysicalStorageSubset 
		(_BANK3_SUBSET, RAM, 0x6000, MCHEmul::Address ({ 0x00, 0x60 }, false), 0x2000); // The connected one...
	VIC20::NotConnectedPhysicalStorageSubset* BANK3_UC = new VIC20::NotConnectedPhysicalStorageSubset
		(_BANK3UC_SUBSET, MCHEmul::UByte (0x60), RAM, 0x6000, MCHEmul::Address ({ 0x00, 0x60 }, false), 0x2000); // The not connected...
	
	// Char ROM
	MCHEmul::PhysicalStorageSubset* CharROM = new MCHEmul::PhysicalStorageSubset
		(_CHARROM_SUBSET, CHARROM, 0x0000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x1000);
	
	// VICI, VIA1 and VIA2...
	// Each of them has only 16 accesible registers, at the end of which there is a free memory that seem to mirror
	// part of the registers of the chip, but only when poking and not when peeking (random value)...
	MCHEmul::PhysicalStorageSubset* VICIRegisters = new COMMODORE::VICIRegisters
		(/** id = COMMODORE::VICIRegisters::_VICREGS_SUBSET */ RAM, 0x9000, MCHEmul::Address ({ 0x00, 0x90 }, false), 0x010);
	MCHEmul::PhysicalStorageSubset* VICIRegisters_After = new MCHEmul::PhysicalStorageSubset 
		(_VICIRAFTER_SUBSET, RAM, 0x9010, MCHEmul::Address ({ 0x10, 0x90 }, false), 0x0100);
	MCHEmul::PhysicalStorageSubset* VIA1Registers = new VIC20::VIA1Registers
		(/** id = VIC20::VIA1Registers::_VIA1_SUBSET */ RAM, 0x9110, MCHEmul::Address ({ 0x10, 0x91 }, false), 0x010);
	MCHEmul::PhysicalStorageSubset* VIA2Registers = new VIC20::VIA2Registers
		(/** id = VIC20::VIA2Registers::_VIA2_SUBSET */ RAM, 0x9120, MCHEmul::Address ({ 0x20, 0x91 }, false), 0x010);
	MCHEmul::PhysicalStorageSubset* VIARegisters_After = new MCHEmul::PhysicalStorageSubset 
		(_VIAAFTER_SUBSET, RAM, 0x9130, MCHEmul::Address ({ 0x30, 0x91 }, false), 0x06d0);
	
	// Block 4 of RAM, after the IO registers.
	// This block is used by the system for the screen memory and for IO data...
	// This piece is officially subdivided in two blocks of 1024 (0x0400) bytes each...
	MCHEmul::PhysicalStorageSubset* BANK4 = new MCHEmul::PhysicalStorageSubset 
		(_BANK4_SUBSET, RAM, 0x9800, MCHEmul::Address ({ 0x00, 0x98 }, false), 0x0800);
	
	// Block 5 of RAM. Expansion: 8 k.
	// Cartridges usually use this space to start...
	MCHEmul::PhysicalStorageSubset* BANK5 = new MCHEmul::PhysicalStorageSubset 
		(_BANK5_SUBSET, RAM, 0xa000, MCHEmul::Address ({ 0x00, 0xa0 }, false), 0x2000); // The connected one...
	VIC20::NotConnectedPhysicalStorageSubset* BANK5_UC = new VIC20::NotConnectedPhysicalStorageSubset 
		(_BANK5UC_SUBSET, MCHEmul::UByte (0xa0), RAM, 0xa000, MCHEmul::Address ({ 0x00, 0xa0 }, false), 0x2000); // The not connected...
	
	// ROMS
	// Basic ROM
	MCHEmul::PhysicalStorageSubset* BasicROM = new MCHEmul::PhysicalStorageSubset
		(_BASICROM_SUBSET, BASICROM, 0x0000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x2000);	
	// Kernel ROM
	MCHEmul::PhysicalStorageSubset* KernelROM = new MCHEmul::PhysicalStorageSubset 
		(_KERNELROM_SUBSET, KERNELROM, 0x0000, MCHEmul::Address ({ 0x00, 0xe0 }, false), 0x2000);

	// A map with all the subsets possible...
	MCHEmul::PhysicalStorageSubsets allsubsets (
		{
			{ _PAGEZERO_SUBSET,										PageZero }, 
			{ _STACK_SUBSET,										Stack }, 
			{ _PAGE2AND3_SUBSET,									Page2And3}, 
			{ _BANK0_SUBSET,										BANK0 }, 
			{ _BANK0UC_SUBSET,										BANK0_UC }, 
			{ _BASICAREA_SUBSET,									BASICAREA }, 
			{ _BANK1_SUBSET,										BANK1 }, 
			{ _BANK1UC_SUBSET,										BANK1_UC }, 
			{ _BANK2_SUBSET,										BANK2 }, 
			{ _BANK2UC_SUBSET,										BANK2_UC }, 
			{ _BANK3_SUBSET,										BANK3 }, 
			{ _BANK3UC_SUBSET,										BANK3_UC }, 
			{ _CHARROM_SUBSET,										CharROM },
			{ COMMODORE::VICIRegisters::_VICREGS_SUBSET,			VICIRegisters },
			{ _VICIRAFTER_SUBSET,									VICIRegisters_After },
			{ VIC20::VIA1Registers::_VIA1_SUBSET,					VIA1Registers },
			{ VIC20::VIA2Registers::_VIA2_SUBSET,					VIA2Registers },
			{ _VIAAFTER_SUBSET,										VIARegisters_After },
			{ _BANK4_SUBSET,										BANK4 }, 
			{ _BANK5_SUBSET,										BANK5 }, 
			{ _BANK5UC_SUBSET,										BANK5_UC }, 
			{ _BASICROM_SUBSET,										BasicROM }, 
			{ _KERNELROM_SUBSET,									KernelROM }, 
		});

	// Then the views...
	// From any side...
	MCHEmul::MemoryView* GlobalView = new MCHEmul::MemoryView (_CPU_VIEW, allsubsets);

	// ...and finally the memory that is the result...
	MCHEmul::Memory::Content result;
	result._physicalStorages = storages;
	result._subsets = allsubsets;
	result._views = MCHEmul::MemoryViews (
		{
			{ _CPU_VIEW, GlobalView },
		});

	return (result);
}
