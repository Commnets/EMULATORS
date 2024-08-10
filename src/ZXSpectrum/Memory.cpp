#include <ZXSpectrum/Memory.hpp>

// ---
ZXSPECTRUM::Memory::Memory (ZXSPECTRUM::Memory::Configuration cfg, ZXSPECTRUM::Type t)
	: MCHEmul::Memory (0, ZXSPECTRUM::Memory::standardMemoryContent (), { }),
	  _type (t),
	  _configuration (cfg),
	  _STACK_SUBSET (0)
{
	// In the content...
	if (error () != MCHEmul::_NOERROR)
		return;

	// The ROM to load can be configurable...
	std::string ROMFILE = "./zx81_1.rom";
	switch (_type)
	{
		// TODO...
	}

	bool ok = true;
	ok &= physicalStorage (_ROM_SET) -> loadInto (ROMFILE);
	// TODO: Fiz the values in the ROM....

	if (!ok)
		_error = MCHEmul::_INIT_ERROR;

	// Gets a pointer to the main pieces of the memory that are activable...
	// TODO...

	// Sets the configuration of the memory...
	setConfiguration (_configuration);
}

// ---
void ZXSPECTRUM::Memory::setConfiguration (ZXSPECTRUM::Memory::Configuration cfg)
{
	// TODO
}

// ---
bool ZXSPECTRUM::Memory::initialize ()
{
	bool result = MCHEmul::Memory::initialize ();
	if (!result)
		return (false);

	// TODO

	// The active view has to be initially the CPU view...
	setCPUView ();

	return (true);
}

// ---
MCHEmul::Memory::Content ZXSPECTRUM::Memory::standardMemoryContent ()
{
	/** All dirs in Little - endian format. */

	// Phisical storages
	// The way the different parts are finally ROM or RAM will depend on the subsets defined later...
	MCHEmul::PhysicalStorage* RAM = 
		new MCHEmul::PhysicalStorage (_RAM_SET, MCHEmul::PhysicalStorage::Type::_RAM, 0x10000);				// 64k. All behind!
	MCHEmul::PhysicalStorage* ROM = 
		new MCHEmul::PhysicalStorage (_ROM_SET, MCHEmul::PhysicalStorage::Type::_ROM, 0x10000);				// 64k. All behind!

	// The map of phisical storages, used later...
	MCHEmul::PhysicalStorages storages (
		{
			{ _RAM_SET, RAM },
			{ _ROM_SET, ROM }
		});

	// Legend to understand subsets...
	// _UC = Unaccesible (Empty).
	// _S  = Shadow of a previous one
	// _V  = Video memory (it is shadow at the same time)
	// _CS = Memory than could be accesible when signals _CS (ROM_CS o RAM_CS) are off (@see setConfiguration method)

	// The memory is prepared by designed to be "splitted" in 16k blocks-size.

	// Subsets
	// TODO...

	// A map with all the subsets possible...
	MCHEmul::PhysicalStorageSubsets allsubsets (
		{
			// TODO
		});

	// Then the views...
	// The view from the CPU...
	MCHEmul::MemoryView* cpuView = new MCHEmul::MemoryView 
		(_CPU_VIEW, 
			{
				// TODO
			});

	// ...and the view from the ULA...
	MCHEmul::MemoryView* ulaView = new MCHEmul::MemoryView 
		(_ULA_VIEW, 
			{
				// TODO
			});

	// ...and finally the memory that is the result...
	MCHEmul::Memory::Content result;
	result._physicalStorages = storages;
	result._subsets = allsubsets;
	result._views = MCHEmul::MemoryViews (
		{
			{ _CPU_VIEW, cpuView},
			{ _ULA_VIEW, ulaView},
		});

	return (result);
}
