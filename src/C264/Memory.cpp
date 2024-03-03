#include <C264/Memory.hpp>
#include <C264/C6529B1Registers.hpp>
#include <C264/C6529B2Registers.hpp>
#include <C264/TED.hpp>
#include <C264/TEDRegisters.hpp>

// ---
C264::Memory::Memory (unsigned int cfg, const MCHEmul::Memory::Content& cnt, const std::string& lang)
	: MCHEmul::Memory (0, cnt, { }),
	  _configuration (cfg),
	  _RAM1 (nullptr), // Initialized later...
	  _RAM2 (nullptr),
	  _RAM3 (nullptr),
	  _RAM4 (nullptr),
	  _RAM5 (nullptr),
	  _basicROM (nullptr),
	  _kernelROM1 (nullptr),
	  _kernelROM2 (nullptr)
{
	// In the content...
	if (error () != MCHEmul::_NOERROR)
		return;

	_basicROM			= subset (_BASICROM_SUBSET);
	_kernelROM1			= subset (_KERNELROM1_SUBSET);
	_kernelROM2			= subset (_KERNELROM2_SUBSET);
	_RAM1				= subset (_RAM1_SUBSET);
	_RAM2				= subset (_RAM2_SUBSET);
	_RAM3				= subset (_RAM3_SUBSET);
	_RAM4				= subset (_RAM4_SUBSET);
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

	if (!ok)
		_error = MCHEmul::_INIT_ERROR;

	// The configuration is not set, 
	// as it is done in the classes inheriting this one...
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
		new MCHEmul::PhysicalStorage (_RAM, MCHEmul::PhysicalStorage::Type::_RAM, 0x10000);				// 64k
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
	// The two registers of the IO Expansion port...
	MCHEmul::PhysicalStorageSubset* IO7501Port = 
		new F6500::IO7501PortRegisters (_IO7501PORT_SUBSET, RAM);
	// Page 0: The quickest possible access...
	MCHEmul::PhysicalStorageSubset* PageZero = new MCHEmul::PhysicalStorageSubset 
		(_PAGEZERO_SUBSET, RAM, 0x0002, MCHEmul::Address ({ 0x02, 0x00 }, false), 0xfe);
	// Page 1: Stack, Where the CPU stores info...
	MCHEmul::Stack* Stack = new MCHEmul::Stack 
		(_STACK_SUBSET, RAM, 0x0100, MCHEmul::Address ({ 0x00, 0x01 }, false), 0x0100);

	// Next RAM...
	MCHEmul::PhysicalStorageSubset* RAM1 = new MCHEmul::PhysicalStorageSubset 
		(_RAM1_SUBSET, RAM, 0x0200, MCHEmul::Address ({ 0x00, 0x02 }, false), 0x3e00);				// 16k - 512 bytes...
	MCHEmul::PhysicalStorageSubset* RAM2 = new MCHEmul::PhysicalStorageSubset 
		(_RAM2_SUBSET, RAM, 0x4000, MCHEmul::Address ({ 0x00, 0x40 }, false), 0x4000);				// 16k

	// The Basic is set just after this empty zone...
	MCHEmul::PhysicalStorageSubset* BasicROM = new MCHEmul::PhysicalStorageSubset 
		(_BASICROM_SUBSET, BASICROM, 0x0000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000);		// 16k ROM
	// But it can be also RAM
	MCHEmul::PhysicalStorageSubset* RAM3 = new MCHEmul::PhysicalStorageSubset 
		(_RAM2_SUBSET, RAM, 0x8000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000);				// 16k

	// Where the BASIC expansions can be...
	MCHEmul::PhysicalStorageSubset* RAM4 = new MCHEmul::PhysicalStorageSubset 
		(_RAM4_SUBSET, RAM, 0xc000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x3d00);				// 16k - 786 bytes...
	// But also can be ROM... (it is by default)
	MCHEmul::PhysicalStorageSubset* KernelROM1 = new MCHEmul::PhysicalStorageSubset 
		(_KERNELROM1_SUBSET, KERNELROM, 0x0000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x3d00);	// 16k - 786 bytes...

	// The I/O Zone...
	// The only common pieces to all C264 computers are the TED
	// and the 6529B chip managing the keyboard...
	// From $fd00 to $fd0f (0x10) could be either ACIA (in CPlus4) or ¿RAM? (in C16/116)
	// From $fd10 to $fd1f (0x10) could be either C6529B1 (in CPlus4) or ¿RAM? (in C16/116)
	// From $fd20 to $fd2f (0x10) it is not mapped!...What is the behaviour? = RAM...
	MCHEmul::PhysicalStorageSubset* IORAM0 = new MCHEmul::PhysicalStorageSubset 
		(_IORAM0_SUBSET, RAM, 0xfd20, MCHEmul::Address ({ 0x20, 0xfd }, false), 0x10);				// 16 bytes
	// From $fd30 to $fd3f (0x10) there is very simple chip connected to the keyboard matrix...
	MCHEmul::PhysicalStorageSubset* IO6529B1 = new C264::C6529B1Registers 
		(RAM, 0xfd30, MCHEmul::Address ({ 0x30, 0xfd }, false), 0x10);								// C6529B (1) ...for the keyboard! 16 bytes
	// From $fd40 to $fdcf (0x90) it is not mapped!...What is the behaviour? = RAM...
	MCHEmul::PhysicalStorageSubset* IORAM1 = new MCHEmul::PhysicalStorageSubset 
		(_IORAM1_SUBSET, RAM, 0xfd40, MCHEmul::Address ({ 0x40, 0xfd }, false), 0x90);				// 144 bytes
	// From $fdd0 to $fddf (0x10) something related with the communication but not well defined so far ... = RAM so far...
	MCHEmul::PhysicalStorageSubset* IORAM2 = new MCHEmul::PhysicalStorageSubset 
		(_IORAM2_SUBSET, RAM, 0xfdd0, MCHEmul::Address ({ 0xd0, 0xfd }, false), 0x10);				// 16 bytes
	// From $fde0 to $febf (0xe0) it is not mapped!.. What is the behaviour? = RAM...
	MCHEmul::PhysicalStorageSubset* IORAM3 = new MCHEmul::PhysicalStorageSubset 
		(_IORAM3_SUBSET, RAM, 0xfde0, MCHEmul::Address ({ 0xe0, 0xfd }, false), 0xe0);				// 224 bytes
	// The TIA1 registers (0xfec0 - 0xfedf) appear just only when the drivers are connected... In other case, It will be RAM space?
	MCHEmul::PhysicalStorageSubset* IORAM4 = new MCHEmul::PhysicalStorageSubset 
		(_IORAM4_SUBSET, RAM, 0xfec0, MCHEmul::Address ({ 0xc0, 0xfe }, false), 0x20);				// 32 bytes
	// The TIA2 registers (0xfee0 - 0xfeff) appear just only when the drivers are connected... In other case, It will be RAM space?
	MCHEmul::PhysicalStorageSubset* IORAM5 = new MCHEmul::PhysicalStorageSubset 
		(_IORAM5_SUBSET, RAM, 0xfee0, MCHEmul::Address ({ 0xe0, 0xfe }, false), 0x20);				// 32 bytes
	// The TED within the C264...it can access to more bytes of memory...
	MCHEmul::PhysicalStorageSubset* IOTED = new C264::TEDRegisters 
		(RAM, 0xff00, MCHEmul::Address ({ 0x00, 0xff }, false), 0x40);								// TED in C264 = 64 bytes

	// The rest of the RAM...
	MCHEmul::PhysicalStorageSubset* RAM5 = new MCHEmul::PhysicalStorageSubset 
		(_RAM5_SUBSET, RAM, 0xff40, MCHEmul::Address ({ 0x40, 0xff }, false), 0xc0);				// 192 bytes
	// But also can be ROM...
	MCHEmul::PhysicalStorageSubset* KernelROM2 = new MCHEmul::PhysicalStorageSubset 
		(_KERNELROM2_SUBSET, KERNELROM, 0x3f40, MCHEmul::Address ({ 0x40, 0xff }, false), 0xc0);	// 192 bytes...

	// A map with all the subsets possible...
	MCHEmul::PhysicalStorageSubsets allsubsets (
		{
			{ _IO7501PORT_SUBSET,								IO7501Port }, 
			{ _PAGEZERO_SUBSET,									PageZero }, 
			{ _STACK_SUBSET,									Stack },
			{ _RAM1_SUBSET,										RAM1 },
			{ _RAM2_SUBSET,										RAM2 },
			{ _RAM3_SUBSET,										RAM3 },
			{ _BASICROM_SUBSET,									BasicROM },
			{ _RAM4_SUBSET,										RAM4 },
			{ _KERNELROM1_SUBSET,								KernelROM1 },
			{ C264::C6529B1Registers::_C6529B1REGS_SUBSET,		IO6529B1 },
			{ _IORAM1_SUBSET,									IORAM1 },
			{ _IORAM2_SUBSET,									IORAM2 },
			{ _IORAM3_SUBSET,									IORAM3 },
			{ _IORAM4_SUBSET,									IORAM4 },
			{ _IORAM5_SUBSET,									IORAM5 },
			{ COMMODORE::TEDRegisters::_TEDREGS_SUBSET,			IOTED },
			{ _RAM5_SUBSET,										RAM5 },
			{ _KERNELROM2_SUBSET,								KernelROM2 },
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
C264::C16_116Memory::C16_116Memory (unsigned int cfg, const std::string& lang)
	: C264::Memory (cfg, C264::C16_116Memory::standardMemoryContent (), lang)
{
	setConfiguration (_configuration);
}

// ---
void C264::C16_116Memory::setConfiguration (unsigned int cfg)
{
	// Attending to the type of machine different options are active or not active!
	switch (_configuration = cfg)
	{
		// 16k
		case 0:
			{
				_RAM1		-> setActive (true);
				_RAM2		-> setActive (false);
				_RAM3		-> setActive (false);
				_RAM4		-> setActive (false);
				_RAM5		-> setActive (false);
				_basicROM	-> setActive (true);
				_kernelROM1	-> setActive (true);
				_kernelROM2	-> setActive (true);
			}

			break;

		// 32k
		case 1:
			{
				_RAM1		-> setActive (true);
				_RAM2		-> setActive (true);
				_RAM3		-> setActive (false);
				_RAM4		-> setActive (false);
				_RAM5		-> setActive (false);
				_basicROM	-> setActive (true);
				_kernelROM1	-> setActive (true);
				_kernelROM2	-> setActive (true);
			}

			break;

		// 48k
		case 2:
			{
				_RAM1		-> setActive (true);
				_RAM2		-> setActive (true);
				_RAM3		-> setActive (true);
				_RAM4		-> setActive (false);
				_RAM5		-> setActive (false);
				_basicROM	-> setActive (false);
				_kernelROM1	-> setActive (true);
				_kernelROM2	-> setActive (true);
			}

			break;

		// 64k
		case 3:
			{
				_RAM1		-> setActive (true);
				_RAM2		-> setActive (true);
				_RAM3		-> setActive (true);
				_RAM4		-> setActive (true);
				_RAM5		-> setActive (true);
				_basicROM	-> setActive (false);
				_kernelROM1	-> setActive (false);
				_kernelROM2	-> setActive (false);
			}

			break;

		default:
			// It shouldn't exist, but just in case...
			assert (false);
			break;
	}
}

// ---
MCHEmul::Memory::Content C264::C16_116Memory::standardMemoryContent ()
{
	MCHEmul::Memory::Content result = 
		std::move (C264::Memory::standardMemoryContent ());

	// Add the new element to the subsets...
	result._subsets.insert 
		(MCHEmul::PhysicalStorageSubsets::value_type 
			(COMMODORE::ACIARegisters::_ACIAREGS_SUBSET, 
				new MCHEmul::PhysicalStorageSubset (COMMODORE::ACIARegisters::_ACIAREGS_SUBSET, // RAM instead...
					result.physicalStorage (_RAM), 0xfd00, MCHEmul::Address ({ 0x00, 0xfd }, false), 0x10)));
	result._subsets.insert 
		(MCHEmul::PhysicalStorageSubsets::value_type 
			(C264::C6529B2Registers::_C6529B2REGS_SUBSET, 
				new MCHEmul::PhysicalStorageSubset (C264::C6529B2Registers::_C6529B2REGS_SUBSET, // RAM instead...
					result.physicalStorage (_RAM), 0xfd10, MCHEmul::Address ({ 0x10, 0xfd }, false), 0x10)));

	// Then the views...
	result._views = MCHEmul::MemoryViews (
		{
			{ _CPU_VIEW, new MCHEmul::MemoryView (_CPU_VIEW, result._subsets) }
		});

	return (result);
}

// ---
C264::CPlus4Memory::CPlus4Memory (unsigned int cfg, const std::string& lang)
	: C264::Memory (cfg, C264::CPlus4Memory::standardMemoryContent (), lang),
	  _code1 (nullptr),
	  _code2 (nullptr)
{
	// In the content...
	if (error () != MCHEmul::_NOERROR)
		return;

	_code1 = subset (_BASICROM_SUBSET);
	_code2 = subset (_KERNELROM1_SUBSET);

	// The roms with the standard code for this machine!
	std::string CODE1FILE = "./CP4/3-plus-1.317053-01.bin";
	std::string CODE2FILE = "./CP4/3-plus-1.317054-01.bin";

	// In this case there different options per languaje...

	bool ok = true;
	ok &= physicalStorage (_BASICROM) -> loadInto (CODE1FILE);
	subset (_BASICROM_SUBSET) -> fixDefaultValues (); // Fix the values for further initializations...
	ok &= subset (_KERNELROM) -> loadInto (CODE2FILE);
	subset (_KERNELROM1_SUBSET) -> fixDefaultValues ();
	subset (_KERNELROM2_SUBSET) -> fixDefaultValues ();

	if (!ok)
		_error = MCHEmul::_INIT_ERROR;

	setConfiguration (_configuration);
}

// ---
void C264::CPlus4Memory::setConfiguration (unsigned int cfg)
{
	// The configuration is always the same...
	// There is only 32K for memory, 
	// the rest is ocuppied by the standard programs!

	_RAM1		-> setActive (true);
	_RAM2		-> setActive (true);
	_RAM3		-> setActive (false);
	_RAM4		-> setActive (false);
	_RAM5		-> setActive (false);
	_basicROM	-> setActive (true);
	_kernelROM1	-> setActive (true);
	_kernelROM2	-> setActive (true);
}

// ---
MCHEmul::Memory::Content C264::CPlus4Memory::standardMemoryContent ()
{
	MCHEmul::Memory::Content result = 
		std::move (C264::Memory::standardMemoryContent ());

	// Add the new element to the subsets...
	result.subsets ().insert 
		(MCHEmul::PhysicalStorageSubsets::value_type 
			(COMMODORE::ACIARegisters::_ACIAREGS_SUBSET, 
				new COMMODORE::ACIARegisters (result.physicalStorage (_RAM), // The ACIA...
					0xfd00, MCHEmul::Address ({ 0x00, 0xfd }, false), 0x10)));
	result.subsets ().insert
		(MCHEmul::PhysicalStorageSubsets::value_type 
			(COMMODORE::C6529BRegisters::_C6529BREGS_SUBSET, 
				new C264::C6529B2Registers (result.physicalStorage (_RAM), // And another latch chip...
					0xfd10, MCHEmul::Address ({ 0x10, 0xfd }, false), 0x10)));

	// Then the views...
	result._views = MCHEmul::MemoryViews (
		{
			{ _CPU_VIEW, new MCHEmul::MemoryView (_CPU_VIEW, result._subsets) }
		});

	return (result);
}
