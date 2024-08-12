#include <ZXSpectrum/Memory.hpp>

// ---
ZXSPECTRUM::Memory::Memory (ZXSPECTRUM::Memory::Configuration cfg, 
		ZXSPECTRUM::Type t, const std::string& lang)
	: MCHEmul::Memory (0, ZXSPECTRUM::Memory::standardMemoryContent (), { }),
	  _type (t),
	  _configuration (cfg),
	  _ROM (nullptr),
	  _RAMBANK5 (nullptr),
	  _RAMBANK2 (nullptr), 
	  _RAMBANK2_E (nullptr),
	  _RAMBANK0 (nullptr), 
	  _RAMBANK0_E (nullptr),
	  _STACK_SUBSET (0)
{
	// In the content...
	if (error () != MCHEmul::_NOERROR)
		return;

	// The ROM to load can be configurable...
	std::string ROMFILE = "./48Standard-ENG.rom";
	if (lang == "ESP")
		ROMFILE = "./48Standard-ESP.rom";

	bool ok = true;
	ok &= physicalStorage (_ROM48KBASIC_SET) -> loadInto (ROMFILE);
	subset (_ROM48KBASIC_SUBSET) -> fixDefaultValues ();

	if (!ok)
		_error = MCHEmul::_INIT_ERROR;

	// Gets a pointer to the main pieces of the memory that are activable...
	_ROM = subset (_ROM48KBASIC_SUBSET);
	_RAMBANK5 = subset (_RAMBANK5_SUBSET);
	_RAMBANK2 = subset (_RAMBANK2_SUBSET);
	_RAMBANK2_E = dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> (subset (_RAMBANK2_SUBSET_E));
	_RAMBANK0 = subset (_RAMBANK0_SUBSET);
	_RAMBANK0_E = dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> (subset (_RAMBANK0_SUBSET_E));

	// None of them can be null...
	assert (_ROM != nullptr &&
			_RAMBANK5 != nullptr &&
			_RAMBANK2 != nullptr &&
			_RAMBANK2_E != nullptr &&
			_RAMBANK0 != nullptr &&
			_RAMBANK0_E != nullptr);

	// Sets the configuration of the memory...
	setConfiguration (_configuration);
}

// ---
void ZXSPECTRUM::Memory::setConfiguration (ZXSPECTRUM::Memory::Configuration cfg)
{
	switch (_type)
	{
		// The very classic ZXSpectrum!...
		case ZXSPECTRUM::Type::_STANDARD:
			{
				switch (cfg)
				{
					case ZXSPECTRUM::Memory::Configuration::_16K:
						{
							_ROM -> setActive (true);
							_RAMBANK5 -> setActive (true);
							_RAMBANK5 -> setActiveForReading (true);
							_RAMBANK2 -> setActive (false);
							_RAMBANK2 -> setActiveForReading (false);
							_RAMBANK2_E -> setActive (true);
							_RAMBANK2_E -> setActiveForReading (true);
							_RAMBANK0 -> setActive (false);
							_RAMBANK0 -> setActiveForReading (false);
							_RAMBANK0_E -> setActive (true);
							_RAMBANK0_E -> setActiveForReading (true);
						}

						break;

					case ZXSPECTRUM::Memory::Configuration::_48K:
						{
							_ROM -> setActive (true);
							_RAMBANK5 -> setActive (true);
							_RAMBANK5 -> setActiveForReading (true);
							_RAMBANK2 -> setActive (true);
							_RAMBANK2 -> setActiveForReading (true);
							_RAMBANK2_E -> setActive (false);
							_RAMBANK2_E -> setActiveForReading (false);
							_RAMBANK0 -> setActive (true);
							_RAMBANK0 -> setActiveForReading (true);
							_RAMBANK0_E -> setActive (false);
							_RAMBANK0_E -> setActiveForReading (false);
						}

						break;

					default:
						assert (false); // If shouldn't be here...
						break;

				}
			}

			break;

		default:
			assert (false); // It should be here...
			break;
	}
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
	MCHEmul::PhysicalStorage* ROM48kBasic = 
		new MCHEmul::PhysicalStorage (_ROM48KBASIC_SET, MCHEmul::PhysicalStorage::Type::_ROM, 0x4000);		// 16k.
	MCHEmul::PhysicalStorage* RAMB5 =
		new MCHEmul::PhysicalStorage (_RAMBANK5_SET, MCHEmul::PhysicalStorage::Type::_RAM, 0x4000);			// 16k.
	MCHEmul::PhysicalStorage* RAMB2 =
		new MCHEmul::PhysicalStorage (_RAMBANK2_SET, MCHEmul::PhysicalStorage::Type::_RAM, 0x4000);			// 16k.
	MCHEmul::PhysicalStorage* RAMB0 =
		new MCHEmul::PhysicalStorage (_RAMBANK0_SET, MCHEmul::PhysicalStorage::Type::_RAM, 0x4000);			// 16k.

	// The map of phisical storages, used later...
	MCHEmul::PhysicalStorages storages (
		{
			{ _ROM48KBASIC_SET, ROM48kBasic },
			{ _RAMBANK5_SET, RAMB5 },
			{ _RAMBANK2_SET, RAMB2 },
			{ _RAMBANK0_SET, RAMB0 },
		});

	// Subsets
	// ROM: 
	// From 0x0000 to 0x3fff. 16k. In 16k and 48k versions.
	MCHEmul::PhysicalStorageSubset* ROMS = new MCHEmul::PhysicalStorageSubset
		(_ROM48KBASIC_SUBSET, ROM48kBasic, 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 0x4000);
	// RAMS: 
	// From 0x4000 to 0x7fff. 16k. In 16k and 48k versions.
	MCHEmul::PhysicalStorageSubset* RAMB5S = new MCHEmul::PhysicalStorageSubset
		(_RAMBANK5_SUBSET, RAMB5, 0x0000, MCHEmul::Address ({ 0x00, 0x40 }, false), 0x4000);
	// From 0x8000 to 0xbfff. 16k. In 48k version only.
	MCHEmul::PhysicalStorageSubset* RAMB2S = new MCHEmul::PhysicalStorageSubset
		(_RAMBANK2_SUBSET, RAMB2, 0x0000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000);
	MCHEmul::EmptyPhysicalStorageSubset* RAMB2S_E = new MCHEmul::EmptyPhysicalStorageSubset
		(_RAMBANK2_SUBSET_E, MCHEmul::UByte::_0, RAMB2, 0x0000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000); // When not expanded...
	// From 0xc000 to 0xffff. 16k. In 48k version only.
	MCHEmul::PhysicalStorageSubset* RAMB0S = new MCHEmul::PhysicalStorageSubset
		(_RAMBANK0_SUBSET, RAMB0, 0x0000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x4000);
	MCHEmul::EmptyPhysicalStorageSubset* RAMB0S_E = new MCHEmul::EmptyPhysicalStorageSubset
		(_RAMBANK0_SUBSET_E, MCHEmul::UByte::_0, RAMB0, 0x0000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000); // When not expanded...

	// A map with all the subsets possible...
	MCHEmul::PhysicalStorageSubsets allsubsets (
		{
			{ _ROM48KBASIC_SUBSET, ROMS },
			{ _RAMBANK5_SUBSET, RAMB5S },
			{ _RAMBANK2_SUBSET, RAMB2S },
			{ _RAMBANK2_SUBSET_E, RAMB2S_E },
			{ _RAMBANK0_SUBSET, RAMB0S },
			{ _RAMBANK0_SUBSET_E, RAMB0S_E }
		});

	// Then the views...
	// The view from the CPU...
	MCHEmul::MemoryView* cpuView = new MCHEmul::MemoryView 
		(_CPU_VIEW, 
			{
				{ _ROM48KBASIC_SUBSET, ROMS },
				{ _RAMBANK5_SUBSET, RAMB5S },
				{ _RAMBANK2_SUBSET, RAMB2S },
				{ _RAMBANK2_SUBSET_E, RAMB2S_E },
				{ _RAMBANK0_SUBSET, RAMB0S },
				{ _RAMBANK0_SUBSET_E, RAMB0S_E }
			});

	// ...and the view from the ULA...
	MCHEmul::MemoryView* ulaView = new MCHEmul::MemoryView 
		(_ULA_VIEW, 
			{
				{ _ROM48KBASIC_SUBSET, ROMS },
				{ _RAMBANK5_SUBSET, RAMB5S },
				{ _RAMBANK2_SUBSET, RAMB2S },
				{ _RAMBANK2_SUBSET_E, RAMB2S_E },
				{ _RAMBANK0_SUBSET, RAMB0S },
				{ _RAMBANK0_SUBSET_E, RAMB0S_E }
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
