#include <ZX81/Memory.hpp>

// ---
const MCHEmul::UByte& ZX81::MemoryVideoCode::readValue (size_t nB) const
{ 
	if (!(_lastValueRead = MCHEmul::MirrorPhysicalStorageSubset::readValue (nB)).bit (6))
		_lastValueRead = MCHEmul::UByte::_0; // To emulate the video code = 0...
	return (_lastValueRead);
} 

// ---
ZX81::Memory::Memory (ZX81::Memory::Configuration cfg, unsigned int m)
	: MCHEmul::Memory (0, ZX81::Memory::standardMemoryContent (), { }),
	  _configuration (cfg),
	  _ROM (nullptr), 
	  _RAM1K (nullptr),
	  _RAM1K_S (nullptr),
	  _RAM1K_V (nullptr), 
	  _RAM15K_UC (),
	  _RAM15K_UC_S (),
	  _RAM15K_V (),
	  _RAM16K (nullptr),
	  _RAM16K_S (nullptr),
	  _RAM16K_V (nullptr),
	  _STACK_SUBSET (0)
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
	subset (_ROMSHADOW1_SUBSET) -> fixDefaultValues ();
	subset (_ROMSHADOW2_SUBSET) -> fixDefaultValues ();
	subset (_ROMSHADOW3_SUBSET) -> fixDefaultValues ();

	if (!ok)
		_error = MCHEmul::_INIT_ERROR;

	// Gets a pointer to the main pieces of the memory that are activable...
	_ROM				= subset (_ROM_SUBSET);
	_RAM1K				= dynamic_cast <MCHEmul::Stack*> (subset (_RAM1K_SUBSET));
	_RAM1K_S			= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> (subset (_RAM1KSHADOW_SUBSET));
	_RAM1K_V			= dynamic_cast <ZX81::MemoryVideoCode*> (subset (_RAM1K_V_SUBSET));
	for (size_t i = 1; i <= 15; i++)
		_RAM15K_UC.emplace_back (dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> (subset (_RAM1K_SUBSET + i)));
	for (size_t i = 1; i <= 15; i++)
		_RAM15K_UC_S.emplace_back (dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> (subset (_RAM1KSHADOW_SUBSET + i)));
	for (size_t i = 1; i <= 15; i++)
		_RAM15K_V.emplace_back (dynamic_cast <ZX81::MemoryVideoCode*> (subset (_RAM1K_V_SUBSET + i)));
	_RAM16K				= dynamic_cast <MCHEmul::Stack*> (subset (_RAM16K_SUBSET));
	_RAM16K_S			= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> (subset (_RAM16KSHADOW_SUBSET));
	_RAM16K_V			= dynamic_cast <ZX81::MemoryVideoCode*> (subset (_RAM16K_V_SUBSET));
	assert (
		_ROM			!= nullptr &&
		_RAM1K			!= nullptr &&
		_RAM1K_S		!= nullptr &&
		_RAM1K_V		!= nullptr &&
		std::find (_RAM15K_UC.begin (), _RAM15K_UC.end (), nullptr) == _RAM15K_UC.end () &&
		std::find (_RAM15K_UC_S.begin (), _RAM15K_UC_S.end (), nullptr) == _RAM15K_UC_S.end () &&
		std::find (_RAM15K_V.begin (), _RAM15K_V.end (), nullptr) == _RAM15K_V.end () &&
		_RAM16K			!= nullptr &&
		_RAM16K_S		!= nullptr &&
		_RAM16K_V		!= nullptr);

	// Sets the configuration of the memory...
	setConfiguration (_configuration);
}

// ---
void ZX81::Memory::setConfiguration (ZX81::Memory::Configuration cfg)
{
	// In any of the configurations, 
	// the stack will behave as already used from the aerly beginning...
	_RAM1K -> setNotUsed (false);
	_RAM16K -> setNotUsed (false);

	// Attending to the configuration different options are active or not active!
	switch (_configuration = cfg)
	{
		case ZX81::Memory::Configuration::_NOEXPANDED:
			{
				// ROMS (shadow or not) are always active

				_RAM1K			-> setActive (true);
				_RAM1K_S		-> setActive (true);
				_RAM1K_V		-> setActive (true);
				for (size_t i = 0; i < 15; _RAM15K_UC [i++] -> setActive (true));
				for (size_t i = 0; i < 15; _RAM15K_UC_S [i++] -> setActive (true));
				for (size_t i = 0; i < 15; _RAM15K_V [i++] -> setActive (true));
				_RAM16K			-> setActive (false);
				_RAM16K_S		-> setActive (false);
				_RAM16K_V		-> setActive (false);

				_STACK_SUBSET = _RAM1K -> id ();
			}

			break;

		case ZX81::Memory::Configuration::_16KEXPANSION:
			{
				// ROMS (shadow or not) are always active

				_RAM1K			-> setActive (false);
				_RAM1K_S		-> setActive (false);
				_RAM1K_V		-> setActive (false);
				for (size_t i = 0; i < 15; _RAM15K_UC [i++] -> setActive (false));
				for (size_t i = 0; i < 15; _RAM15K_UC_S [i++] -> setActive (false));
				for (size_t i = 0; i < 15; _RAM15K_V [i++] -> setActive (false));
				_RAM16K			-> setActive (true);
				_RAM16K_S		-> setActive (true);
				_RAM16K_V		-> setActive (true);

				_STACK_SUBSET = _RAM16K -> id ();
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

	// The memory RAM is fill up to 0x07! (learnt from other emulators)
	_RAM1K ->	fillWith (MCHEmul::UByte (0x07));
	_RAM16K ->	fillWith (MCHEmul::UByte (0x07));

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
		new MCHEmul::PhysicalStorage (_RAM_SET, MCHEmul::PhysicalStorage::Type::_RAM, 0x10000);				// 64k
	MCHEmul::PhysicalStorage* ROM = 
		new MCHEmul::PhysicalStorage (_ROM_SET, MCHEmul::PhysicalStorage::Type::_ROM, 0x2000);				// 8k

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

	// Subsets
	// The ROM
	MCHEmul::PhysicalStorageSubset* ROMS = new MCHEmul::PhysicalStorageSubset 
		(_ROM_SUBSET, ROM, 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 0x2000);					// 8k
	MCHEmul::MirrorPhysicalStorageSubset* ROMS_S1 = new MCHEmul::MirrorPhysicalStorageSubset
		(_ROMSHADOW1_SUBSET, ROMS, MCHEmul::Address ({ 0x00, 0x20 }, false));							// 8k. It is a mirror of ROMS
	// ----- 16k

	// The RAM, that can also behave as a stack...
	// As Z80 documentation describes, the stack behaves like "pointing to not an empty place"
	// because before keeping something decrement the pointer of the stack and then keeps the value...
	// and it starts from the back to the from...
	// Then we have 1k (the standard one)
	MCHEmul::Stack* RAM1S = new MCHEmul::Stack
		(_RAM1K_SUBSET, RAM, 0x4000, MCHEmul::Address ({ 0x00, 0x40 }, false), 0x0400, true, false);	// 1k that can behave as Stack...
	// ...and the rest (up to 16k) is not accesible and mirrored in 1k block size from the previous one...
	std::vector <MCHEmul::MirrorPhysicalStorageSubset*> RAM1S_S;
	for (size_t i = 1; i <= 15; i++)
		RAM1S_S.emplace_back (new MCHEmul::MirrorPhysicalStorageSubset
			(_RAM1K_SUBSET + (int) i, RAM1S, 
				MCHEmul::Address ({ 0x00, 0x40 + (unsigned char) (i << 2) }, false)));					// 15k that are a mirror...
	// Or all can be replaced by a module of 16K
	MCHEmul::Stack* RAM16S = new MCHEmul::Stack
		(_RAM16K_SUBSET, RAM, 0x4000, MCHEmul::Address ({ 0x00, 0x40 }, false), 0x4000, true, false);	// 16k (When expansion), 
																										// ...that can behave as stack...
																										// One or another...
	// ----- 16k
 
	// Then two mirrors of the ROM again...
	MCHEmul::MirrorPhysicalStorageSubset* ROMS_S2 = new MCHEmul::MirrorPhysicalStorageSubset
		(_ROMSHADOW2_SUBSET, ROMS, MCHEmul::Address ({ 0x00, 0x80 }, false));							// 8k. It is a mirror of ROMS
	MCHEmul::MirrorPhysicalStorageSubset* ROMS_S3 = new MCHEmul::MirrorPhysicalStorageSubset
		(_ROMSHADOW3_SUBSET, ROMS, MCHEmul::Address ({ 0x00, 0xa0 }, false));							// 8k. It is a mirror of ROMS
	// ----- 16k

	// The rest is a mirror of the 1k + 15 or 16k RAM from the ULA perspective...
	MCHEmul::MirrorPhysicalStorageSubset* RAM1S_S1 = new MCHEmul::MirrorPhysicalStorageSubset
		(_RAM1KSHADOW_SUBSET, RAM1S, MCHEmul::Address ({ 0x00, 0xc0 }, false));							// 1k. It is a mirror of RAM1S
	// The next 15k are a mirror of the previous ones...
	std::vector <MCHEmul::MirrorPhysicalStorageSubset*> RAM1S_S1_S;
	for (size_t i = 1; i <= 15; i++)
		RAM1S_S1_S.emplace_back (new MCHEmul::MirrorPhysicalStorageSubset
			(_RAM1KSHADOW_SUBSET + (int) i, RAM1S, /* The same than obove. */
				MCHEmul::Address ({ 0x00, 0xc0 + (unsigned char) (i << 2) }, false)));					// 15k. It is a mirror of RAM1S_S1
	// ...but the can be replace by the mirrir of the 16k if there was that...
	MCHEmul::MirrorPhysicalStorageSubset* RAM16S_S1 = new MCHEmul::MirrorPhysicalStorageSubset
		(_RAM16KSHADOW_SUBSET, RAM16S, MCHEmul::Address ({ 0x00, 0xc0 }, false));						// 16k. It is a mirror of RAM16S
	// ----- 16k

	// But it exists for the CPU, and it is the video code RAM for the ULA...
	// With 1 k + 15 of nothing...
	ZX81::MemoryVideoCode* RAM1S_V = new ZX81::MemoryVideoCode
		(_RAM1K_V_SUBSET, RAM1S, MCHEmul::Address ({ 0x00, 0xc0 }, false));								// 1k. It is a mirror of RAM1S
	std::vector <ZX81::MemoryVideoCode*> RAM1S_V_S;
	for (size_t i = 1; i <= 15; i++)
		RAM1S_V_S.emplace_back (new ZX81::MemoryVideoCode
			(_RAM1K_V_SUBSET + (int) i, RAM1S, /* The same than at the beginning. */
				MCHEmul::Address ({ 0x00, 0xc0 + (unsigned char) (i << 2) }, false)));					// 15k. It is a mirror of RAM1S_V
	// ...or 16k!
	ZX81::MemoryVideoCode* RAM16S_V = new ZX81::MemoryVideoCode
		(_RAM16K_V_SUBSET, RAM16S, MCHEmul::Address ({ 0x00, 0xc0 }, false));							// 16k. It is a mirror of RAM16S
	// ----- 16k

	// A map with all the subsets possible...
	MCHEmul::PhysicalStorageSubsets allsubsets (
		{
			{ _ROM_SUBSET,						ROMS }, 
			{ _ROMSHADOW1_SUBSET,				ROMS_S1 }, 
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
			{ _RAM16K_SUBSET,					RAM16S },
			{ _ROMSHADOW2_SUBSET,				ROMS_S2 },
			{ _ROMSHADOW3_SUBSET,				ROMS_S3 }, 
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
			{ _RAM16KSHADOW_SUBSET,				RAM16S_S1 },
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
			  { _RAM16K_SUBSET,					RAM16S },
			  { _ROMSHADOW2_SUBSET,				ROMS_S2 }, 
			  { _ROMSHADOW3_SUBSET,				ROMS_S3 }, 
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
			  { _RAM16K_SUBSET,					RAM16S },
			  { _ROMSHADOW2_SUBSET,				ROMS_S2 }, 
			  { _ROMSHADOW3_SUBSET,				ROMS_S3 }, 
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
			  { _RAM16KSHADOW_SUBSET,			RAM16S_S1 }
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
