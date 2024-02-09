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
	_charROM			= subset (_CHARROM_SUBSET);
	_charRAM			= subset (_CHARRAM_SUBSET);
	// TODO

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

	// Sets the configuration of the memory...
	setConfiguration (_configuration);
}

// ---
void C264::Memory::setConfiguration (unsigned int cfg, bool a0)
{
	// Attending to the configuration different options are active or not active!
	switch (_configuration = cfg)
	{
		// TODO...

		default:
			// It shouldn't exist, but just in case...
			assert (false);
			break;
	}
}

// ---
void C264::Memory::setConfiguration (unsigned char cfg)
{
	// TODO
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

	// TODO

	// A map with all the subsets possible...
	MCHEmul::PhysicalStorageSubsets allsubsets (
		{
			{ _PAGEZERO_SUBSET,										PageZero }, 
			{ _STACK_SUBSET,										Stack }

			// TODO
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
