#include <C264/Memory.hpp>

// ---
C264::Memory::Memory (unsigned int cfg, const std::string& lang)
	: MCHEmul::Memory (0, C264::Memory::standardMemoryContent (), { }),
	  _configuration (cfg)
{
	// In the content...
	if (error () != MCHEmul::_NOERROR)
		return;

	_basicROM			= subset (_BASICROM_SUBSET);
	_basicRAM			= subset (_BASICRAM_SUBSET);
	_kernelROM			= subset (_KERNELROM_SUBSET);
	_kernelRAM			= subset (_KERNELRAM_SUBSET);
	_RAM1				= subset (_RAM1_SUBSET);
	_RAM2				= subset (_RAM2_SUBSET);
	_RAM3				= subset (_RAM3_SUBSET);
	_RAM4				= subset (_RAM4_SUBSET);

	// The default ROMS...
	// They might change depending on the language
	// By default the English language is taken
	std::string ROMFILE = "./basic.318006-01.bin";
	std::string KERNELFILE = "./kernal.318004-05-ENG.bin";

	// If the languaje selected is not ENG...
	// then another char rom file is selected!
	// The CHARROM here is part of the Kernal
	if (lang == "FRA") { KERNELFILE = "kernal.318005-05-FRA.bin"; }
	else if (lang == "HUN") { KERNELFILE = "kernal.318030-02-HUN.bin"; }
	else if (lang == "SWE") { KERNELFILE = "kernel.325155-03-SWE.bin"; }

	bool ok = true;
	ok &= subset (_BASICROM_SUBSET) -> loadInto (ROMFILE);
	subset (_BASICROM_SUBSET) -> fixDefaultValues (); // Fix the values for further initializations...
	ok &= subset (_KERNELROM_SUBSET) -> loadInto (KERNELFILE);
	subset (_KERNELROM_SUBSET) -> fixDefaultValues ();

	if (!ok)
		_error = MCHEmul::_INIT_ERROR;

	// Sets the configuration of the memory that will depend on the type of machine...
	setConfiguration (_configuration);
}

// ---
void C264::Memory::setConfiguration (unsigned int cfg)
{
	// Attending to the type of machine different options are active or not active!
	switch (_configuration = cfg)
	{
		// 16k
		case 0:
			{
				_RAM1 -> setActive (true);
				_RAM2 -> setActive (false);
				_RAM3 -> setActive (false);
				_RAM4 -> setActive (false);
			}

			break;

		// 32k
		case 1:
			{
				_RAM1 -> setActive (true);
				_RAM2 -> setActive (true);
				_RAM3 -> setActive (false);
				_RAM4 -> setActive (false);
			}

			break;

		// 48k
		case 2:
			{
				_RAM1 -> setActive (true);
				_RAM2 -> setActive (true);
				_RAM3 -> setActive (true);
				_RAM4 -> setActive (false);
			}

			break;

		// 64k
		case 3:
			{
				_RAM1 -> setActive (true);
				_RAM2 -> setActive (true);
				_RAM3 -> setActive (true);
				_RAM4 -> setActive (true);
			}

			break;

		default:
			// It shouldn't exist, but just in case...
			assert (false);
			break;
	}
}

// ---
bool C264::Memory::initialize ()
{
	bool result = MCHEmul::Memory::initialize ();
	if (!result)
		return (false);

	// The active view has to be initially the CPU view...
	setCPUView ();

	return (true);
}

// ---
MCHEmul::Memory::Content C264::Memory::standardMemoryContent ()
{
	/** All dirs in Little - endian format. */

	// Phisical storages
	// RAM...
	MCHEmul::PhysicalStorage* RAM = 
		new MCHEmul::PhysicalStorage (_RAM, MCHEmul::PhysicalStorage::Type::_RAM, 0x4000);				// 64k
	// ROM...
	MCHEmul::PhysicalStorage* BASICROM = 
		new MCHEmul::PhysicalStorage (_BASICROM, MCHEmul::PhysicalStorage::Type::_ROM, 0x4000);			// 16k
	MCHEmul::PhysicalStorage* KERNELROM	= 
		new MCHEmul::PhysicalStorage (_KERNELROM, MCHEmul::PhysicalStorage::Type::_ROM, 0x4000);		// 16k

	// The map of phisical storages, used later...
	MCHEmul::PhysicalStorages storages (
		{
			{ _RAM, RAM },
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
	// Rest is RAM...
	MCHEmul::PhysicalStorageSubset* RAM1 = new MCHEmul::PhysicalStorageSubset 
		(_RAM1_SUBSET, RAM, 0x0200, MCHEmul::Address ({ 0x00, 0x02 }, false), 0x3e00);				// 16k - 512 bytes...
	MCHEmul::PhysicalStorageSubset* RAM2 = new MCHEmul::PhysicalStorageSubset 
		(_RAM2_SUBSET, RAM, 0x4000, MCHEmul::Address ({ 0x00, 0x40 }, false), 0x4000);				// 16k
	MCHEmul::PhysicalStorageSubset* RAM3 = new MCHEmul::PhysicalStorageSubset 
		(_RAM3_SUBSET, RAM, 0x8000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000);				// 16k
	MCHEmul::PhysicalStorageSubset* RAM4 = new MCHEmul::PhysicalStorageSubset 
		(_RAM4_SUBSET, RAM, 0xc000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x4000);				// 16k
	// And ROM at the end, with RAM behind 
	// that actives or not depending on the configuration of the machine...
	MCHEmul::PhysicalStorageSubset* BasicROM = new MCHEmul::PhysicalStorageSubset 
		(_BASICROM_SUBSET, BASICROM, 0x0000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000);		// 16k
	MCHEmul::PhysicalStorageSubset* BasicRAM = new MCHEmul::PhysicalStorageSubset 
		(_BASICRAM_SUBSET, RAM, 0x8000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000);			// 16k RAM behind...
	MCHEmul::PhysicalStorageSubset* KernelROM = new MCHEmul::PhysicalStorageSubset 
		(_KERNELROM_SUBSET, KERNELROM, 0x0000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x4000);	// 16k
	MCHEmul::PhysicalStorageSubset* KernelRAM = new MCHEmul::PhysicalStorageSubset 
		(_KERNELRAM_SUBSET, RAM, 0xc000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x4000);			// 16k RAM behind...

	// A map with all the subsets possible...
	MCHEmul::PhysicalStorageSubsets allsubsets (
		{
			{ _PAGEZERO_SUBSET,									PageZero }, 
			{ _STACK_SUBSET,									Stack },
			{ _RAM1_SUBSET,										RAM1 },
			{ _RAM2_SUBSET,										RAM2 },
			{ _RAM3_SUBSET,										RAM3 },
			{ _RAM4_SUBSET,										RAM4 },
			{ _BASICROM_SUBSET,									BasicROM },
			{ _BASICRAM_SUBSET,									BasicRAM },
			{ _KERNELROM_SUBSET,								KernelROM },
			{ _KERNELRAM_SUBSET,								KernelRAM }
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
