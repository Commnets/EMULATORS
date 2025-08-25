#include <ZX81/Memory.hpp>
#include <ZX81/ULA.hpp>

MCHEmul::CPU* ZX81::MemoryVideoCode::_cpu = nullptr;
ZX81::ULA* ZX81::MemoryVideoCode::_ula = nullptr;

// ---
const MCHEmul::UByte& ZX81::MemoryVideoCode::readValue (size_t nB) const
{ 
	_lastValueRead = MCHEmul::MirrorPhysicalStorageSubset::readValue (nB);
	
	if (_cpu != nullptr && 
		_cpu -> programCounter ().internalRepresentation () >= 0xc000 &&
		!_lastValueRead.bit (6)) // When the counter is above 0xc000 and the info read has not the bit 6 set, 
								 // it is transalated into a HALT
	{
		// The value is passed to the ULA...
		_ula -> readCharData (_cpu, _lastValueRead);

		_lastValueRead = MCHEmul::UByte::_0;
	}

	return (_lastValueRead);
} 

// ---
ZX81::Memory::Memory (ZX81::Memory::Configuration cfg, ZX81::Type t)
	: MCHEmul::Memory (0, ZX81::Memory::standardMemoryContent (t), { }),
	  _type (t),
	  _configuration (cfg),
	  _ROM (nullptr), 
	  _ROM_S1 (nullptr),
	  _ROMCS1 (nullptr), 
	  _RAM1K (nullptr),
	  _RAM1K_S (nullptr),
	  _RAM1K_V (nullptr), 
	  _RAM15K_UC (),
	  _RAM15K_UC_S (),
	  _RAM15K_V (),
	  _RAM16K_CS1 (nullptr),
	  _RAM16K_S (nullptr),
	  _RAM16K_V (nullptr),
	  _ROM_S2 (nullptr),
	  _ROM_S3 (nullptr),
	  _ROMCS2 (nullptr), 
	  _STACK_SUBSET (0)
{
	// In the content...
	if (error () != MCHEmul::_NOERROR)
		return;

	// The ROM to load can be configurable...
	std::string ROMFILE = "./bios/zx81_1.rom";
	switch (_type)
	{
		case ZX81::Type::_ZX80: 
			ROMFILE = "./bios/zx80.rom"; 
			break;

		case ZX81::Type::_ZX811: 
			ROMFILE = "./bios/zx81_1.rom"; 
			break;

		case ZX81::Type::_ZX812:
			ROMFILE = "./bios/zx81_2.rom";
			break;

		case ZX81::Type::_ZX813: 
			ROMFILE = "./bios/zx81_3.rom"; 
			break;
	}

	bool ok = true;
	ok &= physicalStorage (_ROM_SET) -> loadInto (ROMFILE);
	// In a ZX80 the ROM it is only 4 k, but it shadowed in the same why later...
	if (t == ZX81::Type::_ZX80) physicalStorage (_ROM_SET) -> loadInto (ROMFILE, 0x1000); 
	subset (_ROM_SUBSET) -> fixDefaultValues (); // Fix the values for further initializations...
	subset (_ROMSHADOW1_SUBSET) -> fixDefaultValues ();
	subset (_ROMSHADOW2_SUBSET) -> fixDefaultValues ();
	subset (_ROMSHADOW3_SUBSET) -> fixDefaultValues ();

	if (!ok)
		_error = MCHEmul::_INIT_ERROR;

	// Gets a pointer to the main pieces of the memory that are activable...
	// Block 1: ROM
	_ROM				= subset (_ROM_SUBSET);
	_ROM_S1				= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> (subset (_ROMSHADOW1_SUBSET));
	_ROMCS1				= subset (_ROMCS1_SUBSET);
	// Block 2/4: RAM and Video RAM
	_RAM1K				= dynamic_cast <MCHEmul::Stack*> (subset (_RAM1K_SUBSET)); // 2nd
	_RAM1K_S			= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> (subset (_RAM1KSHADOW_SUBSET)); // 4th from ULA
	_RAM1K_V			= dynamic_cast <ZX81::MemoryVideoCode*> (subset (_RAM1K_V_SUBSET)); // 4th from CPU
	for (size_t i = 1; i <= 15; i++)
		_RAM15K_UC.emplace_back (dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> (subset (_RAM1K_SUBSET + (int) i))); // 2nd
	for (size_t i = 1; i <= 15; i++)
		_RAM15K_UC_S.emplace_back (dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> (subset (_RAM1KSHADOW_SUBSET + (int) i))); // 4th from ULA
	for (size_t i = 1; i <= 15; i++)
		_RAM15K_V.emplace_back (dynamic_cast <ZX81::MemoryVideoCode*> (subset (_RAM1K_V_SUBSET + (int) i))); // 4th from CPU
	_RAM16K_CS1			= dynamic_cast <MCHEmul::Stack*> (subset (_RAM16KCS1_SUBSET)); // 2th RAM_CS = 0
	_RAM16K_S			= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> (subset (_RAM16KSHADOW_SUBSET)); // 4th RAM_CS = 0 from ULA
	_RAM16K_V			= dynamic_cast <ZX81::MemoryVideoCode*> (subset (_RAM16K_V_SUBSET)); // 4th RAM_CS = 0 from CPU
	// Block 3: ROM
	_ROM_S2				= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> (subset (_ROMSHADOW2_SUBSET));
	_ROM_S3				= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> (subset (_ROMSHADOW3_SUBSET));
	_ROMCS2				= subset (_ROMCS2_SUBSET);
	assert (
		_ROM			!= nullptr &&
		_ROM_S1			!= nullptr &&
		_ROMCS1			!= nullptr &&
		_RAM1K			!= nullptr &&
		_RAM1K_S		!= nullptr &&
		_RAM1K_V		!= nullptr &&
		std::find (_RAM15K_UC.begin (), _RAM15K_UC.end (), nullptr) == _RAM15K_UC.end () &&
		std::find (_RAM15K_UC_S.begin (), _RAM15K_UC_S.end (), nullptr) == _RAM15K_UC_S.end () &&
		std::find (_RAM15K_V.begin (), _RAM15K_V.end (), nullptr) == _RAM15K_V.end () &&
		_RAM16K_CS1		!= nullptr &&
		_RAM16K_S		!= nullptr &&
		_RAM16K_V		!= nullptr &&
		_ROM_S2			!= nullptr &&
		_ROM_S3			!= nullptr &&
		_ROMCS2			!= nullptr);

	// Sets the configuration of the memory...
	setConfiguration (_configuration, _type);
}

// ---
void ZX81::Memory::setConfiguration (ZX81::Memory::Configuration cfg, ZX81::Type t)
{
	// In any of the configurations, 
	// the stack will behave as already used from the early beginning...
	_RAM1K -> setNotUsed (false);
	_RAM16K_CS1 -> setNotUsed (false);

	// Attending to the configuration different options are active or not active!
	switch (_configuration = cfg)
	{
		case ZX81::Memory::Configuration::_NOEXPANDED:
			{
				// ROMS (shadow or not) are always active...
				_ROM			-> setActive (true);
				_ROM_S1			-> setActive (true);
				_ROM_S2			-> setActive (true);
				_ROM_S3			-> setActive (true);
				// ...but not the expansions...
				_ROMCS1			-> setActive (false);
				_ROMCS2			-> setActive (false);

				// RAMS: 1k active (sadows included)...
				_RAM1K			-> setActive (true);
				_RAM1K_S		-> setActive (true);
				_RAM1K_V		-> setActive (true);
				for (size_t i = 0; i < 15; _RAM15K_UC [i++] -> setActive (true));
				for (size_t i = 0; i < 15; _RAM15K_UC_S [i++] -> setActive (true));
				for (size_t i = 0; i < 15; _RAM15K_V [i++] -> setActive (true));
				// ...but not expansion related...
				_RAM16K_CS1		-> setActive (false);
				_RAM16K_S		-> setActive (false);
				_RAM16K_V		-> setActive (false);

				// The stack is in the 1k zone...
				_STACK_SUBSET = _RAM1K -> id ();
				_stack = nullptr; // To reload the stack...
			}

			break;

		case ZX81::Memory::Configuration::_16KEXPANSION:
			{
				// This situation shouldn't happen, just in case...
				if (t == ZX81::Type::_ZX80)
					_LOG ("ZX80 doesn't allow _16KEXPANSION configuration");
				else
				{
					// ROMS (shadow or not) are always active
					_ROM			-> setActive (true);
					_ROM_S1			-> setActive (true);
					_ROM_S2			-> setActive (true);
					_ROM_S3			-> setActive (true);
					// ...but not the expansions...
					_ROMCS1			-> setActive (false);
					_ROMCS2			-> setActive (false);

					// RAMS: 1k no active...
					_RAM1K			-> setActive (false);
					_RAM1K_S		-> setActive (false);
					_RAM1K_V		-> setActive (false);
					for (size_t i = 0; i < 15; _RAM15K_UC [i++] -> setActive (false));
					for (size_t i = 0; i < 15; _RAM15K_UC_S [i++] -> setActive (false));
					for (size_t i = 0; i < 15; _RAM15K_V [i++] -> setActive (false));
					// ...and expansion active!
					_RAM16K_CS1		-> setActive (true);
					_RAM16K_S		-> setActive (true);
					_RAM16K_V		-> setActive (true);

					// The stack is in the 16k zone
					_STACK_SUBSET = _RAM16K_CS1 -> id ();
					_stack = nullptr; // To reload the stack!
				}
			}

			break;

		default:
			{ 
				// It shouldn't exist, but just in case...
				_LOG ("ZX81 configuration mode " + 
					std::to_string ((int) _configuration) + " not supported");

				assert (false);
			}

			break;
	}
}

// ---
bool ZX81::Memory::initialize ()
{
	bool result = MCHEmul::Memory::initialize ();
	if (!result)
		return (false);

	// Initialize the RAM with random values...
	// ...as it is described mainñy in  the ZXSpectrum documentation...
	// The ROM is not affected at all!
	for (size_t i = 0; i < _RAM1K -> size (); i++)
		_RAM1K -> set (_RAM1K -> initialAddress () + i, std::rand () % 256);
	for (size_t i = 0; i < _RAM16K_CS1 -> size (); i++)
		_RAM16K_CS1 -> set (_RAM16K_CS1 -> initialAddress () + i, std::rand () % 256);

	// The active view has to be initially the CPU view...
	setCPUView ();

	return (true);
}

// ---
MCHEmul::Memory::Content ZX81::Memory::standardMemoryContent (ZX81::Type t)
{
	// So far the structue of the memory is the same in any type of ZX81 like computer...
	// What changes is the ROM loaded, and maybe the way it is configured...

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
	// First block is ROM
	MCHEmul::PhysicalStorageSubset* ROMS = new MCHEmul::PhysicalStorageSubset 
		(_ROM_SUBSET, ROM, 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 0x2000);					// 8k
	MCHEmul::MirrorPhysicalStorageSubset* ROMS_S1 = new MCHEmul::MirrorPhysicalStorageSubset
		(_ROMSHADOW1_SUBSET, ROMS, MCHEmul::Address ({ 0x00, 0x20 }, false));							// 8k. It is a mirror of ROMS
	// Or, when a expansion cartridge is inserted (ROMCS = 0), the hole space can be accesible as ROM....
	MCHEmul::PhysicalStorageSubset* ROMS_CS1 = new MCHEmul::PhysicalStorageSubset 
		(_ROMCS1_SUBSET, ROM, 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 0x4000);				// 16k
	// ----- 16k

	// Second block is RAM
	// The RAM, that can also behave as an stack...
	// As Z80 documentation describes, the stack behaves like "pointing to not an empty place" because
	// before keeping something, the pointer of the stack is decremented and then the value kept.
	// and it starts from the back to the front...
	// Then we have 1k (the standard one)
	MCHEmul::Stack* RAM1S = new MCHEmul::Stack
		(_RAM1K_SUBSET, RAM, 0x4000, MCHEmul::Address ({ 0x00, 0x40 }, false), 0x0400, 
			MCHEmul::Stack::Configuration (true, false /** Pointing always to the last written. */, 
				false /** No overflow detection. */, -1));	// 1k that can behave as Stack...
	// ...and the rest (up to 16k) is not accesible and mirrored the previous one in 1k blocks-size...
	std::vector <MCHEmul::MirrorPhysicalStorageSubset*> RAM1S_S;
	for (size_t i = 1; i <= 15; i++)
		RAM1S_S.emplace_back (new MCHEmul::MirrorPhysicalStorageSubset
			(_RAM1K_SUBSET + (int) i, RAM1S, 
				MCHEmul::Address ({ 0x00, 0x40 + (unsigned char) (i << 2) }, false)));					// 15k that are a mirror...
	// But these previous 16k could be fully accesible externally when the RAM_CS = 0...
	MCHEmul::Stack* RAM16S_CS1 = new MCHEmul::Stack
		(_RAM16KCS1_SUBSET, RAM, 0x4000, MCHEmul::Address ({ 0x00, 0x40 }, false), 0x4000, 
			MCHEmul::Stack::Configuration (true, false /** Pointing always to the last written. */,
				true /** No overflow detection. */, -1));												// 16k (When expansion), 
																										// ...that can behave as stack...
																										// One or another...
	// ----- 16k
 
	// Third block is ROM again
	// Wither two mirrors of the ROM again...
	MCHEmul::MirrorPhysicalStorageSubset* ROMS_S2 = new MCHEmul::MirrorPhysicalStorageSubset
		(_ROMSHADOW2_SUBSET, ROMS, MCHEmul::Address ({ 0x00, 0x80 }, false));							// 8k. It is a mirror of ROMS (above)
	MCHEmul::MirrorPhysicalStorageSubset* ROMS_S3 = new MCHEmul::MirrorPhysicalStorageSubset
		(_ROMSHADOW3_SUBSET, ROMS, MCHEmul::Address ({ 0x00, 0xa0 }, false));							// 8k. It is a mirror of ROMS (above)
	// Or, when a expansion cartridge is inserted (ROMCS = 0), the hole space can be accesible as ROM....
	MCHEmul::PhysicalStorageSubset* ROMS_CS2 = new MCHEmul::PhysicalStorageSubset 
		(_ROMCS2_SUBSET, ROM, 0x8000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000);				// 16k. 
	// ----- 16k

	// Fourth and last block is RAM and video RAM depending on the point of view (CPU or ULA)
	// From the ULA perspective...
	// The rest is a mirror of the 1k + 15 or 16k RAM from the ULA perspective...
	MCHEmul::MirrorPhysicalStorageSubset* RAM1S_S1 = new MCHEmul::MirrorPhysicalStorageSubset
		(_RAM1KSHADOW_SUBSET, RAM1S, MCHEmul::Address ({ 0x00, 0xc0 }, false));							// 1k. It is a mirror of RAM1S
	// The next 15k are a mirror of the previous ones...
	std::vector <MCHEmul::MirrorPhysicalStorageSubset*> RAM1S_S1_S;
	for (size_t i = 1; i <= 15; i++)
		RAM1S_S1_S.emplace_back (new MCHEmul::MirrorPhysicalStorageSubset
			(_RAM1KSHADOW_SUBSET + (int) i, RAM1S, /* The same than obove. */
				MCHEmul::Address ({ 0x00, 0xc0 + (unsigned char) (i << 2) }, false)));					// 15k. It is a mirror of RAM1S_S1
	// ...but the can be replace by the mirror of the 16k if there was that... (RAM_CS = 0)
	MCHEmul::MirrorPhysicalStorageSubset* RAM16S_CS1S1 = new MCHEmul::MirrorPhysicalStorageSubset
		(_RAM16KSHADOW_SUBSET, RAM16S_CS1, MCHEmul::Address ({ 0x00, 0xc0 }, false));					// 16k. It is a mirror of RAM16S_CS1
	// ----- 16k

	// From CPU perspective...
	// But it exists for the CPU, and it is the video code RAM for the ULA...
	// With 1 k + 15 of nothing...
	ZX81::MemoryVideoCode* RAM1S_V = new ZX81::MemoryVideoCode
		(_RAM1K_V_SUBSET, RAM1S, MCHEmul::Address ({ 0x00, 0xc0 }, false));								// 1k. It is a mirror of RAM1S
	std::vector <ZX81::MemoryVideoCode*> RAM1S_V_S;
	for (size_t i = 1; i <= 15; i++)
		RAM1S_V_S.emplace_back (new ZX81::MemoryVideoCode
			(_RAM1K_V_SUBSET + (int) i, RAM1S, /* The same than at the beginning. */
				MCHEmul::Address ({ 0x00, 0xc0 + (unsigned char) (i << 2) }, false)));					// 15k. It is a mirror of RAM1S_V
	// ...or 16k mirroring RAM16S_CS1 when (RAMCS = 0)
	ZX81::MemoryVideoCode* RAM16S_V = new ZX81::MemoryVideoCode
		(_RAM16K_V_SUBSET, RAM16S_CS1, MCHEmul::Address ({ 0x00, 0xc0 }, false));						// 16k. It is a mirror of RAM16S
	// ----- 16k

	// A map with all the subsets possible...
	MCHEmul::PhysicalStorageSubsets allsubsets (
		{
			{ _ROM_SUBSET,						ROMS }, 
			{ _ROMSHADOW1_SUBSET,				ROMS_S1 }, 
			{ _ROMCS1_SUBSET,					ROMS_CS1 }, 
			{ _RAM1K_SUBSET,					RAM1S },
			// And its mirrors...
			{ _RAM1K_S_SUBSET + 0,				RAM1S_S [0] },
			{ _RAM1K_S_SUBSET + 1,				RAM1S_S [1] },
			{ _RAM1K_S_SUBSET + 2,				RAM1S_S [2] },
			{ _RAM1K_S_SUBSET + 3,				RAM1S_S [3] },
			{ _RAM1K_S_SUBSET + 4,				RAM1S_S [4] },
			{ _RAM1K_S_SUBSET + 5,				RAM1S_S [5] },
			{ _RAM1K_S_SUBSET + 6,				RAM1S_S [6] },
			{ _RAM1K_S_SUBSET + 7,				RAM1S_S [7] },
			{ _RAM1K_S_SUBSET + 8,				RAM1S_S [8] },
			{ _RAM1K_S_SUBSET + 9,				RAM1S_S [9] },
			{ _RAM1K_S_SUBSET + 10,				RAM1S_S [10] },
			{ _RAM1K_S_SUBSET + 11,				RAM1S_S [11] },
			{ _RAM1K_S_SUBSET + 12,				RAM1S_S [12] },
			{ _RAM1K_S_SUBSET + 13,				RAM1S_S [13] },
			{ _RAM1K_S_SUBSET + 14,				RAM1S_S [14] },
			{ _RAM16KCS1_SUBSET,				RAM16S_CS1 },
			{ _ROMSHADOW2_SUBSET,				ROMS_S2 },
			{ _ROMSHADOW3_SUBSET,				ROMS_S3 }, 
			{ _ROMCS2_SUBSET,					ROMS_CS2 }, 
			{ _RAM1KSHADOW_SUBSET,				RAM1S_S1 },
			// And its mirrors...
			{ _RAM1KSHADOW_S_SUBSET + 0,		RAM1S_S1_S [0] },
			{ _RAM1KSHADOW_S_SUBSET + 1,		RAM1S_S1_S [1] },
			{ _RAM1KSHADOW_S_SUBSET + 2,		RAM1S_S1_S [2] },
			{ _RAM1KSHADOW_S_SUBSET + 3,		RAM1S_S1_S [3] },
			{ _RAM1KSHADOW_S_SUBSET + 4,		RAM1S_S1_S [4] },
			{ _RAM1KSHADOW_S_SUBSET + 5,		RAM1S_S1_S [5] },
			{ _RAM1KSHADOW_S_SUBSET + 6,		RAM1S_S1_S [6] },
			{ _RAM1KSHADOW_S_SUBSET + 7,		RAM1S_S1_S [7] },
			{ _RAM1KSHADOW_S_SUBSET + 8,		RAM1S_S1_S [8] },
			{ _RAM1KSHADOW_S_SUBSET + 9,		RAM1S_S1_S [9] },
			{ _RAM1KSHADOW_S_SUBSET + 10,		RAM1S_S1_S [10] },
			{ _RAM1KSHADOW_S_SUBSET + 11,		RAM1S_S1_S [11] },
			{ _RAM1KSHADOW_S_SUBSET + 12,		RAM1S_S1_S [12] },
			{ _RAM1KSHADOW_S_SUBSET + 13,		RAM1S_S1_S [13] },
			{ _RAM1KSHADOW_S_SUBSET + 14,		RAM1S_S1_S [14] },
			{ _RAM16KSHADOW_SUBSET,				RAM16S_CS1S1 },
			{ _RAM1K_V_SUBSET,					RAM1S_V },
			// And its mirrors...
			{ _RAM1K_V_S_SUBSET + 0,			RAM1S_V_S [0] },
			{ _RAM1K_V_S_SUBSET + 1,			RAM1S_V_S [1] },
			{ _RAM1K_V_S_SUBSET + 2,			RAM1S_V_S [2] },
			{ _RAM1K_V_S_SUBSET + 3,			RAM1S_V_S [3] },
			{ _RAM1K_V_S_SUBSET + 4,			RAM1S_V_S [4] },
			{ _RAM1K_V_S_SUBSET + 5,			RAM1S_V_S [5] },
			{ _RAM1K_V_S_SUBSET + 6,			RAM1S_V_S [6] },
			{ _RAM1K_V_S_SUBSET + 7,			RAM1S_V_S [7] },
			{ _RAM1K_V_S_SUBSET + 8,			RAM1S_V_S [8] },
			{ _RAM1K_V_S_SUBSET + 9,			RAM1S_V_S [9] },
			{ _RAM1K_V_S_SUBSET + 10,			RAM1S_V_S [10] },
			{ _RAM1K_V_S_SUBSET + 11,			RAM1S_V_S [11] },
			{ _RAM1K_V_S_SUBSET + 12,			RAM1S_V_S [12] },
			{ _RAM1K_V_S_SUBSET + 13,			RAM1S_V_S [13] },
			{ _RAM1K_V_S_SUBSET + 14,			RAM1S_V_S [14] },
			{ _RAM16K_V_SUBSET,					RAM16S_V }
		});

	// Then the views...
	// The view from the CPU...
	MCHEmul::MemoryView* cpuView = new MCHEmul::MemoryView 
		(_CPU_VIEW, 
			{ { _ROM_SUBSET,					ROMS }, 
			  { _ROMSHADOW1_SUBSET,				ROMS_S1 }, 
			  { _ROMCS1_SUBSET,					ROMS_CS1 }, 
			  { _RAM1K_SUBSET,					RAM1S }, 
			  { _RAM1K_S_SUBSET + 0,			RAM1S_S [0] },
			  { _RAM1K_S_SUBSET + 1,			RAM1S_S [1] },
			  { _RAM1K_S_SUBSET + 2,			RAM1S_S [2] },
			  { _RAM1K_S_SUBSET + 3,			RAM1S_S [3] },
			  { _RAM1K_S_SUBSET + 4,			RAM1S_S [4] },
			  { _RAM1K_S_SUBSET + 5,			RAM1S_S [5] },
			  { _RAM1K_S_SUBSET + 6,			RAM1S_S [6] },
			  { _RAM1K_S_SUBSET + 7,			RAM1S_S [7] },
			  { _RAM1K_S_SUBSET + 8,			RAM1S_S [8] },
			  { _RAM1K_S_SUBSET + 9,			RAM1S_S [9] },
			  { _RAM1K_S_SUBSET + 10,			RAM1S_S [10] },
			  { _RAM1K_S_SUBSET + 11,			RAM1S_S [11] },
			  { _RAM1K_S_SUBSET + 12,			RAM1S_S [12] },
			  { _RAM1K_S_SUBSET + 13,			RAM1S_S [13] },
			  { _RAM1K_S_SUBSET + 14,			RAM1S_S [14] },
			  { _RAM16KCS1_SUBSET,					RAM16S_CS1 },
			  { _ROMSHADOW2_SUBSET,				ROMS_S2 }, 
			  { _ROMSHADOW3_SUBSET,				ROMS_S3 }, 
			  { _ROMCS2_SUBSET,					ROMS_CS2 }, 
			  // From the CPU, this up zone of the memory is viewed as video executable data...
			  { _RAM1K_V_SUBSET,				RAM1S_V },
			  { _RAM1K_V_S_SUBSET + 0,			RAM1S_V_S [0] },
			  { _RAM1K_V_S_SUBSET + 1,			RAM1S_V_S [1] },
			  { _RAM1K_V_S_SUBSET + 2,			RAM1S_V_S [2] },
			  { _RAM1K_V_S_SUBSET + 3,			RAM1S_V_S [3] },
			  { _RAM1K_V_S_SUBSET + 4,			RAM1S_V_S [4] },
			  { _RAM1K_V_S_SUBSET + 5,			RAM1S_V_S [5] },
			  { _RAM1K_V_S_SUBSET + 6,			RAM1S_V_S [6] },
			  { _RAM1K_V_S_SUBSET + 7,			RAM1S_V_S [7] },
			  { _RAM1K_V_S_SUBSET + 8,			RAM1S_V_S [8] },
			  { _RAM1K_V_S_SUBSET + 9,			RAM1S_V_S [9] },
			  { _RAM1K_V_S_SUBSET + 10,			RAM1S_V_S [10] },
			  { _RAM1K_V_S_SUBSET + 11,			RAM1S_V_S [11] },
			  { _RAM1K_V_S_SUBSET + 12,			RAM1S_V_S [12] },
			  { _RAM1K_V_S_SUBSET + 13,			RAM1S_V_S [13] },
			  { _RAM1K_V_S_SUBSET + 14,			RAM1S_V_S [14] },
			  { _RAM16K_V_SUBSET,				RAM16S_V } });

	// ...and the view from the ULA...
	MCHEmul::MemoryView* ulaView = new MCHEmul::MemoryView 
		(_ULA_VIEW, 
			{ { _ROM_SUBSET,					ROMS }, 
			  { _ROMSHADOW1_SUBSET,				ROMS_S1 }, 
			  { _ROMCS1_SUBSET,					ROMS_CS1 }, 
			  { _RAM1K_SUBSET,					RAM1S }, 
			  { _RAM1K_S_SUBSET + 0,			RAM1S_S [0] },
			  { _RAM1K_S_SUBSET + 1,			RAM1S_S [1] },
			  { _RAM1K_S_SUBSET + 2,			RAM1S_S [2] },
			  { _RAM1K_S_SUBSET + 3,			RAM1S_S [3] },
			  { _RAM1K_S_SUBSET + 4,			RAM1S_S [4] },
			  { _RAM1K_S_SUBSET + 5,			RAM1S_S [5] },
			  { _RAM1K_S_SUBSET + 6,			RAM1S_S [6] },
			  { _RAM1K_S_SUBSET + 7,			RAM1S_S [7] },
			  { _RAM1K_S_SUBSET + 8,			RAM1S_S [8] },
			  { _RAM1K_S_SUBSET + 9,			RAM1S_S [9] },
			  { _RAM1K_S_SUBSET + 10,			RAM1S_S [10] },
			  { _RAM1K_S_SUBSET + 11,			RAM1S_S [11] },
			  { _RAM1K_S_SUBSET + 12,			RAM1S_S [12] },
			  { _RAM1K_S_SUBSET + 13,			RAM1S_S [13] },
			  { _RAM1K_S_SUBSET + 14,			RAM1S_S [14] },
			  { _RAM16KCS1_SUBSET,				RAM16S_CS1 },
			  { _ROMSHADOW2_SUBSET,				ROMS_S2 }, 
			  { _ROMSHADOW3_SUBSET,				ROMS_S3 }, 
			  { _ROMCS2_SUBSET,					ROMS_CS2 }, 
			  { _RAM1KSHADOW_SUBSET,			RAM1S_S1 }, 
			  { _RAM1KSHADOW_S_SUBSET + 0,		RAM1S_S1_S [0] },
			  { _RAM1KSHADOW_S_SUBSET + 1,		RAM1S_S1_S [1] },
			  { _RAM1KSHADOW_S_SUBSET + 2,		RAM1S_S1_S [2] },
			  { _RAM1KSHADOW_S_SUBSET + 3,		RAM1S_S1_S [3] },
			  { _RAM1KSHADOW_S_SUBSET + 4,		RAM1S_S1_S [4] },
			  { _RAM1KSHADOW_S_SUBSET + 5,		RAM1S_S1_S [5] },
			  { _RAM1KSHADOW_S_SUBSET + 6,		RAM1S_S1_S [6] },
			  { _RAM1KSHADOW_S_SUBSET + 7,		RAM1S_S1_S [7] },
			  { _RAM1KSHADOW_S_SUBSET + 8,		RAM1S_S1_S [8] },
			  { _RAM1KSHADOW_S_SUBSET + 9,		RAM1S_S1_S [9] },
			  { _RAM1KSHADOW_S_SUBSET + 10,		RAM1S_S1_S [10] },
			  { _RAM1KSHADOW_S_SUBSET + 11,		RAM1S_S1_S [11] },
			  { _RAM1KSHADOW_S_SUBSET + 12,		RAM1S_S1_S [12] },
			  { _RAM1KSHADOW_S_SUBSET + 13,		RAM1S_S1_S [13] },
			  { _RAM1KSHADOW_S_SUBSET + 14,		RAM1S_S1_S [14] },
			  { _RAM16KSHADOW_SUBSET,			RAM16S_CS1S1 }
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
