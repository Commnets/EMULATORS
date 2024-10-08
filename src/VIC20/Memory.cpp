#include <VIC20/Memory.hpp>
#include <VIC20/VIA1Registers.hpp>
#include <VIC20/VIA2Registers.hpp>
#include <COMMODORE/VICI/VICIRegisters.hpp>

// ---
void VIC20::VIA1VIA2RegistersReflection::setValue (size_t nB, const MCHEmul::UByte& d)
{
	// The structure repeats every 0x40 = 64 bytes...
	size_t b  = nB % 0x40; // From 0 to 0x3f
	size_t c  = b / 0x10;  // From 0 to 3...It defines the block within the structure...
	size_t pp = c % 0x10;  // Which byte inside the block...

	switch (c)
	{
		case 0:
		case 2:
			{
				_via1Registers -> setValue (pp, d);
			}

			break;

		case 3:
			{
				_via2Registers -> setValue (pp, d);
			}

			break;

		case 1:
			// Does nothing...
			break;

		default:
			// It shouldn't be here...
			assert (false);
			break;
	}
}

// ---
const MCHEmul::UByte& VIC20::VIA1VIA2RegistersReflection::readValue (size_t nB) const
{
	MCHEmul::UByte result = MCHEmul::UByte::_0;

	// The structure repeats every 0x40 = 64 bytes...
	size_t b  = nB % 0x40; // From 0 to 0x3f
	size_t c  = b / 0x10;  // From 0 to 3...It defines the block within the structure...
	size_t pp = c % 0x10;  // Which byte inside the block...

	switch (c)
	{
		case 0:
		case 2:
			{
				result = _via1Registers -> readValue (pp);
			}

			break;

		case 3:
			{
				result = _via2Registers -> readValue (pp);
			}

			break;

		case 1:
			// Does nothing...
			break;

		default:
			// It shouldn't be here...
			assert (false);
			break;
	}

	return (_lastValue = result);
}

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

	// The info is load into the physical storage directly, 
	// but fixed into the subset above!

	bool ok = true;
	ok &= physicalStorage (_BASICROM) -> loadInto (ROMFILE);
	subset (_BASICROM_SUBSET) -> fixDefaultValues (); // Fix the values for further initializations...
	ok &= physicalStorage (_CHARROM) -> loadInto (CHARROMFILE);
	subset (_CHARROM_SUBSET) -> fixDefaultValues ();
	ok &= physicalStorage (_KERNELROM) -> loadInto (KERNELFILE);
	subset (_KERNELROM_SUBSET) -> fixDefaultValues ();

	if (!ok)
		_error = MCHEmul::_INIT_ERROR;

	// Gets a pointer to the main pieces of the memory that are activable...
	_BANK0			= subset (_BANK0_SUBSET);
	_BANK0UC		= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> (subset (_BANK0UC_SUBSET));
	_BANK1			= subset (_BANK1_SUBSET);
	_BANK1UC		= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> (subset (_BANK1UC_SUBSET));
	_BANK2			= subset (_BANK2_SUBSET);
	_BANK2UC		= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> (subset (_BANK2UC_SUBSET));
	_BANK3			= subset (_BANK3_SUBSET);
	_BANK3UC		= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> (subset (_BANK3UC_SUBSET));
	_SCREENC1RAM	= subset (_SCREENC1RAM_SUBSET);
	_SCREENC1RAMUC	= dynamic_cast <MCHEmul::RandomPhysicalStorageSubset*> (subset (_SCREENC1RAMUC_SUBSET));
	_SCREENC2RAM	= subset (_SCREENC2RAM_SUBSET);
	_SCREENC2RAMUC	= dynamic_cast <MCHEmul::RandomPhysicalStorageSubset*> (subset (_SCREENC2RAMUC_SUBSET));
	_BANK5			= subset (_BANK5_SUBSET);
	_BANK5UC		= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> (subset (_BANK5UC_SUBSET));
	assert (_BANK0 != nullptr && _BANK0UC != nullptr &&
			_BANK1 != nullptr && _BANK1UC != nullptr &&
			_BANK2 != nullptr && _BANK2UC != nullptr &&
			_BANK3 != nullptr && _BANK3UC != nullptr &&
			_SCREENC1RAM != nullptr && _SCREENC1RAMUC != nullptr &&
			_SCREENC2RAM != nullptr && _SCREENC2RAMUC != nullptr &&
			_BANK5 != nullptr && _BANK5UC != nullptr); // None of the can be nullptr...

	// Sets the configuration of the memory...
	setConfiguration (_configuration);
}

// ---
void VIC20::Memory::setConfiguration (VIC20::Memory::Configuration cfg, bool a0)
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
				_BANK5			-> setActive (a0);
				_BANK5UC		-> setActive (!a0);

				// The screen memory is in the second possible position...
				_SCREENC1RAM	-> setActive (false);
				_SCREENC1RAMUC	-> setActive (true);
				_SCREENC2RAM	-> setActive (true);
				_SCREENC2RAMUC  -> setActive (false);
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
				_BANK5			-> setActive (a0);
				_BANK5UC		-> setActive (!a0);

				// The screen memory is in the second possible position...
				_SCREENC1RAM	-> setActive (false);
				_SCREENC1RAMUC	-> setActive (true);
				_SCREENC2RAM	-> setActive (true);
				_SCREENC2RAMUC  -> setActive (false);
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
				_BANK5			-> setActive (a0);
				_BANK5UC		-> setActive (!a0);

				// The screen memory is in the first possible position...
				_SCREENC1RAM	-> setActive (true);
				_SCREENC1RAMUC	-> setActive (false);
				_SCREENC2RAM	-> setActive (false);
				_SCREENC2RAMUC  -> setActive (true);
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
				_BANK5			-> setActive (a0);
				_BANK5UC		-> setActive (!a0);

				// The screen memory is in the first possible position...
				_SCREENC1RAM	-> setActive (true);
				_SCREENC1RAMUC	-> setActive (false);
				_SCREENC2RAM	-> setActive (false);
				_SCREENC2RAMUC  -> setActive (true);
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

				_BANK5			-> setActive (a0);
				_BANK5UC		-> setActive (!a0);

				// The screen memory is in the first possible position...
				_SCREENC1RAM	-> setActive (true);
				_SCREENC1RAMUC	-> setActive (false);
				_SCREENC2RAM	-> setActive (false);
				_SCREENC2RAMUC  -> setActive (true);
			}

			break;

		// The BANK5 is usually active when autorun cartridges 8k are introduced...
		// so in that c ases the a0 parameter will be true when started!

		default:
			// It shouldn't exist, but just in case...
			assert (false);
			break;
	}
}

// ---
void VIC20::Memory::setConfiguration (unsigned char cfg)
{
	_BANK0	 -> setActive (true);					// 3k min always...
	_BANK0UC -> setActive (false);

	// Active the banks if the are requested...
	_BANK1	 -> setActive ((cfg & 0x02) != 0x00);
	_BANK1UC -> setActive ((cfg & 0x02) == 0x00);
	_BANK2	 -> setActive ((cfg & 0x04) != 0x00);
	_BANK2UC -> setActive ((cfg & 0x04) == 0x00);
	_BANK3	 -> setActive ((cfg & 0x08) != 0x00);
	_BANK3UC -> setActive ((cfg & 0x08) == 0x00);
	_BANK5	 -> setActive ((cfg & 0x20) != 0x00);
	_BANK5UC -> setActive ((cfg & 0x20) == 0x00);

	// The screen memory will be in different place according with the banks active...
	// In the moment there is some king of expansion, the screen memory changes its position!
	if (_BANK1 -> active () ||
		_BANK2 -> active () ||
		_BANK3 -> active () ||
		_BANK5 -> active ())
	{
		_SCREENC1RAM	-> setActive (true);
		_SCREENC1RAMUC	-> setActive (false);
		_SCREENC2RAM	-> setActive (false);
		_SCREENC2RAMUC  -> setActive (true);
	}
	else
	{
		_SCREENC1RAM	-> setActive (false);
		_SCREENC1RAMUC	-> setActive (true);
		_SCREENC2RAM	-> setActive (true);
		_SCREENC2RAMUC  -> setActive (false);
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
	MCHEmul::EmptyPhysicalStorageSubset* BANK0_UC = new MCHEmul::EmptyPhysicalStorageSubset
		(_BANK0UC_SUBSET, MCHEmul::UByte (0x04), RAM, 0x0400, MCHEmul::Address ({ 0x00, 0x04 }, false), 0x0c00);

	// Very BASIC RAM; Usually where the BASIC area is...and also the Screen RAM in not expanded systems (or 3k)
	MCHEmul::PhysicalStorageSubset* BASICAREA = new MCHEmul::PhysicalStorageSubset 
		(_BASICAREA_SUBSET, RAM, 0x1000, MCHEmul::Address ({ 0x00, 0x10 }, false), 0x1000);
	
	// Block 1 of RAM. Expansion: 8 k
	MCHEmul::PhysicalStorageSubset* BANK1 = new MCHEmul::PhysicalStorageSubset 
		(_BANK1_SUBSET, RAM, 0x2000, MCHEmul::Address ({ 0x00, 0x20 }, false), 0x2000); // The connected one...
	MCHEmul::EmptyPhysicalStorageSubset* BANK1_UC = new MCHEmul::EmptyPhysicalStorageSubset
		(_BANK1UC_SUBSET, MCHEmul::UByte (0x20), RAM, 0x2000, MCHEmul::Address ({ 0x00, 0x20 }, false), 0x2000); // The not connected...
	
	// Block 2 of RAM. Expansion: 8 K
	MCHEmul::PhysicalStorageSubset* BANK2 = new MCHEmul::PhysicalStorageSubset 
		(_BANK2_SUBSET, RAM, 0x4000, MCHEmul::Address ({ 0x00, 0x40 }, false), 0x2000); // The connected one...
	MCHEmul::EmptyPhysicalStorageSubset* BANK2_UC = new MCHEmul::EmptyPhysicalStorageSubset
		(_BANK2UC_SUBSET, MCHEmul::UByte (0x40), RAM, 0x4000, MCHEmul::Address ({ 0x00, 0x40 }, false), 0x2000); // The not connected...
	
	// Block 3 of RAM. Expansion: 8 k
	MCHEmul::PhysicalStorageSubset* BANK3 = new MCHEmul::PhysicalStorageSubset 
		(_BANK3_SUBSET, RAM, 0x6000, MCHEmul::Address ({ 0x00, 0x60 }, false), 0x2000); // The connected one...
	MCHEmul::EmptyPhysicalStorageSubset* BANK3_UC = new MCHEmul::EmptyPhysicalStorageSubset
		(_BANK3UC_SUBSET, MCHEmul::UByte (0x60), RAM, 0x6000, MCHEmul::Address ({ 0x00, 0x60 }, false), 0x2000); // The not connected...
	
	// Char ROM
	MCHEmul::PhysicalStorageSubset* CharROM = new MCHEmul::PhysicalStorageSubset
		(_CHARROM_SUBSET, CHARROM, 0x0000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x1000);
	
	// VICI, VIA1 and VIA2...
	// Each of them has only 16 accesible registers, at the end of which there is a free memory that seem to mirror
	// part of the registers of the chip, but only when poking and not when peeking (random value)...
	MCHEmul::PhysicalStorageSubset* VICIRegisters = new COMMODORE::VICIRegisters
		(/** id = COMMODORE::VICIRegisters::_VICREGS_SUBSET */ RAM, 0x9000, MCHEmul::Address ({ 0x00, 0x90 }, false), 0x100);
	MCHEmul::RandomPhysicalStorageSubset* VICIRegisters_After = new MCHEmul::RandomPhysicalStorageSubset
		(_VICIRAFTER_SUBSET, RAM, 0x9100, MCHEmul::Address ({ 0x00, 0x91 }, false), 0x0010); // The values are returned random...
	VIC20::VIA1Registers* VIA1Registers = new VIC20::VIA1Registers
		(/** id = VIC20::VIA1Registers::_VIA1_SUBSET */ RAM, 0x9110, MCHEmul::Address ({ 0x10, 0x91 }, false), 0x010);
	VIC20::VIA2Registers* VIA2Registers = new VIC20::VIA2Registers
		(/** id = VIC20::VIA2Registers::_VIA2_SUBSET */ RAM, 0x9120, MCHEmul::Address ({ 0x20, 0x91 }, false), 0x010);
	VIC20::VIA1VIA2RegistersReflection* VIARegisters_After = new VIC20::VIA1VIA2RegistersReflection 
		(_VIAAFTER_SUBSET, VIA1Registers, VIA2Registers, RAM, 0x9130, MCHEmul::Address ({ 0x30, 0x91 }, false), 0x02d0); // Strange behaviour...

	// Screen Color RAM, can be in several locations depending on the configuration...
	MCHEmul::PhysicalStorageSubset* SCREENC1RAM = new MCHEmul::PhysicalStorageSubset 
		(_SCREENC1RAM_SUBSET, RAM, 0x9400, MCHEmul::Address ({ 0x00, 0x94 }, false), 0x0200);
	MCHEmul::RandomPhysicalStorageSubset* SCREENC1RAM_UC = new MCHEmul::RandomPhysicalStorageSubset
		(_SCREENC1RAMUC_SUBSET, RAM, 0x9400, MCHEmul::Address ({ 0x00, 0x94 }, false), 0x0200); // The values are returned random...
	MCHEmul::PhysicalStorageSubset* SCREENC2RAM = new MCHEmul::PhysicalStorageSubset 
		(_SCREENC2RAM_SUBSET, RAM, 0x9600, MCHEmul::Address ({ 0x00, 0x96 }, false), 0x0200);
	MCHEmul::RandomPhysicalStorageSubset* SCREENC2RAM_UC = new MCHEmul::RandomPhysicalStorageSubset
		(_SCREENC2RAMUC_SUBSET, RAM, 0x9600, MCHEmul::Address ({ 0x00, 0x96 }, false), 0x0200); // The values are returned random...

	// Block 4 of RAM (2k), initially useful for I/O...
	// This piece is officially subdivided in two blocks of 1024 (0x0400) bytes each...
	MCHEmul::PhysicalStorageSubset* BANK4 = new MCHEmul::PhysicalStorageSubset 
		(_BANK4_SUBSET, RAM, 0x9800, MCHEmul::Address ({ 0x00, 0x98 }, false), 0x0800);
	
	// Block 5 of RAM. Expansion: 8 k.
	// Cartridges usually use this space to start...
	MCHEmul::PhysicalStorageSubset* BANK5 = new MCHEmul::PhysicalStorageSubset 
		(_BANK5_SUBSET, RAM, 0xa000, MCHEmul::Address ({ 0x00, 0xa0 }, false), 0x2000); // The connected one...
	MCHEmul::EmptyPhysicalStorageSubset* BANK5_UC = new MCHEmul::EmptyPhysicalStorageSubset 
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
			{ _SCREENC1RAM_SUBSET,									SCREENC1RAM }, 
			{ _SCREENC1RAMUC_SUBSET,								SCREENC1RAM_UC }, 
			{ _SCREENC2RAM_SUBSET,									SCREENC2RAM }, 
			{ _SCREENC2RAMUC_SUBSET,								SCREENC2RAM_UC }, 
			{ _BANK4_SUBSET,										BANK4 }, 
			{ _BANK5_SUBSET,										BANK5 }, 
			{ _BANK5UC_SUBSET,										BANK5_UC }, 
			{ _BASICROM_SUBSET,										BasicROM }, 
			{ _KERNELROM_SUBSET,									KernelROM } 
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
