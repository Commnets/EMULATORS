#include <ZXSpectrum/Memory.hpp>

// ---
ZXSPECTRUM::Memory::Memory (ZXSPECTRUM::Memory::Configuration cfg, 
		ZXSPECTRUM::Type t, const std::string& lang)
	: MCHEmul::Memory (0, ZXSPECTRUM::Memory::standardMemoryContent (), { }),
	  _type (t),
	  _configuration (cfg),
	  _ROMBasic (nullptr),
	  _RAM16k (nullptr),
	  _RAM32k_E (nullptr), 
	  _RAM48k (nullptr),
	  _RAMULA (nullptr),
	  _STACK_SUBSET (_RAM16K_SUBSET) // Initially the version not expanded...
{
	// In the content...
	if (error () != MCHEmul::_NOERROR)
		return;

	// The ROM to load can be configurable...
	std::string ROMFILE = "./bios/48Standard-ENG.rom";
	if (lang == "ESP") ROMFILE = "./bios/48Standard-ESP.rom";
	else if (lang == "NOR") ROMFILE = "./bios/48Standard-NOR.rom";
	else if (lang == "ARA") ROMFILE = "./bios/48Standard-ARA.rom";

	bool ok = true;
	ok &= physicalStorage (_ROM_SET) -> loadInto (ROMFILE);
	subset (_ROM48KBASIC_SUBSET) -> fixDefaultValues ();

	if (!ok)
		_error = MCHEmul::_INIT_ERROR;

	// Gets a pointer to the main pieces of the memory that are activable...
	_ROMBasic = subset (_ROM48KBASIC_SUBSET);
	_RAM16k = dynamic_cast <MCHEmul::Stack*> (subset (_RAM16K_SUBSET));
	_RAM32k_E = dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> (subset (_RAM32KE_SUBSET));
	_RAM48k = dynamic_cast <MCHEmul::Stack*> (subset (_RAM48K_SUBSET));
	_RAMULA = subset (_RAMULA_SUBSET);

	// None of them can be null...
	assert (_ROMBasic != nullptr &&
			_RAM16k != nullptr &&
			_RAM32k_E != nullptr &&
			_RAM48k != nullptr &&
			_RAMULA != nullptr);

	// Sets the configuration of the memory...
	setConfiguration (_configuration);
}

// ---
void ZXSPECTRUM::Memory::setConfiguration (ZXSPECTRUM::Memory::Configuration cfg)
{
	// The ULA is always active...
	_RAMULA -> setActive (true);
	_RAMULA -> setActiveForReading (true);

	switch (_type)
	{
		// The very classic ZXSpectrum!...
		case ZXSPECTRUM::Type::_STANDARD:
			{
				switch (cfg)
				{
					case ZXSPECTRUM::Memory::Configuration::_16K:
						{
							_ROMBasic -> setActive (true);
							_RAM16k -> setActive (true);
							_RAM16k -> setActiveForReading (true);
							_RAM32k_E -> setActive (true);
							_RAM32k_E -> setActiveForReading (true);
							_RAM48k -> setActive (false);
							_RAM48k -> setActiveForReading (false);

							_STACK_SUBSET = _RAM16k -> id ();
							_stack = nullptr; // To reload the stack!
						}

						break;

					case ZXSPECTRUM::Memory::Configuration::_48K:
						{
							_ROMBasic -> setActive (true);
							_RAM16k -> setActive (false);
							_RAM16k -> setActiveForReading (false);
							_RAM32k_E -> setActive (false);
							_RAM32k_E -> setActiveForReading (false);
							_RAM48k -> setActive (true);
							_RAM48k -> setActiveForReading (true);

							_STACK_SUBSET = _RAM48k -> id ();
							_stack = nullptr; // To reaload the stack!
						}

						break;

					default:
						{ 
							// It shouldn't exist, but just in case...
							_LOG ("ZXSpectrum Configuration mode not supported:" +
								std::to_string ((int) _configuration));
							assert (false); // Just when running under debug mode...
						}

						break;
				}
			}

			break;

		default:
			{ 
				// It shouldn't exist, but just in case...
				_LOG ("ZXSpectrum Machine type not supported:" +
					std::to_string ((int) _type));
				assert (false); // Just when running under debug mode...
			}

			break;
	}
}

// ---
bool ZXSPECTRUM::Memory::initialize ()
{
	bool result = MCHEmul::Memory::initialize ();
	if (!result)
		return (false);

	// Initialize the RAM with random values...
	// ...as it is described mainñy in  the ZXSpectrum documentation...
	// The ROM is not affected at all!
	for (size_t i = 0; i < _RAM16k -> size (); i++)
		_RAM16k -> set (_RAM16k -> initialAddress () + i, std::rand () % 256);
	for (size_t i = 0; i < _RAM48k -> size (); i++)
		_RAM48k -> set (_RAM48k -> initialAddress () + i, std::rand () % 256);

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
	MCHEmul::PhysicalStorage* ROM = 
		new MCHEmul::PhysicalStorage (_ROM_SET, MCHEmul::PhysicalStorage::Type::_ROM, 0x10000);				// 64k, initially...
	MCHEmul::PhysicalStorage* RAM =
		new MCHEmul::PhysicalStorage (_RAM_SET, MCHEmul::PhysicalStorage::Type::_RAM, 0x10000);				// 64k, initally...

	// The map of phisical storages, used later...
	MCHEmul::PhysicalStorages storages (
		{
			{ _ROM_SET, ROM },
			{ _RAM_SET, RAM }
		});

	// ------
	// From the view of the CPU
	// Subsets
	// ROM: 
	// From 0x0000 to 0x3fff. 16k. In 16k and 48k versions.
	MCHEmul::PhysicalStorageSubset* ROMBasic = new MCHEmul::PhysicalStorageSubset
		(_ROM48KBASIC_SUBSET, ROM, 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 0x4000);
	// RAMS: 
	// 16K version: From 0x4000 to 0x7fff. 16k.
	MCHEmul::Stack* RAM16k = new MCHEmul::Stack
		(_RAM16K_SUBSET, RAM, 0x4000, MCHEmul::Address ({ 0x00, 0x40 }, false), 0x4000,
			MCHEmul::Stack::Configuration (true, false /** Pointing to the last written. */, 
				false /** No overflow detection. */, -1));
	MCHEmul::EmptyPhysicalStorageSubset* RAM32k_E = new MCHEmul::EmptyPhysicalStorageSubset
		(_RAM32KE_SUBSET, MCHEmul::UByte::_0, RAM, 0x8000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x8000); // When not expanded, the rest is empty...
	// 48k version: From 0x4000 to 0xffff. 32k.
	MCHEmul::Stack* RAM48k = new MCHEmul::Stack
		(_RAM48K_SUBSET, RAM, 0x4000, MCHEmul::Address ({ 0x00, 0x40 }, false), 0xc000,
			MCHEmul::Stack::Configuration (true, false /** Pointing to the last written. */, 
				false /** No overflow detection. */, -1));
	// ------

	ROMBasic -> setName ("ROM Basic");
	RAM16k -> setName ("RAM 16k");
	RAM32k_E -> setName ("RAM 32k not accesible");
	RAM48k -> setName ("RAM 48k");

	// The view from the CPU...
	MCHEmul::MemoryView* cpuView = new MCHEmul::MemoryView 
		(_CPU_VIEW, 
			{
				{ _ROM48KBASIC_SUBSET, ROMBasic },
				{ _RAM16K_SUBSET, RAM16k },
				{ _RAM32KE_SUBSET, RAM32k_E },
				{ _RAM48K_SUBSET, RAM48k }
			});

	// ------
	// The same info from the ULA perspetive...
	// The ULA only sees part of the RAM, and from the initial position...
	MCHEmul::PhysicalStorageSubset* RAMULA =new MCHEmul::PhysicalStorageSubset 
		(_RAMULA_SUBSET, RAM, 0x4000, MCHEmul::Address ({ 0x00, 0x00 }, false), 
			0x1b00 /** $1800 screen memory + $0300 color data. */);
	// ------

	// ...and the view from the ULA...
	MCHEmul::MemoryView* ulaView = new MCHEmul::MemoryView 
		(_ULA_VIEW, 
			{
				{ _RAMULA_SUBSET, RAMULA }
			});

	// A map with all the subsets possible...
	MCHEmul::PhysicalStorageSubsets allsubsets (
		{
			{ _ROM48KBASIC_SUBSET, ROMBasic },
			{ _RAM16K_SUBSET, RAM16k },
			{ _RAM32KE_SUBSET, RAM32k_E },
			{ _RAM48K_SUBSET, RAM48k },
			{ _RAMULA_SUBSET, RAMULA }
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
