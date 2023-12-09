#include <VIC20/Memory.hpp>

// ---
VIC20::Memory::Memory (const std::string& lang)
	: MCHEmul::Memory (VIC20::Memory::standardMemoryContent ())
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
	if (!ok)
		_error = MCHEmul::_INIT_ERROR;
}

// ---
bool VIC20::Memory::initialize ()
{
	bool result = MCHEmul::Memory::initialize ();
	if (!result)
		return (false);

	// The active view has to be initially the CPU vire...
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

	// ...and finally the memory that is the result...
	MCHEmul::Memory::Content result;
	result._physicalStorages = storages;
//	result._subsets = allsubsets;
//	result._views = MCHEmul::MemoryViews (
//		{
//			{ _CPU_VIEW, CPUView },
//			{ _VICII_VIEW, VICIIView }
//		});

	return (result);
}
