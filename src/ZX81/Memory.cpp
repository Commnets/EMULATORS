#include <ZX81/Memory.hpp>

// ---
ZX81::Memory::Memory (ZX81::Memory::Configuration cfg, unsigned int m)
	: MCHEmul::Memory (0, ZX81::Memory::standardMemoryContent (), { }),
	  _configuration (cfg),
	  _ROM (nullptr), 
	  _ROMSHADOW (nullptr),
	  _RAM1K (nullptr), 
	  _RAM1K_UC (nullptr),
	  _RAM16K (nullptr)
{
	// In the content...
	if (error () != MCHEmul::_NOERROR)
		return;

	// The ROM to load can be configurable...
	std::string ROMFILE = "./zx81_1.rom";
	switch (m)
	{
		case 0: 
			ROMFILE = "./zx80.rom"; 
			break;

		case 1: 
			ROMFILE = "./zx81_1.rom"; 
			break;

		case 2:
			ROMFILE = "./zx81_2.rom";
			break;

		case 3: 
			ROMFILE = "./zx81_3.rom"; 
			break;
	}

	bool ok = true;
	ok &= physicalStorage (_ROM_SET) -> loadInto (ROMFILE);
	subset (_ROM_SUBSET) -> fixDefaultValues (); // Fix the values for further initializations...

	if (!ok)
		_error = MCHEmul::_INIT_ERROR;

	// Gets a pointer to the main pieces of the memory that are activable...
	_ROM			= subset (_ROM_SUBSET);
	_ROMSHADOW		= static_cast <MCHEmul::MirrorPhysicalStorageSubset*> (subset (_ROMSHADOW_SUBSET));
	_RAM1K			= subset (_RAM1K_SUBSET);
	_RAM1K_UC		= static_cast <MCHEmul::EmptyPhysicalStorageSubset*> (subset (_RAM1K_UC_SUBSET));
	_RAM16K			= subset (_RAM16K_SUBSET);
	assert (_ROM != nullptr && 
			_ROMSHADOW != nullptr &&
			_RAM1K != nullptr && 
			_RAM1K_UC != nullptr &&
			_RAM16K != nullptr); // None of the can be nullptr...

	// Sets the configuration of the memory...
	setConfiguration (_configuration);
}

// ---
void ZX81::Memory::setConfiguration (ZX81::Memory::Configuration cfg)
{
	// Attending to the configuration different options are active or not active!
	switch (_configuration = cfg)
	{
		case ZX81::Memory::Configuration::_NOEXPANDED:
			{
				_RAM1K -> setActive (true);
				_RAM1K_UC -> setActive (true);
				_RAM16K -> setActive (false);
			}

			break;

		case ZX81::Memory::Configuration::_16KEXPANSION:
			{
				_RAM1K -> setActive (false);
				_RAM1K_UC -> setActive (false);
				_RAM16K -> setActive (true);
			}

			break;

		default:
			// It shouldn't exist, but just in case...
			assert (false);
			break;
	}
}

// ---
bool ZX81::Memory::initialize ()
{
	bool result = MCHEmul::Memory::initialize ();
	if (!result)
		return (false);

	// The active view has to be initially the CPU view...
	setCPUView ();

	return (true);
}

// ---
MCHEmul::Memory::Content ZX81::Memory::standardMemoryContent ()
{
	/** All dirs in Little - endian format. */

	// Phisical storages
	MCHEmul::PhysicalStorage* RAM = 
		new MCHEmul::PhysicalStorage (_RAM_SET, MCHEmul::PhysicalStorage::Type::_RAM, 0x10000);			// 64k
	MCHEmul::PhysicalStorage* ROM = 
		new MCHEmul::PhysicalStorage (_ROM_SET, MCHEmul::PhysicalStorage::Type::_ROM, 0x2000);			// 8k

	// The map of phisical storages, used later...
	MCHEmul::PhysicalStorages storages (
		{
			{ _RAM_SET, RAM },
			{ _ROM_SET, ROM }
		});

	// Subsets
	// Stack, Where the CPU stores info...
	// In a Z80 processor based machine, it can be the full RAM memory...
	MCHEmul::Stack* Stack = new MCHEmul::Stack 
		(_STACK_SUBSET, RAM, 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 0x1000);

	// The ROM
	MCHEmul::PhysicalStorageSubset* ROMS = new MCHEmul::PhysicalStorageSubset 
		(_ROM_SUBSET, ROM, 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 0x2000);							// 8k
	MCHEmul::MirrorPhysicalStorageSubset* ROMS_S = new MCHEmul::MirrorPhysicalStorageSubset
		(_ROMSHADOW_SUBSET, ROMS, MCHEmul::Address ({ 0x00, 0x20 }, false));									// It is a mirror of the previous one...

	// The RAM...
	// The basic one...
	MCHEmul::PhysicalStorageSubset* RAM1S = new MCHEmul::PhysicalStorageSubset 
		(_RAM1K_SUBSET, RAM, 0x4000, MCHEmul::Address ({ 0x00, 0x40 }, false), 0x0400);							// 1k
	// The rest is not accesible...
	MCHEmul::EmptyPhysicalStorageSubset* RAM1S_UC = new MCHEmul::EmptyPhysicalStorageSubset
		(_RAM1K_UC_SUBSET, MCHEmul::UByte::_0, RAM, 0x4400, MCHEmul::Address ({ 0x00, 0x44 }, false), 0x3c00);	// 15k
	// ---but both can be replaced by a module of 16K
	MCHEmul::PhysicalStorageSubset* RAM16S = new MCHEmul::PhysicalStorageSubset 
		(_RAM1K_SUBSET, RAM, 0x4000, MCHEmul::Address ({ 0x00, 0x40 }, false), 0x4000);							// 16k (When expansion)

	// A map with all the subsets possible...
	MCHEmul::PhysicalStorageSubsets allsubsets (
		{
			{ _STACK_SUBSET,										Stack }, 
			{ _ROM_SUBSET,											ROMS }, 
			{ _ROMSHADOW_SUBSET,									ROMS_S }, 
			{ _RAM1K_SUBSET,										RAM1S }, 
			{ _RAM1K_UC_SUBSET,										RAM1S_UC }, 
			{ _RAM16K_SUBSET,										RAM16S } 
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
