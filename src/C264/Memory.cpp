#include <C264/Memory.hpp>

// ---
C264::Memory::Memory (const MCHEmul::Memory::Content& cnt, 
		C264::Type t, unsigned int cfg, const std::string& lang)
	: MCHEmul::Memory (0, cnt, { }),
	  _type (t),
	  _configuration (cfg),
	  _ROMActive (true), // By default the ROM is active...
	  _C1Low (false), _C1High (false),
	  _C2Low (false), _C2High (false),
	  _RAM1 (nullptr),
	  _RAM2 (nullptr),
	  _EMPTYRAM2 (nullptr),
	  _basicROM (nullptr),
	  _RAM3 (nullptr),
	  _EMPTYRAM3 (nullptr),
	  _lowCartridge (nullptr),
	  _kernelROM1 (nullptr),
	  _RAM4 (nullptr),
	  _EMPTYRAM4 (nullptr),
	  _highCartridge1 (nullptr),
	  _IOnomapped2 (nullptr),
	  _IO6529B1 (nullptr),
	  _IOnomapped3 (nullptr),
	  _IOC1C2Selector (nullptr),
	  _IOnomapped5 (nullptr),
	  _IOTIA9 (nullptr),
	  _IOTIA8 (nullptr),
	  _IOTED (nullptr),
	  _ROMRAMSwitch (nullptr),
	  _kernelROM2 (nullptr),
	  _RAM5 (nullptr),
	  _EMPTYRAM5 (nullptr),
	  _highCartridge2 (nullptr)
{
	// In the content...
	if (error () != MCHEmul::_NOERROR)
		return;

	// Get references to all things tha can change in configuration...
	// Low RAM zones...
	_RAM1				= subset (_RAM1_SUBSET);
	_RAM2				= subset (_RAM2_SUBSET);
	_EMPTYRAM2			= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> 
							(subset (_EMPTYRAM2_SUBSET));		// This zone of the RAM cannot exist...
	// BASIC or RAM or Nothing...
	_basicROM			= subset (_BASICROM_SUBSET);			// Either this...
	_RAM3				= subset (_RAM3_SUBSET);				// or this...
	_EMPTYRAM3			= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> 
							(subset (_EMPTYRAM3_SUBSET));		// ...or this when more RAM that the very basic (RAM1) exists..
	_lowCartridge		= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> 
							(subset (_LOWCARTRIDGE_SUBSET));	// This zone of the memory can be occupied by a cartridge, but empty by default...
	// ROM or RAM or Nothing...
	_kernelROM1			= subset (_KERNELROM1_SUBSET);			// Either this...
	_RAM4				= subset (_RAM4_SUBSET);				// ...or this...
	_EMPTYRAM4			= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> 
							(subset (_EMPTYRAM4_SUBSET));		// ...or this
	_highCartridge1		= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> 
							(subset (_HIGHCARTRIDGE1_SUBSET));	// This zone of the memory can be occupied by a cartridge, but empty by default...
	// IO zone and ROM/RAM switching always on...
	_IOnomapped2		= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> 
							(subset (_IONOMAPPED2_SUBSET));
	_IO6529B1			= dynamic_cast <C264::C6529B1Registers*> 
							(subset (C264::C6529B1Registers::_C6529B1REGS_SUBSET));
	_IOnomapped3		= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> 
							(subset (_IONOMAPPED3_SUBSET));
	_IOC1C2Selector		= dynamic_cast <C264::C1C2SelectorRegisters*>
							(subset (C264::C1C2SelectorRegisters::_C1C2SELECTORREGS_SUBSET));
	_IOnomapped5		= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> 
							(subset (_IONOMAPPED5_SUBSET));
	_IOTIA9				= dynamic_cast <COMMODORE::TIARegisters*> 
							(subset (COMMODORE::TIARegisters::_TIAREGS_SUBSET));
	_IOTIA8				= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> 
							(subset (_IOTIA8_SUBSET));
	_IOTED				= dynamic_cast <C264::TEDRegisters*> 
							(subset (C264::TEDRegisters::_TEDREGS_SUBSET));
	_ROMRAMSwitch		= dynamic_cast <C264::ROMRAMSwitchRegisters*> 
							(subset (C264::ROMRAMSwitchRegisters::_ROMRAMSWITCHREGS_SUBSET)); 
	// KERNAL or RAM or Nothing...
	_kernelROM2			= subset (_KERNELROM2_SUBSET);			// Either this...
	_RAM5				= subset (_RAM5_SUBSET);				// or this...
	_EMPTYRAM5			= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> 
							(subset (_EMPTYRAM5_SUBSET));		// ...or this
	_highCartridge2		= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> 
							(subset (_HIGHCARTRIDGE2_SUBSET));	// This zone of the memory can be occupied by a cartridge, but empty by default...

	// None can be nullptr...
	assert (
		_RAM1 != nullptr && 
		_RAM2 != nullptr && 
		_EMPTYRAM2 != nullptr && 
		_basicROM != nullptr && 
		_RAM3 != nullptr &&
		_EMPTYRAM3 != nullptr &&
		_lowCartridge != nullptr &&
		_kernelROM1 != nullptr && 
		_RAM4 != nullptr && 
		_EMPTYRAM4 != nullptr &&
		_highCartridge1 != nullptr &&
		_IOnomapped2 != nullptr &&
		_IO6529B1 != nullptr && 
		_IOnomapped3 != nullptr && 
		_IOC1C2Selector != nullptr &&
		_IOnomapped5 != nullptr && 
		_IOTIA9 != nullptr && 
		_IOTIA8 != nullptr &&
		_IOTED != nullptr && 
		_ROMRAMSwitch != nullptr && 
		_kernelROM2 != nullptr && 
		_RAM5 != nullptr &&
		_EMPTYRAM5 != nullptr &&
		_highCartridge2 != nullptr);

	// The default ROMS...
	// They might change depending on the language
	// By default the English language is taken
	std::string ROMFILE = "./bios/basic.318006-01.bin";
	std::string KERNELFILE = "./bios/kernal.318004-05-ENG.bin";

	// If the languaje selected is not ENG...
	// then another char rom file is selected!
	// The CHARROM here is part of the Kernal
	if (lang == "FRA") { KERNELFILE = "./bios/kernal.318005-05-FRA.bin"; }
	else if (lang == "HUN") { KERNELFILE = "./bios/kernal.318030-02-HUN.bin"; }
	else if (lang == "SWE") { KERNELFILE = "./bios/kernel.325155-03-SWE.bin"; }

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
	// It it were done here, the virtual method won't be invoked with a behaviour not defined...
}

// ---
bool C264::Memory::initialize ()
{
	bool result = MCHEmul::Memory::initialize ();
	if (!result)
		return (false);

	// Sets a temporary configuration with all RAM active, just to fullfilled...
	// Keep the status of the configuration...
	unsigned int oC = configuration ();
	bool oRA = ROMactive ();
	bool oCL1 = C1Low (); bool oCH1 = C1High ();
	bool oCL2 = C2Low (); bool oCH2 = C2High ();
	setConfiguration (1 /** With all memory RAM possible. */, false, false, false, false, false);
	// Just to fillup all RAM with the initial value!
	for (size_t i = 0X000; i < 0x10000; i += 0x40)
		if (i < 0xfd00 || i >= 0xff40) // The IO zone is not filled...
			fillWith (MCHEmul::Address ({ 0x00, 0x00 }) + i, 
				(((i / 0x40) % 2) == 0) ? MCHEmul::UByte::_0 : MCHEmul::UByte::_FF, 0x40);
	// Go back to the same configuration...
	setConfiguration (oC, oRA, oCL1, oCH1, oCL2, oCH2);

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
	// External Cartridges but rmpty that can be added later...
	MCHEmul::PhysicalStorage* LOWCARTRIDGEROM = 
		new MCHEmul::PhysicalStorage (_LOWCARTRIDGEROM, MCHEmul::PhysicalStorage::Type::_ROM, 0x4000);			// 16k
	MCHEmul::PhysicalStorage* HIGHCARTRIDGEROM = 
		new MCHEmul::PhysicalStorage (_HIGHCARTRIDGEROM, MCHEmul::PhysicalStorage::Type::_ROM, 0x4000);			// 16k

	// The map of phisical storages, used later...
	MCHEmul::PhysicalStorages storages (
		{
			{ _RAM, RAM },
			{ _BASICROM, BASICROM },
			{ _KERNELROM, KERNELROM },
			{ _LOWCARTRIDGEROM, LOWCARTRIDGEROM },
			{ _HIGHCARTRIDGEROM, HIGHCARTRIDGEROM }
		});

	// Subsets
	// $0000-$0001: The two registers of the IO expansion port...
	MCHEmul::PhysicalStorageSubset* IO7501Port = 
		new F6500::IO7501PortRegisters (_IO7501PORT_SUBSET, RAM);
	IO7501Port -> setName ("IO7501 Port Registers");

	// $0002-$00FF: Page 0.The quickest possible access...
	MCHEmul::PhysicalStorageSubset* PageZero = new MCHEmul::PhysicalStorageSubset 
		(_PAGEZERO_SUBSET, RAM, 0x0002, MCHEmul::Address ({ 0x02, 0x00 }, false), 0xfe);
	PageZero -> setName ("Page Zero");

	// $0100-$01FF: Page 1.Stack, Where the CPU stores info...
	MCHEmul::Stack* Stack = new MCHEmul::Stack 
		(_STACK_SUBSET, RAM, 0x0100, MCHEmul::Address ({ 0x00, 0x01 }, false), 0x0100,
			MCHEmul::Stack::Configuration (true, true /** Pointing to the first empty position. */, 
				false /** No overflow detection. */, -1));
	Stack -> setName ("Stack");

	// $0200-$7FFF: Basic RAM, divided in blocks of 16ks
	MCHEmul::PhysicalStorageSubset* RAM1 = new MCHEmul::PhysicalStorageSubset 
		(_RAM1_SUBSET, RAM, 0x0200, MCHEmul::Address ({ 0x00, 0x02 }, false), 0x3e00);							// 16k - 512 bytes (2 previous pages)
	RAM1 -> setName ("Basic RAM 1");
	MCHEmul::PhysicalStorageSubset* RAM2 = new MCHEmul::PhysicalStorageSubset 
		(_RAM2_SUBSET, RAM, 0x4000, MCHEmul::Address ({ 0x00, 0x40 }, false), 0x4000);							// 16k
	RAM2 -> setName ("Basic RAM 2");
	// But this piece of the RAM can no be connected if the machine is not extended in RAM...
	MCHEmul::PhysicalStorageSubset* EMPTYRAM2 = new MCHEmul::EmptyPhysicalStorageSubset 
		(_EMPTYRAM2_SUBSET, MCHEmul::UByte::_0, RAM, 0x4000, MCHEmul::Address ({ 0x00, 0x40 }, false), 0x4000);
	EMPTYRAM2 -> setName ("Empty Basic RAM 2");

	// From $8000-$BFFF: BASIC ROM (RAM behind)
	MCHEmul::PhysicalStorageSubset* BasicROM = new MCHEmul::PhysicalStorageSubset 
		(_BASICROM_SUBSET, BASICROM, 0x0000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000);					// 16k ROM
	BasicROM -> setName ("BASIC ROM");
	// But it can be also RAM...
	MCHEmul::PhysicalStorageSubset* RAM3 = new MCHEmul::PhysicalStorageSubset 
		(_RAM3_SUBSET, RAM, 0x8000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000);
	RAM3 -> setName ("BASIC RAM 3");
	// ...or totally empty with nothing connected behind...
	MCHEmul::PhysicalStorageSubset* EMPTYRAM3 = new MCHEmul::EmptyPhysicalStorageSubset 
		(_EMPTYRAM3_SUBSET, MCHEmul::UByte::_0, RAM, 0x8000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000);
	EMPTYRAM3 -> setName ("Empty BASIC RAM 3");
	// This zone of the memory can be occupied by a cartridge, but empty by default...
	MCHEmul::PhysicalStorageSubset* LOWCARTRIDGE = new MCHEmul::EmptyPhysicalStorageSubset 
		(_LOWCARTRIDGE_SUBSET, MCHEmul::UByte::_0, LOWCARTRIDGEROM, 0x0000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000);
	LOWCARTRIDGE -> setName ("Low Cartridge");

	// $C000-$FD00: KERNEL ROM (RAM behind). There is another piece after IO area...
	MCHEmul::PhysicalStorageSubset* KernelROM1 = new MCHEmul::PhysicalStorageSubset 
		(_KERNELROM1_SUBSET, KERNELROM, 0x0000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x3d00);				// 16k - 768 bytes
	KernelROM1 -> setName ("KERNEL ROM 1");
	// But again there can be RAM behind!
	MCHEmul::PhysicalStorageSubset* RAM4 = new MCHEmul::PhysicalStorageSubset 
		(_RAM4_SUBSET, RAM, 0xc000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x3d00);
	RAM4 -> setName ("KERNEL RAM 4");
	// ...or again nothing connected behind...
	MCHEmul::PhysicalStorageSubset* EMPTYRAM4 = new MCHEmul::EmptyPhysicalStorageSubset 
		(_EMPTYRAM4_SUBSET, MCHEmul::UByte::_0, RAM, 0xc000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x3d00);
	EMPTYRAM4 -> setName ("Empty KERNEL RAM 4");
	// This zone of the memory can be occupied by a cartridge, but empty by default...
	MCHEmul::PhysicalStorageSubset* HIGHCARTRIDGE1 = new MCHEmul::EmptyPhysicalStorageSubset 
		(_HIGHCARTRIDGE1_SUBSET, MCHEmul::UByte::_0, HIGHCARTRIDGEROM, 0x0000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x3d00);
	HIGHCARTRIDGE1 -> setName ("High Cartridge 1");

	// The I/O Zone...
	// $FD00-$FD0F (0x10): Could be either ACIA (in CPlus4) or not mapped (in C16/116)
	// $FD10-$FD1F (0x10): Could be either C6529B1 (in CPlus4) or not mapped (in C16/116)
	// These tow first blocks are no created here but depending on the type of machine...
	// $FD20-$FD2F (0x10): It is not mapped!...
	MCHEmul::EmptyPhysicalStorageSubset* IONOMAPPED2 = new MCHEmul::EmptyPhysicalStorageSubset 
		(_IONOMAPPED2_SUBSET, MCHEmul::UByte::_0, RAM, 0xfd20, MCHEmul::Address ({ 0x20, 0xfd }, false), 0x10);	// 16 bytes
	IONOMAPPED2 -> setName ("IO Not Mapped 2");
	// $FD30-$FD3F (0x10): There is very simple chip connected to the keyboard matrix...
	C264::C6529B1Registers* IO6529B1 = new C264::C6529B1Registers 
		(RAM, 0xfd30, MCHEmul::Address ({ 0x30, 0xfd }, false), 0x10);											// C6529B (2) ...for the keyboard! 16 bytes
	IO6529B1 -> setName ("C6529B1 Registers");
	// $FD40-$FDCF (0x90): It is not mapped!...
	MCHEmul::EmptyPhysicalStorageSubset* IONOMAPPED3 = new MCHEmul::EmptyPhysicalStorageSubset 
		(_IONOMAPPED3_SUBSET, MCHEmul::UByte::_0, RAM, 0xfd40, MCHEmul::Address ({ 0x40, 0xfd }, false), 0x90);	// 144 bytes
	IONOMAPPED3 -> setName ("IO Not Mapped 3");
	// $FDD0-$FDDF (0x10): This zone of the memory is just to switch between different c1 c2 lines external...
	C264::C1C2SelectorRegisters* IOC1C2Selector = new C264::C1C2SelectorRegisters 
		(RAM, 0xfdd0, MCHEmul::Address ({ 0xd0, 0xfd }, false), 0x10);											// 16 bytes
	IOC1C2Selector -> setName ("IO C1 C2 Selector");
	// $FDE0-$FEBF (0xe0): It is not mapped!...
	MCHEmul::EmptyPhysicalStorageSubset* IONOMAPPED5 = new MCHEmul::EmptyPhysicalStorageSubset 
		(_IONOMAPPED5_SUBSET, MCHEmul::UByte::_0, RAM, 0xfde0, MCHEmul::Address ({ 0xe0, 0xfd }, false), 0xe0);	// 224 bytes
	IONOMAPPED5 -> setName ("IO Not Mapped 5");
	// $FEC0-$FEDF (0x20): It is where the TIA manages the CBB 1551 as device #9
	COMMODORE::TIARegisters* IOTIA9 = new COMMODORE::TIARegisters
		(RAM, 0xfec0, MCHEmul::Address ({ 0xc0, 0xfe }, false), 0x20);											// 32 bytes
	IOTIA9 -> setName ("TIA Registers");
	// $FEE0-$FEFF (0x20): It is where the TIA manages the CBB 1551 as device #8
	MCHEmul::MirrorPhysicalStorageSubset* IOTIA8 = 
		new MCHEmul::MirrorPhysicalStorageSubset 
			(_IOTIA8_SUBSET, IOTIA9, MCHEmul::Address ({ 0xe0, 0xfe }, false));									// 32 bytes
	IOTIA8 -> setName ("TIA Registers Mirror");
	// $FF00-$FF3D (0x3E): The TED within the C264...it can access to more bytes of memory...
	C264::TEDRegisters* IOTED = new C264::TEDRegisters 
		(RAM, 0xff00, MCHEmul::Address ({ 0x00, 0xff }, false), 0x3e);											// TED in C264 = 62 bytes
	IOTED -> setName ("TED Registers");
	// $FF3E-$$FF3F (0x02): The switch RAM/ROM registers...
	C264::ROMRAMSwitchRegisters* ROMRAMSWITCH = new C264::ROMRAMSwitchRegisters
		(RAM, 0xff3e, MCHEmul::Address ({ 0x3e, 0xff }, false), 0x02);											// 2 bytes
	ROMRAMSWITCH -> setName ("ROM/RAM Switch Registers");

	// The rest is ROM...
	MCHEmul::PhysicalStorageSubset* KernelROM2 = new MCHEmul::PhysicalStorageSubset 
		(_KERNELROM2_SUBSET, KERNELROM, 0x3f40, MCHEmul::Address ({ 0x40, 0xff }, false), 0xc0);				// 192 bytes...
	KernelROM2 -> setName ("KERNEL ROM 2");
	// ...but it also can be RAM...
	MCHEmul::PhysicalStorageSubset* RAM5 = new MCHEmul::PhysicalStorageSubset 
		(_RAM5_SUBSET, RAM, 0xff40, MCHEmul::Address ({ 0x40, 0xff }, false), 0xc0);
	RAM5 -> setName ("KERNEL RAM 5");
	// ...or again with nothing connected behind...
	MCHEmul::EmptyPhysicalStorageSubset* EMPTYRAM5 = new MCHEmul::EmptyPhysicalStorageSubset 
		(_EMPTYRAM5_SUBSET, MCHEmul::UByte::_0, RAM, 0xff40, MCHEmul::Address ({ 0x40, 0xff }, false), 0xc0);
	EMPTYRAM5 -> setName ("Empty KERNEL RAM 5");
	// This zone of the memory can be occupied by a cartridge, but empty by default...
	MCHEmul::PhysicalStorageSubset* HIGHCARTRIDGE2 = new MCHEmul::EmptyPhysicalStorageSubset 
		(_HIGHCARTRIDGE2_SUBSET, MCHEmul::UByte::_0, HIGHCARTRIDGEROM, 0x3f40, MCHEmul::Address ({ 0x40, 0xff }, false), 0xc0);
	HIGHCARTRIDGE2 -> setName ("High Cartridge 2");

	// A map with all the subsets possible...
	MCHEmul::PhysicalStorageSubsets allsubsets (
		{
			{ _IO7501PORT_SUBSET,										IO7501Port }, 
			{ _PAGEZERO_SUBSET,											PageZero }, 
			{ _STACK_SUBSET,											Stack },
			{ _RAM1_SUBSET,												RAM1 },
			{ _RAM2_SUBSET,												RAM2 },
			{ _EMPTYRAM2_SUBSET,										EMPTYRAM2 },
			{ _BASICROM_SUBSET,											BasicROM },
			{ _RAM3_SUBSET,												RAM3 },
			{ _EMPTYRAM3_SUBSET,										EMPTYRAM3 },
			{ _LOWCARTRIDGE_SUBSET,										LOWCARTRIDGE },
			{ _KERNELROM1_SUBSET,										KernelROM1 },
			{ _RAM4_SUBSET,												RAM4 },
			{ _EMPTYRAM4_SUBSET,										EMPTYRAM4 },
			{ _KERNELROM2_SUBSET,										KernelROM2 },
			{ _RAM4_SUBSET,												RAM4 },
			{ _HIGHCARTRIDGE1_SUBSET,									HIGHCARTRIDGE1 },
			{ _IONOMAPPED2_SUBSET,										IONOMAPPED2 },
			{ C264::C6529B1Registers::_C6529B1REGS_SUBSET,				IO6529B1 },
			{ _IONOMAPPED3_SUBSET,										IONOMAPPED3 },
			{ C264::C1C2SelectorRegisters::_C1C2SELECTORREGS_SUBSET,	IOC1C2Selector },
			{ _IONOMAPPED5_SUBSET,										IONOMAPPED5 },
			{ COMMODORE::TIARegisters::_TIAREGS_SUBSET,					IOTIA9 },
			{ _IOTIA8_SUBSET,											IOTIA8 },
			{ C264::TEDRegisters::_TEDREGS_SUBSET,						IOTED },
			{ C264::ROMRAMSwitchRegisters::_ROMRAMSWITCHREGS_SUBSET,	ROMRAMSWITCH },
			{ _KERNELROM2_SUBSET,										KernelROM2 },
			{ _RAM5_SUBSET,												RAM5 },
			{ _EMPTYRAM5_SUBSET,										EMPTYRAM5 },
			{ _HIGHCARTRIDGE2_SUBSET,									HIGHCARTRIDGE2 }
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
	: C264::Memory (C264::C16_116Memory::standardMemoryContent (), C264::Type::_C16, cfg, lang),
	  _IOnomapped0 (nullptr),
	  _IOnomapped1 (nullptr)
{
	_IOnomapped0		= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> 
							(subset (_IONOMAPPED0_SUBSET));
	_IOnomapped1		= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> 
							(subset (_IONOMAPPED1_SUBSET));

	// None can be nullptr...
	assert (
		_IOnomapped0 != nullptr && 
		_IOnomapped1 != nullptr);

	setConfiguration (configuration (), ROMactive (), 
		C1Low (), C1High (), C2Low (), C2High ());
}
	
// ---
void C264::C16_116Memory::setConfiguration (unsigned int cfg, bool a, 
	bool c1l, bool c1h, bool c2l, bool c2h)
{
	if (cfg != 0 && cfg != 1)
	{
		_LOG ("Configuration mode:" + std::to_string (cfg) + 
			" not valid in C16/C116 memory");

		return;
	}

	// New values for configuration...
	_configuration = cfg;
	// ...and ROM/RAM status...
	_ROMActive = a;
	// ...and C1 & C2 lines status...
	_C1Low = c1l; _C1High = c1h;
	_C2Low = c2l; _C2High = c2h;

	// The very basic zones of the RAM...
	_RAM1					-> setActive (true); // The very basic RAM...

	// IO zone is always active...
	_IOnomapped0			-> setActive (true);
	_IOnomapped1			-> setActive (true);
	_IOnomapped2			-> setActive (true);
	_IO6529B1				-> setActive (true);
	_IOnomapped3			-> setActive (true);
	_IOC1C2Selector			-> setActive (true);
	_IOnomapped5			-> setActive (true);
	_IOTIA9					-> setActive (true);
	_IOTIA8					-> setActive (true);
	_IOTED					-> setActive (true);
	_ROMRAMSwitch			-> setActive (true); 

	// ...when none of this flags is on, the BASIC ROM is connected...
	bool rA = !_C1Low && !_C1High && !_C2Low && !_C2High;
	// ...and just to keep track of the opposite situation...
	bool cA = !rA;

	// The ROM...
	// Inclusing the the basic and the kernel with the possibilities of having external connections...
	// Either BASIC or RAM or Nothing...
	_basicROM			-> setActive (a);				// Active with ROM connected...
	_lowCartridge		-> setActive (false);			// Disconnected always by default...
	// Either KERNEL or RAM or Nothing...
	_kernelROM1			-> setActive (a);				// Active when ROM is on...
	_highCartridge1		-> setActive (false);			// Disconnected always by default...
	// Either KERNEL or RAM...
	_kernelROM2			-> setActive (a);				// Active with ROM is on...
	_highCartridge2		-> setActive (false);			// Disconnected always by default...

	// The RAM will depend on the configuration...
	// 16k RAM
	if (_configuration == 0)
	{
		// No more RAM...
		_RAM2				-> setActive (false);		// No RAM2...
		_EMPTYRAM2			-> setActive (true);		// No RAM2...
		_RAM3				-> setActive (false);		// No RAM3...
		_EMPTYRAM3			-> setActive (!a);			// EMPTYRAM3 active when no ROM is active...
		_RAM4				-> setActive (false);		// No RAM4...
		_EMPTYRAM4			-> setActive (!a);			// EMPTYRAM4 active when no ROM is active...
		_RAM5				-> setActive (false);		// No RAM5...
		_EMPTYRAM5			-> setActive (!a);			// EMPTYRAM5 active when no ROM is active...
	}
	// 64k RAM...
	else
	{
		// ..more RAM...
		_RAM2				-> setActive (true);		// Because it is extended to 64k...
		_EMPTYRAM2			-> setActive (false);		// No empty space...
		_RAM3				-> setActive (!a);			// RAM3 active when ROM is no active...
		_EMPTYRAM3			-> setActive (false);		// No empty space...
		_RAM4				-> setActive (!a);			// RAM4 active when ROM is no active...
		_EMPTYRAM4			-> setActive (false);		// No empty space...
		_RAM5				-> setActive (!a);			// RAM5 active when ROM is no active...
		_EMPTYRAM5			-> setActive (false);		// No empty space...
	}
}

// ---
MCHEmul::Memory::Content C264::C16_116Memory::standardMemoryContent ()
{
	MCHEmul::Memory::Content result = 
		std::move (C264::Memory::standardMemoryContent ());

	// Add the new element to the subsets...
	// There is ACIA so an no mapped place is installed instead...
	MCHEmul::PhysicalStorageSubset* nSS = nullptr;
	result._subsets.insert 
		(MCHEmul::PhysicalStorageSubsets::value_type 
			(_IONOMAPPED0_SUBSET, nSS = new MCHEmul::EmptyPhysicalStorageSubset 
				(_IONOMAPPED0_SUBSET, // No mapped instead...
					MCHEmul::UByte::_0, result.physicalStorage (_RAM), 0xfd00, MCHEmul::Address ({ 0x00, 0xfd }, false), 0x10)));
	nSS -> setName ("IO Not Mapped 0");
	// It doesn't exist a second C6529 register so a no mapped place is created also instead...
	result._subsets.insert 
		(MCHEmul::PhysicalStorageSubsets::value_type 
			(_IONOMAPPED1_SUBSET, nSS = new MCHEmul::EmptyPhysicalStorageSubset 
				(_IONOMAPPED1_SUBSET, // No mapperd instead...
					MCHEmul::UByte::_0, result.physicalStorage (_RAM), 0xfd10, MCHEmul::Address ({ 0x10, 0xfd }, false), 0x10)));
	nSS -> setName ("IO Not Mapped 1");

	// Then the views are recreated...
	result._views = MCHEmul::MemoryViews (
		{
			{ _CPU_VIEW, new MCHEmul::MemoryView (_CPU_VIEW, result._subsets) }
		});

	return (result);
}

// ---
C264::CPlus4Memory::CPlus4Memory (unsigned int cfg, const std::string& lang)
	: C264::Memory (C264::CPlus4Memory::standardMemoryContent (), C264::Type::_CPLUS4, cfg, lang),
	  _functionLow (nullptr), 
	  _functionHigh1 (nullptr),
	  _functionHigh2 (nullptr)
{
	// In the content...
	if (error () != MCHEmul::_NOERROR)
		return;

	_functionLow	= subset (_FUNCTIONLOW_SUBSET);
	_functionHigh1	= subset (_FUNCTIONHIGH1_SUBSET);
	_functionHigh2	= subset (_FUNCTIONHIGH2_SUBSET);
	_IOACIA			= dynamic_cast <COMMODORE::ACIARegisters*> 
		(subset (COMMODORE::ACIARegisters::_ACIAREGS_SUBSET));
	_IO6529B2		= dynamic_cast <C264::C6529B2Registers*> 
		(subset (C264::C6529B2Registers::_C6529B2REGS_SUBSET));

	// None can be nullptr...
	assert (
		_functionLow != nullptr && 
		_functionHigh1 != nullptr &&
		_functionHigh2 != nullptr &&
		_IOACIA != nullptr &&
		_IO6529B2 != nullptr);

	// The roms with the standard code for this machine!
	std::string FUNCTIONLOWFILE = "./CP4/3-plus-1.317053-01.bin";
	std::string FUNCTIONHIGHFILE = "./CP4/3-plus-1.317054-01.bin";

	// In this case there different options per languaje...

	bool ok = true;
	ok &= physicalStorage (_FUNCTIONLOW) -> loadInto (FUNCTIONLOWFILE);
	subset (_FUNCTIONLOW_SUBSET) -> fixDefaultValues (); // Fix the values for further initializations...
	ok &= physicalStorage (_FUNCTIONHIGH) -> loadInto (FUNCTIONHIGHFILE);
	subset (_FUNCTIONHIGH1_SUBSET) -> fixDefaultValues ();
	subset (_FUNCTIONHIGH2_SUBSET) -> fixDefaultValues ();

	if (!ok)
		_error = MCHEmul::_INIT_ERROR;

	setConfiguration (configuration (), ROMactive (), 
		C1Low (), C1High (), C2Low (), C2High ());
}
	
// ---
void C264::CPlus4Memory::setConfiguration (unsigned int cfg, bool a, 
	bool c1l, bool c1h, bool c2l, bool c2h)
{
	if (cfg != 0)
	{
		_LOG ("Configuration mode:" + std::to_string (cfg) + 
			" not valid in CPlus4 memory");

		return;
	}

	// New values for configuration...
	_configuration = cfg;
	// ...and ROM/RAM status...
	_ROMActive = a;
	// ...and also the configuration of the C1 and C2 lines...
	_C1Low = c1l; _C1High = c1h;
	_C2Low = c2l; _C2High = c2h;

	// The very basic zones of the RAM...
	_RAM1			-> setActive (true);
	_RAM2			-> setActive (true); // Because it is extended to 64k...

	// The IO zone is always active...
	_IOACIA			-> setActive (true);
	_IO6529B2		-> setActive (true);
	_IOnomapped2	-> setActive (true);
	_IO6529B1		-> setActive (true);
	_IOnomapped3	-> setActive (true);
	_IOC1C2Selector	-> setActive (true);
	_IOnomapped5	-> setActive (true);
	_IOTIA9			-> setActive (true);
	_IOTIA8			-> setActive (true);
	_IOTED			-> setActive (true);
	_ROMRAMSwitch	-> setActive (true);

	bool fA = (_C1Low && _C2Low);
	bool rA = (!_C1Low && !_C1High && !_C2Low && !_C2High);
	bool cA = !fA && !rA; // Any other combination...

	// The ROM...
	// In this case, the cartridges are connected just when c1low = true and c2low = true...
	// ...so when the valoe stored in the FDD0 register is 0x05...
	// The zone of the RAM...
	_functionLow	-> setActive (a && fA);		// Active with both lines connected...
	_basicROM		-> setActive (a && rA);		// Active with no line connected...
	_lowCartridge	-> setActive (a && cA);		// Active with any other combination...
	// The zone 1 of the KERNEL...
	_functionHigh1	-> setActive (a && fA);		// Active with both lines connected...
	_kernelROM1		-> setActive (a && rA);		// Active with no line connected...
	_highCartridge1	-> setActive (a && cA);		// Active with any other combination...
	// The zone 2 of the KERNEL...
	_functionHigh2	-> setActive (a && fA);		// Active with both lines connected...
	_kernelROM2		-> setActive (a && rA);		// Active with no line connected...
	_highCartridge1	-> setActive (a && cA);		// Active with any other combination...

	// Then the rest of the RAM...
	// This type of computer has always 64k...
	_EMPTYRAM2		-> setActive (false); // The RAM exists instead!
	_RAM3			-> setActive (!a);
	_EMPTYRAM3		-> setActive (false); // No empty space...
	_RAM4			-> setActive (!a);
	_EMPTYRAM4		-> setActive (false); // No empty space...
	_RAM5			-> setActive (!a);
	_EMPTYRAM5		-> setActive (false); // No empty space...
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
	// The function Low...
	MCHEmul::PhysicalStorageSubset* nSS = nullptr;
	result._subsets.insert 
		(MCHEmul::PhysicalStorageSubsets::value_type
			(_FUNCTIONLOW_SUBSET, nSS = new MCHEmul::PhysicalStorageSubset 
				(_FUNCTIONLOW_SUBSET, FUNCLOW, 0x0000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000)));
	nSS -> setName ("Function Low ROM");
	// The function High 1...
	result._subsets.insert 
		(MCHEmul::PhysicalStorageSubsets::value_type
			(_FUNCTIONHIGH1_SUBSET, nSS = new MCHEmul::PhysicalStorageSubset 
				(_FUNCTIONHIGH1_SUBSET, FUNCHIGH, 0x0000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x3d00)));
	nSS -> setName ("Function High 1 ROM");
	// The function High 2...
	result._subsets.insert 
		(MCHEmul::PhysicalStorageSubsets::value_type
			(_FUNCTIONHIGH2_SUBSET, nSS = new MCHEmul::PhysicalStorageSubset 
				(_FUNCTIONHIGH2_SUBSET, FUNCHIGH, 0x0000, MCHEmul::Address ({ 0x40, 0xff }, false), 0xc0)));

	// The ACIA chip (used in communications)
	result._subsets.insert 
		(MCHEmul::PhysicalStorageSubsets::value_type 
			(COMMODORE::ACIARegisters::_ACIAREGS_SUBSET, 
				nSS = new COMMODORE::ACIARegisters (result.physicalStorage (_RAM), // The ACIA...
					0xfd00, MCHEmul::Address ({ 0x00, 0xfd }, false), 0x10)));
	nSS -> setName ("ACIA Registers");
	// The C6529 (2) chip, which use is still unknown...
	result._subsets.insert
		(MCHEmul::PhysicalStorageSubsets::value_type 
			(C264::C6529B2Registers::_C6529B2REGS_SUBSET,
				nSS = new C264::C6529B2Registers (result.physicalStorage (_RAM), // And another latch chip...
					0xfd10, MCHEmul::Address ({ 0x10, 0xfd }, false), 0x10)));
	nSS -> setName ("C6529B2 Registers");

	// Finally, recreates the view...
	result._views = MCHEmul::MemoryViews (
		{
			{ _CPU_VIEW, new MCHEmul::MemoryView (_CPU_VIEW, result._subsets) }
		});

	return (result);
}
