#include <C264/Memory.hpp>
#include <C264/C6529B1Registers.hpp>
#include <C264/C6529B2Registers.hpp>
#include <C264/TEDRegisters.hpp>
#include <C264/ROMRAMSwitchRegisters.hpp>

// ---
C264::Memory::Memory (const MCHEmul::Memory::Content& cnt, const std::string& lang)
	: MCHEmul::Memory (0, cnt, { }),
	  _configuration (MCHEmul::UByte::_0), // Meaning BASIC & KERNEL actives...but is not used...
	  _RAM1 (nullptr), // Initialized later...
	  _RAM2 (nullptr),
	  _basicROM (nullptr),
	  _RAM3 (nullptr),
	  _carlow1 (nullptr),
	  _carlow2 (nullptr),
	  _kernelROM1 (nullptr),
	  _kernelROM2 (nullptr),
	  _RAM4 (nullptr),
	  _carhigh1 (nullptr),
	  _carhigh2 (nullptr),
	  _kernelROM3 (nullptr),
	  _RAM5 (nullptr)
{
	// In the content...
	if (error () != MCHEmul::_NOERROR)
		return;

	// Get references to all things tha can change in configuration...
	_RAM1				= subset (_RAM1_SUBSET);
	_RAM2				= subset (_RAM2_SUBSET);
	_basicROM			= subset (_BASICROM_SUBSET);
	_RAM3				= subset (_RAM3_SUBSET);
	_carlow1			= subset (_CARLOW1_SUBSET);
	_carlow2			= subset (_CARLOW2_SUBSET);
	_kernelROM1			= subset (_KERNELROM1_SUBSET);
	_kernelROM2			= subset (_KERNELROM2_SUBSET);
	_RAM4				= subset (_RAM4_SUBSET);
	_carhigh1			= subset (_CARHIGH1_SUBSET);
	_carhigh2			= subset (_CARHIGH2_SUBSET);
	_kernelROM3			= subset (_KERNELROM3_SUBSET);
	_RAM5				= subset (_RAM5_SUBSET);

	// The default ROMS...
	// They might change depending on the language
	// By default the English language is taken
	std::string ROMFILE = "./basic.318006-01.bin";
	std::string KERNELFILE = "./kernal.318004-05-ENG.bin";

	// If the languaje selected is not ENG...
	// then another char rom file is selected!
	// The CHARROM here is part of the Kernal
	if (lang == "FRA") { KERNELFILE = "./kernal.318005-05-FRA.bin"; }
	else if (lang == "HUN") { KERNELFILE = "./kernal.318030-02-HUN.bin"; }
	else if (lang == "SWE") { KERNELFILE = "./kernel.325155-03-SWE.bin"; }

	bool ok = true;
	ok &= physicalStorage (_BASICROM) -> loadInto (ROMFILE);
	subset (_BASICROM_SUBSET) -> fixDefaultValues (); // Fix the values for further initializations...
	ok &= physicalStorage (_KERNELROM) -> loadInto (KERNELFILE);
	subset (_KERNELROM1_SUBSET) -> fixDefaultValues ();
	subset (_KERNELROM2_SUBSET) -> fixDefaultValues ();
	subset (_KERNELROM3_SUBSET) -> fixDefaultValues ();

	if (!ok)
		_error = MCHEmul::_INIT_ERROR;

	// The configuration is not set, 
	// as it is done in the classes inheriting this one...
}

// ---
void C264::Memory::setConfiguration (const MCHEmul::UByte& cfg)
{
	_configuration = cfg;

	// Always!
	_RAM1		-> setActive (true);
	_RAM2		-> setActive (true);

	// Meaning RAM active...
	if (_configuration == MCHEmul::UByte::_FF)
	{
		// ROMS inactive...
		_basicROM	-> setActive (false);
		_kernelROM1	-> setActive (false);
		_kernelROM2	-> setActive (false);
		_kernelROM3	-> setActive (false);

		// "Empty" Cartridges inactive...
		_carlow1	-> setActive (false);
		_carlow2	-> setActive (false);
		_carhigh1	-> setActive (false);
		_carhigh2	-> setActive (false);

		// "Official" Catridges inactive also...
		if (existsSubset (_CARLOW1CON_SUBSET))
			subset (_CARLOW1CON_SUBSET) -> setActive (false);
		if (existsSubset (_CARLOW2CON_SUBSET))
			subset (_CARLOW2CON_SUBSET) -> setActive (false);
		if (existsSubset (_CARHIGH1CON_SUBSET))
			subset (_CARHIGH1CON_SUBSET) -> setActive (false);
		if (existsSubset (_CARHIGH2CON_SUBSET))
			subset (_CARHIGH2CON_SUBSET) -> setActive (false);

		// RAMs active!
		_RAM3		-> setActive (true);
		_RAM4		-> setActive (true);
		_RAM5		-> setActive (true);

		return;	
	}

	// Meaning ROMS active...
	// But which one will actually depend on the configuration...
	// LOW ROMS
	switch (_configuration.value () & 0x03)
	{
		// LOROM = BASIC
		case 0:
			{
				// BASIC ROM active...
				_basicROM -> setActive (true);

				// "Empty" cartridges (at this place) inactive...
				_carlow1 -> setActive (false);
				_carlow2 -> setActive (false);
				
				// "Official" cartridges (at this places) inactive...
				if (existsSubset (_CARLOW1CON_SUBSET))
					subset (_CARLOW1CON_SUBSET) -> setActive (false);
				if (existsSubset (_CARLOW2CON_SUBSET))
					subset (_CARLOW2CON_SUBSET) -> setActive (false);
				
				// RAM behind inactive...
				_RAM3 -> setActive (false);
			}

			break;

		// FUNCTION LOW not connected (in C16/C116)
		case 1:
			{
				// BASIC ROM inactive...
				_basicROM -> setActive (false);

				// "Empty" cartridges active...
				_carlow1 -> setActive (true);
				if (existsSubset (_CARLOW1CON_SUBSET))
					subset (_CARLOW1CON_SUBSET) -> setActive (false);
				_carlow2 -> setActive (true);
				if (existsSubset (_CARLOW2CON_SUBSET))
					subset (_CARLOW2CON_SUBSET) -> setActive (false);
				
				// but also RAM behind inactive...
				_RAM3 -> setActive (false);
			}

			break;

		// LOROM = CARTRIDGE 1 LOW
		case 2:
			{
				// BASIC ROM inactive
				_basicROM -> setActive (false);

				// "Official" Cartridge 1 low active...if exists!
				// And if doesn't the "Empty" one is activated instead...
				if (existsSubset (_CARLOW1CON_SUBSET))
				{
					_carlow1 -> setActive (false);

					subset (_CARLOW1CON_SUBSET) -> setActive (true);
				}
				else
					_carlow1 -> setActive (true);

				// "Official" Cartridge 2 low inactive...
				_carlow2 -> setActive (true);
				if (existsSubset (_CARLOW2CON_SUBSET))
					subset (_CARLOW2CON_SUBSET) -> setActive (false);

				// RAM behind inactive
				_RAM3 -> setActive (false);
			}

			break;

		// LOROM = CARTRIDGE 2 LOW
		case 3:
			{ 
				// BASIC ROM inactive
				_basicROM -> setActive (false);

				// "Official" Cartridge 1 low inactive...
				_carlow1 -> setActive (true);
				if (existsSubset (_CARLOW1CON_SUBSET))
					subset (_CARLOW1CON_SUBSET) -> setActive (false);

				// "Official" Cartridge 2 low active...if exists!
				// And if doesn't the "Empty" one is activated instead...
				if (existsSubset (_CARLOW2CON_SUBSET))
				{
					_carlow2 -> setActive (false);

					subset (_CARLOW2CON_SUBSET) -> setActive (true);
				}
				else
					_carlow2 -> setActive (true);

				// RAM behind inactive
				_RAM3 -> setActive (false);
			}

			break;

		default:
			// This stiation will never happen (the first instruction is &0x03)
			break;
	}

	// HI ROMS
	switch ((_configuration.value () & 0x0c) >> 2)
	{
		// HIROM = KERNEL
		case 0:
			{
				// Kernel ROM active...
				_kernelROM1 -> setActive (true);
				_kernelROM2 -> setActive (true); // Always...
				_kernelROM3 -> setActive (true);

				// "Empty" cartridges (at this place) inactive...
				_carhigh1 -> setActive (false);
				_carhigh2 -> setActive (false);
				
				// "Official" cartridges (at this places) inactive...
				if (existsSubset (_CARHIGH1CON_SUBSET))
					subset (_CARHIGH1CON_SUBSET) -> setActive (false);
				if (existsSubset (_CARHIGH2CON_SUBSET))
					subset (_CARHIGH2CON_SUBSET) -> setActive (false);
				
				// RAM behind inactive...
				_RAM4 -> setActive (false);
				_RAM5 -> setActive (false);
			}

			break;

		// FUNCTION HI not connected (in C16/C116) = KERNEL
		case 1:
			{
				// Kernel ROM active...
				_kernelROM1 -> setActive (false);
				_kernelROM2 -> setActive (true); // Always...
				_kernelROM3 -> setActive (false);

				// "Empty" cartridges active...
				_carhigh1 -> setActive (true);
				if (existsSubset (_CARHIGH1CON_SUBSET))
					subset (_CARHIGH1CON_SUBSET) -> setActive (false);
				_carhigh2 -> setActive (true);
				if (existsSubset (_CARHIGH2CON_SUBSET))
					subset (_CARHIGH2CON_SUBSET) -> setActive (false);
				
				// RAM behind inactive...
				_RAM4 -> setActive (false);
				_RAM5 -> setActive (false);
			}

			break;

		// HI ROM = CARTRIDGE 1 HI
		case 2:
			{
				// Kernel ROM inactive
				// Notice that kernelROM2 never can go inactive!
				_kernelROM1 -> setActive (false);
				_kernelROM2 -> setActive (true);
				_kernelROM3 -> setActive (false);

				// "Official" Cartridge 1 high active...if exists!
				// And if doesn't the "Empty" one is activated instead...
				if (existsSubset (_CARHIGH1CON_SUBSET))
				{
					_carhigh1 -> setActive (false);

					subset (_CARHIGH1CON_SUBSET) -> setActive (true);
				}
				else
					_carhigh1 -> setActive (true);

				// "Official" Cartridge 2 high inactive...
				_carhigh2 -> setActive (true);
				if (existsSubset (_CARHIGH2CON_SUBSET))
					subset (_CARHIGH2CON_SUBSET) -> setActive (false);

				// RAM behind inactive
				_RAM4 -> setActive (false);
				_RAM5 -> setActive (false);
			}

			break;

		// HO ROM = CARTRIDGE 2 HI
		case 3:
			{ 
				// Kernel ROM inactive
				// Notice that kernelROM2 never can go inactive!
				_kernelROM1 -> setActive (false);
				_kernelROM2 -> setActive (true);
				_kernelROM3 -> setActive (false);

				// "Official" Cartridge 1 high inactive...
				_carhigh1 -> setActive (true);
				if (existsSubset (_CARHIGH1CON_SUBSET))
					subset (_CARHIGH1CON_SUBSET) -> setActive (false);

				// "Official" Cartridge 2 high active...if exists!
				// And if doesn't the "Empty" one is activated instead...
				if (existsSubset (_CARHIGH2CON_SUBSET))
				{
					_carhigh2 -> setActive (false);

					subset (_CARHIGH2CON_SUBSET) -> setActive (true);
				}
				else
					_carhigh2 -> setActive (true);

				// RAM behind inactive
				_RAM4 -> setActive (false);
				_RAM5 -> setActive (false);
			}

			break;

		default:
			// This situation will never happen...
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
		new MCHEmul::PhysicalStorage (_RAM, MCHEmul::PhysicalStorage::Type::_RAM, 0x10000);						// 64k
	// ROM...
	MCHEmul::PhysicalStorage* BASICROM = 
		new MCHEmul::PhysicalStorage (_BASICROM, MCHEmul::PhysicalStorage::Type::_ROM, 0x4000);					// 16k
	MCHEmul::PhysicalStorage* KERNELROM	= 
		new MCHEmul::PhysicalStorage (_KERNELROM, MCHEmul::PhysicalStorage::Type::_ROM, 0x4000);				// 16k

	// The map of phisical storages, used later...
	MCHEmul::PhysicalStorages storages (
		{
			{ _RAM, RAM },
			{ _BASICROM, BASICROM },
			{ _KERNELROM, KERNELROM }
		});

	// Subsets
	// The two registers of the IO expansion port...
	MCHEmul::PhysicalStorageSubset* IO7501Port = 
		new F6500::IO7501PortRegisters (_IO7501PORT_SUBSET, RAM);

	// Page 0: The quickest possible access...
	MCHEmul::PhysicalStorageSubset* PageZero = new MCHEmul::PhysicalStorageSubset 
		(_PAGEZERO_SUBSET, RAM, 0x0002, MCHEmul::Address ({ 0x02, 0x00 }, false), 0xfe);
	// Page 1: Stack, Where the CPU stores info...
	MCHEmul::Stack* Stack = new MCHEmul::Stack 
		(_STACK_SUBSET, RAM, 0x0100, MCHEmul::Address ({ 0x00, 0x01 }, false), 0x0100,
			MCHEmul::Stack::Configuration (true, true /** Pointing to the first empty position. */, 
				false /** No overflow detection. */, -1));
	// ...and basic RAM...(divided in blocks of 16k)
	MCHEmul::PhysicalStorageSubset* RAM1 = new MCHEmul::PhysicalStorageSubset 
		(_RAM1_SUBSET, RAM, 0x0200, MCHEmul::Address ({ 0x00, 0x02 }, false), 0x3e00);							// 16k - 512 bytes (2 previous pages)
	MCHEmul::PhysicalStorageSubset* RAM2 = new MCHEmul::PhysicalStorageSubset 
		(_RAM2_SUBSET, RAM, 0x4000, MCHEmul::Address ({ 0x00, 0x40 }, false), 0x4000);							// 16k

	// BASIC is just now..
	MCHEmul::PhysicalStorageSubset* BasicROM = new MCHEmul::PhysicalStorageSubset 
		(_BASICROM_SUBSET, BASICROM, 0x0000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000);					// 16k ROM
	// But it can be also RAM...
	MCHEmul::PhysicalStorageSubset* RAM3 = new MCHEmul::PhysicalStorageSubset 
		(_RAM3_SUBSET, RAM, 0x8000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000);							// 16k
	// ...and also place for cartridges (2!!), initially that possibility is off!
	MCHEmul::PhysicalStorageSubset* CARLOW1 = new MCHEmul::EmptyPhysicalStorageSubset 
		(_CARLOW1_SUBSET, MCHEmul::UByte::_0, RAM, 0x8000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x2000);	// 8k
	MCHEmul::PhysicalStorageSubset* CARLOW2 = new MCHEmul::EmptyPhysicalStorageSubset 
		(_CARLOW2_SUBSET, MCHEmul::UByte::_0, RAM, 0xa000, MCHEmul::Address ({ 0x00, 0xa0 }, false), 0x2000);	// 8k

	// Where the Kernel ROM starts
	// Within this block thre is: 
	// BASIC Expansion, then the CHARS definition, and the the KERNEL itself!
	MCHEmul::PhysicalStorageSubset* KernelROM1 = new MCHEmul::PhysicalStorageSubset 
		(_KERNELROM1_SUBSET, KERNELROM, 0x0000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x3c00);				// 16k - 1.024 bytes...
	// This part of the kernel can not be take off ever when playing with ROMs! (as far as it is defined, and i understood)
	MCHEmul::PhysicalStorageSubset* KernelROM2 = new MCHEmul::PhysicalStorageSubset 
		(_KERNELROM2_SUBSET, KERNELROM, 0x3c00, MCHEmul::Address ({ 0x00, 0xfc }, false), 0x0100);				// 256...
	// But again there can be RAM behind!
	MCHEmul::PhysicalStorageSubset* RAM4 = new MCHEmul::PhysicalStorageSubset 
		(_RAM4_SUBSET, RAM, 0xc000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x3d00);							// 16k - 768 bytes...
	// ...and also place for more cartridges (other 2!!), initially that possibility is also off!
	MCHEmul::PhysicalStorageSubset* CARHIGH1 = new MCHEmul::EmptyPhysicalStorageSubset 
		(_CARHIGH1_SUBSET, MCHEmul::UByte::_0, RAM, 0xc000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x2000);	// 8k
	MCHEmul::PhysicalStorageSubset* CARHIGH2 = new MCHEmul::EmptyPhysicalStorageSubset 
		(_CARHIGH2_SUBSET, MCHEmul::UByte::_0, RAM, 0xe000, MCHEmul::Address ({ 0x00, 0xe0 }, false), 0x1c00);	// 8k - 1.024 bytes...

	// The I/O Zone...
	// This zone can never be switched off...
	// The only common pieces to all C264 computers are the TED
	// and the 6529B chip managing the keyboard...
	// From $fd00 to $fd0f (0x10) could be either ACIA (in CPlus4) or ¿RAM? (in C16/116)
	// From $fd10 to $fd1f (0x10) could be either C6529B1 (in CPlus4) or ¿RAM? (in C16/116)
	// From $fd20 to $fd2f (0x10) it is not mapped!...What is the behaviour? = RAM...
	MCHEmul::PhysicalStorageSubset* IORAM0 = new MCHEmul::EmptyPhysicalStorageSubset 
		(_IORAM0_SUBSET, MCHEmul::UByte::_0, RAM, 0xfd20, MCHEmul::Address ({ 0x20, 0xfd }, false), 0x10);	// 16 bytes
	// From $fd30 to $fd3f (0x10) there is very simple chip connected to the keyboard matrix...
	MCHEmul::PhysicalStorageSubset* IO6529B1 = new C264::C6529B1Registers 
		(RAM, 0xfd30, MCHEmul::Address ({ 0x30, 0xfd }, false), 0x10);								// C6529B (1) ...for the keyboard! 16 bytes
	// From $fd40 to $fdcf (0x90) it is not mapped!...What is the behaviour? = RAM...
	MCHEmul::PhysicalStorageSubset* IORAM1 = new MCHEmul::EmptyPhysicalStorageSubset 
		(_IORAM1_SUBSET, MCHEmul::UByte::_0, RAM, 0xfd40, MCHEmul::Address ({ 0x40, 0xfd }, false), 0x90);	// 144 bytes
	// From $fdd0 to $fddf (0x10) is clear to switch on/off the different ROM/RAMs of the computer...
	MCHEmul::PhysicalStorageSubset* ROMRAMSWITCH = new C264::ROMRAMSwitchRegisters
		(RAM, 0xfdd0, MCHEmul::Address ({ 0xd0, 0xfd }, false), 0x10);								// 16 bytes
	// From $fde0 to $febf (0xe0) it is not mapped!.. What is the behaviour? = RAM...
	MCHEmul::PhysicalStorageSubset* IORAM2 = new MCHEmul::EmptyPhysicalStorageSubset 
		(_IORAM2_SUBSET, MCHEmul::UByte::_0, RAM, 0xfde0, MCHEmul::Address ({ 0xe0, 0xfd }, false), 0xe0);	// 224 bytes
	// The TIA1 registers (0xfec0 - 0xfedf) appear just only when the drivers are connected... In other case, It will be RAM space?
	MCHEmul::PhysicalStorageSubset* IORAM3 = new MCHEmul::EmptyPhysicalStorageSubset 
		(_IORAM3_SUBSET, MCHEmul::UByte::_0, RAM, 0xfec0, MCHEmul::Address ({ 0xc0, 0xfe }, false), 0x20);	// 32 bytes
	// The TIA2 registers (0xfee0 - 0xfeff) appear just only when the drivers are connected... In other case, It will be RAM space?
	MCHEmul::PhysicalStorageSubset* IORAM4 = new MCHEmul::EmptyPhysicalStorageSubset 
		(_IORAM4_SUBSET, MCHEmul::UByte::_0, RAM, 0xfee0, MCHEmul::Address ({ 0xe0, 0xfe }, false), 0x20);	// 32 bytes
	// The TED within the C264...it can access to more bytes of memory...
	MCHEmul::PhysicalStorageSubset* IOTED = new C264::TEDRegisters 
		(RAM, 0xff00, MCHEmul::Address ({ 0x00, 0xff }, false), 0x40);								// TED in C264 = 64 bytes

	// The rest is also ROM...
	MCHEmul::PhysicalStorageSubset* KernelROM3 = new MCHEmul::PhysicalStorageSubset 
		(_KERNELROM3_SUBSET, KERNELROM, 0x3f40, MCHEmul::Address ({ 0x40, 0xff }, false), 0xc0);	// 192 bytes...
	// ...but it also can be RAM...
	MCHEmul::PhysicalStorageSubset* RAM5 = new MCHEmul::PhysicalStorageSubset 
		(_RAM5_SUBSET, RAM, 0xff40, MCHEmul::Address ({ 0x40, 0xff }, false), 0xc0);				// 192 bytes

	// A map with all the subsets possible...
	MCHEmul::PhysicalStorageSubsets allsubsets (
		{
			{ _IO7501PORT_SUBSET,										IO7501Port }, 
			{ _PAGEZERO_SUBSET,											PageZero }, 
			{ _STACK_SUBSET,											Stack },
			{ _RAM1_SUBSET,												RAM1 },
			{ _RAM2_SUBSET,												RAM2 },
			{ _BASICROM_SUBSET,											BasicROM },
			{ _RAM3_SUBSET,												RAM3 },
			{ _CARLOW1_SUBSET,											CARLOW1 },
			{ _CARLOW2_SUBSET,											CARLOW2 },
			{ _KERNELROM1_SUBSET,										KernelROM1 },
			{ _KERNELROM2_SUBSET,										KernelROM2 },
			{ _RAM4_SUBSET,												RAM4 },
			{ _CARHIGH1_SUBSET,											CARHIGH1 },
			{ _CARHIGH2_SUBSET,											CARHIGH2 },
			{ _IORAM0_SUBSET,											IORAM0 },
			{ C264::C6529B1Registers::_C6529B1REGS_SUBSET,				IO6529B1 },
			{ _IORAM1_SUBSET,											IORAM1 },
			{ C264::ROMRAMSwitchRegisters::_ROMRAMSWITCHREGS_SUBSET,	ROMRAMSWITCH },
			{ _IORAM2_SUBSET,											IORAM2 },
			{ _IORAM3_SUBSET,											IORAM3 },
			{ _IORAM4_SUBSET,											IORAM4 },
			{ COMMODORE::TEDRegisters::_TEDREGS_SUBSET,					IOTED },
			{ _KERNELROM3_SUBSET,										KernelROM3 },
			{ _RAM5_SUBSET,												RAM5 }
		});

	// The views are not created...
	// because when the view is created the memory positions are moved plain
	// and at this point there might be some subsets pending to be created...

	// ...and finally the memory that is the result...
	MCHEmul::Memory::Content result;
	result._physicalStorages = storages;
	result._subsets = allsubsets;

	return (result);
}

// ---
C264::C16_116Memory::C16_116Memory (const std::string& lang)
	: C264::Memory (C264::C16_116Memory::standardMemoryContent (), lang)
{
	setConfiguration (_configuration);
}

// ---
MCHEmul::Memory::Content C264::C16_116Memory::standardMemoryContent ()
{
	MCHEmul::Memory::Content result = 
		std::move (C264::Memory::standardMemoryContent ());

	// Add the new element to the subsets...
	// There is ACIA so an no mapped place is installed instead...
	result._subsets.insert 
		(MCHEmul::PhysicalStorageSubsets::value_type 
			(COMMODORE::ACIARegisters::_ACIAREGS_SUBSET, 
				new MCHEmul::EmptyPhysicalStorageSubset 
					(COMMODORE::ACIARegisters::_ACIAREGS_SUBSET, // RAM instead...
						MCHEmul::UByte::_0, result.physicalStorage (_RAM), 0xfd00, MCHEmul::Address ({ 0x00, 0xfd }, false), 0x10)));
	// It doesn't exist a second C6529 register so a no mapped place is created also instead...
	result._subsets.insert 
		(MCHEmul::PhysicalStorageSubsets::value_type 
			(C264::C6529B2Registers::_C6529B2REGS_SUBSET, 
				new MCHEmul::EmptyPhysicalStorageSubset 
					(C264::C6529B2Registers::_C6529B2REGS_SUBSET, // RAM instead...
						MCHEmul::UByte::_0, result.physicalStorage (_RAM), 0xfd10, MCHEmul::Address ({ 0x10, 0xfd }, false), 0x10)));

	// Then the views...
	result._views = MCHEmul::MemoryViews (
		{
			{ _CPU_VIEW, new MCHEmul::MemoryView (_CPU_VIEW, result._subsets) }
		});

	return (result);
}

// ---
C264::CPlus4Memory::CPlus4Memory (const std::string& lang)
	: C264::Memory (C264::CPlus4Memory::standardMemoryContent (), lang),
	  _functionLow (nullptr), _functionHigh1 (nullptr)
{
	// In the content...
	if (error () != MCHEmul::_NOERROR)
		return;

	_functionLow = subset (_FUNCTIONLOW_SUBSET);
	_functionHigh1 = subset (_FUNCTIONHIGH1_SUBSET);

	// The roms with the standard code for this machine!
	std::string FUNCTIONLOWFILE = "./CP4/3-plus-1.317053-01.bin";
	std::string FUNCTIONHIGHFILE = "./CP4/3-plus-1.317054-01.bin";

	// In this case there different options per languaje...

	bool ok = true;
	ok &= physicalStorage (_FUNCTIONLOW) -> loadInto (FUNCTIONLOWFILE);
	subset (_FUNCTIONLOW_SUBSET) -> fixDefaultValues (); // Fix the values for further initializations...
	ok &= physicalStorage (_FUNCTIONHIGH) -> loadInto (FUNCTIONHIGHFILE);
	subset (_FUNCTIONHIGH1_SUBSET) -> fixDefaultValues ();

	if (!ok)
		_error = MCHEmul::_INIT_ERROR;

	setConfiguration (_configuration);
}

// ---
void C264::CPlus4Memory::setConfiguration (const MCHEmul::UByte& cfg)
{
	// Meaning all RAM?
	if (cfg == MCHEmul::UByte::_FF)
	{
		C264::Memory::setConfiguration (cfg); // The variable "configuration" is changed here...

		// Apart of the things done at parent level, 
		// the functions are also desactivated...
		_functionLow	-> setActive (false);
		_functionHigh1	-> setActive (false);

		return;
	}

	switch (cfg.value () & 0x03)
	{
		// In this case the low functions must be activated...
		case 1:
			{
				_configuration = cfg;

				// BASIC ROM inactive...
				_basicROM -> setActive (false);

				// low Function active
				_functionLow -> setActive (true);

				// "Empty" cartridges (at this place) inactive...
				_carlow1 -> setActive (false);
				_carlow2 -> setActive (false);
				
				// "Official" cartridges (at this places) inactive...
				if (existsSubset (_CARLOW1CON_SUBSET))
					subset (_CARLOW1CON_SUBSET) -> setActive (false);
				if (existsSubset (_CARLOW2CON_SUBSET))
					subset (_CARLOW2CON_SUBSET) -> setActive (false);
				
				// RAM behind inactive...
				_RAM3 -> setActive (false);
			}

			break;

		default:
			{
				C264::Memory::setConfiguration (cfg); // The variable "configuration" is changed here...

				// Apart of what it has to be done...
				// ...the function has to be disconnected!
				_functionLow -> setActive (false);
			}

			break;
	}

	switch ((cfg.value () & 0x0c) >> 2)
	{
		// In this case the high functions must be activated also!
		case 1:
			{
				_configuration = cfg;

				// Kernel ROM inactive...
				// Notice that Kernel 2 remains active!
				_kernelROM1 -> setActive (false);
				_kernelROM2 -> setActive (true);
				_kernelROM3 -> setActive (false);

				// Function high active...
				_functionHigh1 -> setActive (true);

				// "Empty" cartridges (at this place) inactive...
				_carhigh1 -> setActive (false);
				_carhigh2 -> setActive (false);
				
				// "Official" cartridges (at this places) inactive...
				if (existsSubset (_CARHIGH1CON_SUBSET))
					subset (_CARHIGH1CON_SUBSET) -> setActive (false);
				if (existsSubset (_CARHIGH2CON_SUBSET))
					subset (_CARHIGH2CON_SUBSET) -> setActive (false);
				
				// RAM behind inactive...
				_RAM4 -> setActive (false);
				_RAM5 -> setActive (false);
			}

			break;

		// This situation is always invoked!
		default:
			{
				C264::Memory::setConfiguration (cfg); // The variable "configuration" is changed here...

				// Apart of what it has to be done...
				// ...the function has to be disconnected!
				_functionHigh1 -> setActive (false);
			}
			
			break;
	}
}

// ---
MCHEmul::Memory::Content C264::CPlus4Memory::standardMemoryContent ()
{
	MCHEmul::Memory::Content result = 
		std::move (C264::Memory::standardMemoryContent ());

	// Add new physical storage for the predefined code...
	MCHEmul::PhysicalStorage* FUNCLOW = new MCHEmul::PhysicalStorage 
		(_FUNCTIONLOW, MCHEmul::PhysicalStorage::Type::_ROM, 0x4000);		// 16k...
	result._physicalStorages.insert (MCHEmul::PhysicalStorages::value_type (_FUNCTIONLOW, FUNCLOW));
	MCHEmul::PhysicalStorage* FUNCHIGH = new MCHEmul::PhysicalStorage 
		(_FUNCTIONHIGH, MCHEmul::PhysicalStorage::Type::_ROM, 0x4000);		// 16k...
	result._physicalStorages.insert (MCHEmul::PhysicalStorages::value_type (_FUNCTIONHIGH, FUNCHIGH));

	// Add the new element to the subsets...
	// The Function Low...
	result._subsets.insert 
		(MCHEmul::PhysicalStorageSubsets::value_type
			(_FUNCTIONLOW_SUBSET, new MCHEmul::PhysicalStorageSubset 
				(_FUNCTIONLOW_SUBSET, FUNCLOW, 0x0000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000)));
	// The function High...
	result._subsets.insert 
		(MCHEmul::PhysicalStorageSubsets::value_type
			(_FUNCTIONHIGH1_SUBSET, new MCHEmul::PhysicalStorageSubset 
				(_FUNCTIONHIGH1_SUBSET, FUNCHIGH, 0x0000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x3c00)));
	// The ACIA chip (used in communications)
	result._subsets.insert 
		(MCHEmul::PhysicalStorageSubsets::value_type 
			(COMMODORE::ACIARegisters::_ACIAREGS_SUBSET, 
				new COMMODORE::ACIARegisters (result.physicalStorage (_RAM), // The ACIA...
					0xfd00, MCHEmul::Address ({ 0x00, 0xfd }, false), 0x10)));
	// The C6529 (2) chip, which use is still unknown...
	result._subsets.insert
		(MCHEmul::PhysicalStorageSubsets::value_type 
			(C264::C6529B2Registers::_C6529B2REGS_SUBSET,
				new C264::C6529B2Registers (result.physicalStorage (_RAM), // And another latch chip...
					0xfd10, MCHEmul::Address ({ 0x10, 0xfd }, false), 0x10)));

	// Finally, creates the view...
	result._views = MCHEmul::MemoryViews (
		{
			{ _CPU_VIEW, new MCHEmul::MemoryView (_CPU_VIEW, result._subsets) }
		});

	return (result);
}
