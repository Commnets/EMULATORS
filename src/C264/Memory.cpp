#include <C264/Memory.hpp>

// ---
C264::Memory::Memory (const MCHEmul::Memory::Content& cnt, 
		C264::Type t, unsigned int cfg, const std::string& lang)
	: MCHEmul::Memory (0, cnt, { }),
	  _type (t),
	  _configuration (cfg),
	  _memoryConfiguration (0), // The one by default...
	  _ROMActive (true), // By default the ROM is active...
	  _cartridge1Loaded (false), _cartridge1Connected (false),
	  _cartridge2Loaded (false), _cartridge2Connected (false),
	  _RAM1 (nullptr),
	  _RAM2 (nullptr),
	  _RAM2MirrorIO7501Port (nullptr),
	  _RAM2MirrorPageZero (nullptr),
	  _RAM2MirrorStack (nullptr),
	  _RAM2MirrorRAM1 (nullptr),
	  _basicROM (nullptr),
	  _3plus1ROM1 (nullptr),
	  _cartridge1Low (nullptr),
	  _cartridge2Low (nullptr),
	  _noExtension (nullptr),
	  _RAM3 (nullptr),
	  _RAM3MirrorIO7501Port (nullptr),
	  _RAM3MirrorPageZero (nullptr),
	  _RAM3MirrorStack (nullptr),
	  _RAM3MirrorRAM1 (nullptr),
	  _kernelROM1 (nullptr),
	  _3plus1ROM21 (nullptr),
	  _cartridge1High1 (nullptr),
	  _cartridge2High1 (nullptr),
	  _RAM41 (nullptr),
	  _RAM41MirrorIO7501Port (nullptr),
	  _RAM41MirrorPageZero (nullptr),
	  _RAM41MirrorStack (nullptr),
	  _RAM41MirrorRAM1 (nullptr),
	  _RAM41MirrorRAM2 (nullptr),
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
	  _3plus1ROM22 (nullptr),
	  _cartridge1High2 (nullptr),
	  _cartridge2High2 (nullptr),
	  _RAM42 (nullptr),
	  _RAM42MirrorRAM1 (nullptr),
	  _RAM42MirrorRAM2 (nullptr)
{
	// In the content...
	if (error () != MCHEmul::_NOERROR)
		return;

	// Get references to all things tha can change in configuration...
	// B1
	_RAM1						= subset (_RAM1_SUBSET);
	// B2
	_RAM2						= subset (_RAM2_SUBSET);
	_RAM2MirrorIO7501Port		= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> 
									(subset (_RAM2MIRRORIO7501PORT_SUBSET));
	_RAM2MirrorPageZero			= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> 
									(subset (_RAM2MIRRORPAGEZERO_SUBSET));
	_RAM2MirrorStack			= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> 
									(subset (_RAM2MIRRORSTACK_SUBSET));
	_RAM2MirrorRAM1				= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> 
									(subset (_RAM2MIRRORRAM1_SUBSET));
	// B3
	_basicROM					= subset (_BASICROM_SUBSET);
	_3plus1ROM1					= subset (_3PLUS1ROM1_SUBSET);
	_cartridge1Low				= subset (_CARTRIDGE1LOW_SUBSET);
	_cartridge2Low				= subset (_CARTRIDGE2LOW_SUBSET);
	_noExtension				= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> 
									(subset (_NOEXTENSION_SUBSET));
	_RAM3						= subset (_RAM3_SUBSET);
	_RAM3MirrorIO7501Port		= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> 
									(subset (_RAM3MIRRORIO7501PORT_SUBSET));
	_RAM3MirrorPageZero			= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> 
									(subset (_RAM3MIRRORPAGEZERO_SUBSET));
	_RAM3MirrorStack			= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> 
									(subset (_RAM3MIRRORSTACK_SUBSET));
	_RAM3MirrorRAM1				= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> 
									(subset (_RAM3MIRRORRAM1_SUBSET));
	// B4 (Part 1)
	_kernelROM1					= subset (_KERNELROM1_SUBSET);
	_3plus1ROM21				= subset (_3PLUS1ROM21_SUBSET);
	_cartridge1High1			= subset (_CARTRIDGE1HIGH1_SUBSET);
	_cartridge2High1			= subset (_CARTRIDGE2HIGH1_SUBSET);
	_RAM41						= subset (_RAM41_SUBSET);
	_RAM41MirrorIO7501Port		= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> 
									(subset (_RAM41MIRRORIO7501PORT_SUBSET));
	_RAM41MirrorPageZero		= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> 
									(subset (_RAM41MIRRORPAGEZERO_SUBSET));
	_RAM41MirrorStack			= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> 
									(subset (_RAM41MIRRORSTACK_SUBSET));
	_RAM41MirrorRAM1			= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> 
									(subset (_RAM41MIRRORRAM1_SUBSET));
	_RAM41MirrorRAM2			= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*>
									(subset (_RAM41MIRRORRAM2_SUBSET));

	// B4 (Part IO)
	_IOnomapped2				= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> 
									(subset (_IONOMAPPED2_SUBSET));
	_IO6529B1					= dynamic_cast <C264::C6529B1Registers*> 
									(subset (C264::C6529B1Registers::_C6529B1REGS_SUBSET));
	_IOnomapped3				= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> 
									(subset (_IONOMAPPED3_SUBSET));
	_IOC1C2Selector				= dynamic_cast <C264::C1C2SelectorRegisters*>
									(subset (C264::C1C2SelectorRegisters::_C1C2SELECTORREGS_SUBSET));
	_IOnomapped5				= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> 
									(subset (_IONOMAPPED5_SUBSET));
	_IOTIA9						= dynamic_cast <COMMODORE::TIARegisters*> 
									(subset (COMMODORE::TIARegisters::_TIAREGS_SUBSET));
	_IOTIA8						= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> 
									(subset (_IOTIA8_SUBSET));
	_IOTED						= dynamic_cast <C264::TEDRegisters*> 
									(subset (C264::TEDRegisters::_TEDREGS_SUBSET));
	_ROMRAMSwitch				= dynamic_cast <C264::ROMRAMSwitchRegisters*> 
									(subset (C264::ROMRAMSwitchRegisters::_ROMRAMSWITCHREGS_SUBSET)); 
	// B4 (Part 2)
	_kernelROM2					= subset (_KERNELROM2_SUBSET);
	_3plus1ROM22				= subset (_3PLUS1ROM22_SUBSET);
	_cartridge1High2			= subset (_CARTRIDGE1HIGH2_SUBSET);
	_cartridge2High2			= subset (_CARTRIDGE2HIGH2_SUBSET);
	_RAM42						= subset (_RAM42_SUBSET);
	_RAM42MirrorRAM1			= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*> 
									(subset (_RAM42MIRRORRAM1_SUBSET));
	_RAM42MirrorRAM2			= dynamic_cast <MCHEmul::MirrorPhysicalStorageSubset*>
									(subset (_RAM42MIRRORRAM2_SUBSET));

	// None can be nullptr...
	assert (
	  _RAM1						!= nullptr &&
	  _RAM2						!= nullptr &&
	  _RAM2MirrorIO7501Port		!= nullptr &&
	  _RAM2MirrorPageZero		!= nullptr &&
	  _RAM2MirrorStack			!= nullptr &&
	  _RAM2MirrorRAM1			!= nullptr &&
	  _basicROM					!= nullptr &&
	  _3plus1ROM1				!= nullptr &&
	  _cartridge1Low			!= nullptr &&
	  _cartridge2Low			!= nullptr &&
	  _noExtension				!= nullptr &&
	  _RAM3						!= nullptr &&
	  _RAM3MirrorIO7501Port		!= nullptr &&
	  _RAM3MirrorPageZero		!= nullptr &&
	  _RAM3MirrorStack			!= nullptr &&
	  _RAM3MirrorRAM1			!= nullptr &&
	  _kernelROM1				!= nullptr &&
	  _3plus1ROM21				!= nullptr &&
	  _cartridge1High1			!= nullptr &&
	  _cartridge2High1			!= nullptr &&
	  _RAM41					!= nullptr &&
	  _RAM41MirrorIO7501Port	!= nullptr &&
	  _RAM41MirrorPageZero		!= nullptr &&
	  _RAM41MirrorStack			!= nullptr &&
	  _RAM41MirrorRAM1			!= nullptr &&
	  _RAM41MirrorRAM2			!= nullptr &&
	  _IOnomapped2				!= nullptr &&
	  _IO6529B1					!= nullptr &&
	  _IOnomapped3				!= nullptr &&
	  _IOC1C2Selector			!= nullptr &&
	  _IOnomapped5				!= nullptr &&
	  _IOTIA9					!= nullptr &&
	  _IOTIA8					!= nullptr &&
	  _IOTED					!= nullptr &&
	  _ROMRAMSwitch				!= nullptr &&
	  _kernelROM2				!= nullptr &&
	  _3plus1ROM22				!= nullptr &&
	  _cartridge1High2			!= nullptr &&
	  _cartridge2High2			!= nullptr &&
	  _RAM42					!= nullptr &&
	  _RAM42MirrorRAM1			!= nullptr &&
	  _RAM42MirrorRAM2			!= nullptr);

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
	// The cartridges and the 3+1 roms are empty by default...

	if (!ok)
		_error = MCHEmul::_INIT_ERROR;

	// The configuration is not set, 
	// as it is done in the classes inheriting this one...
	// It it were done here, the virtual method won't be invoked with a behaviour not defined...
}

// ---
void C264::Memory::setConfiguration (unsigned int cfg, bool a, unsigned char mcfg)
{
	// There can not be more than 3 configurations possibles..., 
	// ...but the basic ones will depend on the type of machine...
	assert (cfg >= 0 && cfg <= 2);

	// New values for configuration...
	_configuration = cfg;
	// The memory configuration...
	_memoryConfiguration = mcfg;
	// ...and ROM/RAM status...
	_ROMActive = a;

	// B1
	_RAM1					-> setActive (true);	 // 16, 32 or 64K. Always active...
	_RAM1					-> setActiveForReading (true); // ....and active also for reading...It is the basic standard memory...
	// B2
	_RAM2					-> setActive (cfg != 0); // ...active when the computer is 32k...
	_RAM2					-> setActiveForReading (cfg != 0); // ...and also active for reading...
	_RAM2MirrorIO7501Port	-> setActive (cfg == 0); // ...only when the computer is 16k...
	_RAM2MirrorIO7501Port	-> setActiveForReading (cfg == 0); // ...and also for reading... (the rest of the parts of the RAM1 mirror too)
	_RAM2MirrorPageZero		-> setActive (cfg == 0);
	_RAM2MirrorPageZero		-> setActiveForReading (cfg == 0);
	_RAM2MirrorStack		-> setActive (cfg == 0);
	_RAM2MirrorStack		-> setActiveForReading (cfg == 0);
	_RAM2MirrorRAM1			-> setActive (cfg == 0);
	_RAM2MirrorRAM1			-> setActiveForReading (cfg == 0);
	// B3
	_basicROM				-> setActive (a && isMemoryConfigurationLowROMBasic ()); // Active depending on the configuration...
	_basicROM				-> setActiveForReading (a && isMemoryConfigurationLowROMBasic ());	
	_3plus1ROM1				-> setActive (false);	// Always off...
	_3plus1ROM1				-> setActiveForReading (false);
	_cartridge1Low			-> setActive (a && isMemoryConfigurationLowROMCartridge1 ()); // Active depending on the configuration...
	_cartridge1Low			-> setActiveForReading (a && isMemoryConfigurationLowROMCartridge1 ());
	_cartridge2Low			-> setActive (a && isMemoryConfigurationLowROMCartridge2 ()); // Active depending on the configuration...
	_cartridge2Low			-> setActiveForReading (a && isMemoryConfigurationLowROMCartridge2 ());
	_noExtension			-> setActive (a && 
								(!isMemoryConfigurationLowROMBasic () && 
								 !isMemoryConfigurationLowROMCartridge1 () && 
								 !isMemoryConfigurationLowROMCartridge2 ())); // Just when nothing else is connected...
	_noExtension			-> setActiveForReading (a &&
								(!isMemoryConfigurationLowROMBasic () && 
								 !isMemoryConfigurationLowROMCartridge1 () &&
								 !isMemoryConfigurationLowROMCartridge2 ()));
	_RAM3					-> setActive (cfg == 2); // ..active only when it is 64k...
	_RAM3					-> setActiveForReading (!a && cfg == 2); // ...but active for reading just when RAM actives...
	_RAM3MirrorIO7501Port	-> setActive (cfg != 2); // ...active when the configuration is not 64k...
	_RAM3MirrorIO7501Port	-> setActiveForReading (!a && cfg != 2); // ...and active for reading just when RAM actives...
	_RAM3MirrorPageZero		-> setActive (cfg != 2);
	_RAM3MirrorPageZero		-> setActiveForReading (!a && cfg != 2);
	_RAM3MirrorStack		-> setActive (cfg != 2);
	_RAM3MirrorStack		-> setActiveForReading (!a && cfg != 2);
	_RAM3MirrorRAM1			-> setActive (cfg != 2); 
	_RAM3MirrorRAM1			-> setActiveForReading (!a && cfg != 2); 

	// B4 (Part 1)
	_kernelROM1				-> setActive (a && 
								(!cartridgeConnected () || 
								 (cartridgeConnected () && 
									(!isMemoryConfigurationHighROMCartridge1 () && 
									 !isMemoryConfigurationHighROMCartridge2 ())))); // Just when no cartridge is connected...
	_3plus1ROM21			-> setActive (false);	// Always off, not possible in C16/C116
	_3plus1ROM21			-> setActiveForReading (false);
	_cartridge1High1		-> setActive (a && 
								(_cartridge1Connected && 
								 isMemoryConfigurationHighROMCartridge1 ())); // When cartridge 1 is connected and ROM selected....
	_cartridge2High1		-> setActive (a && 
								(_cartridge2Connected && 
								 isMemoryConfigurationHighROMCartridge2 ())); // When cartridge 2 is connected and ROM selected....
	_RAM41					-> setActive (cfg == 2); // ...active only when it is 64k...
	_RAM41					-> setActiveForReading (!a && cfg == 2); // ...but active for reading just when RAM actives...
	_RAM41MirrorIO7501Port	-> setActive (cfg == 0); // ...active when it is 16k...
	_RAM41MirrorIO7501Port	-> setActiveForReading (!a && cfg == 0); // ...and active for reading just when RAM actives...
	_RAM41MirrorPageZero	-> setActive (cfg == 0); 
	_RAM41MirrorPageZero	-> setActiveForReading (!a && cfg == 0);
	_RAM41MirrorStack		-> setActive (cfg == 0);
	_RAM41MirrorStack		-> setActiveForReading (!a && cfg == 0);
	_RAM41MirrorRAM1		-> setActive (cfg == 0);
	_RAM41MirrorRAM1		-> setActiveForReading (!a && cfg == 0);
	_RAM41MirrorRAM2		-> setActive (cfg == 1); // ...active only when it is 32k...
	_RAM41MirrorRAM2		-> setActiveForReading (!a && cfg == 1); // ...but active for reading just when RAM actives...
	// B4 (IO Part)
	// The specific areas of every computer are in their own classes...
	// Remember that there are tow pieces missed in the generic part...
	_IOnomapped2			-> setActive (true);	// Always on...
	_IOnomapped2			-> setActiveForReading (true);
	_IO6529B1				-> setActive (true);	// Always on...
	_IO6529B1				-> setActiveForReading (true);
	_IOnomapped3			-> setActive (true);	// Always on...
	_IOnomapped3			-> setActiveForReading (true);
	_IOC1C2Selector			-> setActive (true);	// Always on...
	_IOC1C2Selector			-> setActiveForReading (true);
	_IOnomapped5			-> setActive (true);	// Always on...
	_IOnomapped5			-> setActiveForReading (true);
	_IOTIA9					-> setActive (true);	// Always on...
	_IOTIA9					-> setActiveForReading (true);
	_IOTIA8					-> setActive (true);	// Always on...
	_IOTIA8					-> setActiveForReading (true);
	_IOTED					-> setActive (true);	// Always on...
	_IOTED					-> setActiveForReading (true);
	_ROMRAMSwitch			-> setActive (true);	// Always on...
	_ROMRAMSwitch			-> setActiveForReading (true);
	// B4 (Part 1)
	// See comments in the part 1....
	_kernelROM2				-> setActive (a && 
								(!cartridgeConnected () || 
								 (cartridgeConnected () && 
									(!isMemoryConfigurationHighROMCartridge1 () && 
									 !isMemoryConfigurationHighROMCartridge2 ()))));
	_kernelROM2				-> setActiveForReading (a && 
								(!cartridgeConnected () || 
								 (cartridgeConnected () && 
									(!isMemoryConfigurationHighROMCartridge1 () && 
										!isMemoryConfigurationHighROMCartridge2 ()))));
	_3plus1ROM22			-> setActive (false);	// Always off...
	_3plus1ROM22			-> setActiveForReading (false);
	_cartridge1High2		-> setActive (a && 
								(_cartridge1Connected && 
								 isMemoryConfigurationHighROMCartridge1 ()));
	_cartridge2High2		-> setActive (a && 
								(_cartridge2Connected && 
								 isMemoryConfigurationHighROMCartridge2 ()));
	_RAM42					-> setActive (cfg == 2); // ...active when computer is 64k...
	_RAM42					-> setActiveForReading (!a && cfg == 2); // ...but active for reading when RAMs active...
	_RAM42MirrorRAM1		-> setActive (cfg == 0); // ...active just when it is 16k...
	_RAM42MirrorRAM1		-> setActiveForReading (!a && cfg == 0); // ...but active for reading when RAMSs active...
	_RAM42MirrorRAM2		-> setActive (cfg == 1); // ...active when computer is 32k...
	_RAM42MirrorRAM2		-> setActiveForReading (!a && cfg == 1); // ...but active for reading when RAMs active...
}

// ---
void C264::Memory::loadCartridge1 (const std::string& f1, const std::string& f2)
{
	// TODO

	_cartridge1Loaded = true;
}

// ---
void C264::Memory::loadCartridge2 (const std::string& f1, const std::string& f2)
{
	// TODO

	_cartridge2Loaded = true;
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
	unsigned int oMC = memoryConfiguration ();
	setConfiguration (0 /** The very basic configuration of the computer */, 
		false /** ROM active */, 0 /** The very basic configuration of the memory. */);
	// Just to fillup all RAM with the initial value!
	for (size_t i = 0X0000; i < 0x10000; i += 0x40)
		if (i < 0xfd00 || i >= 0xff40) // The IO zone is not filled...
			fillWith (MCHEmul::Address ({ 0x00, 0x00 }) + i, 
				(((i / 0x40) % 2) == 0) ? MCHEmul::UByte::_0 : MCHEmul::UByte::_FF, 0x40);
	// Go back to the same configuration...
	setConfiguration (oC, oRA, oMC);
	// The cartridges are not connected...
	_cartridge1Connected = _cartridge2Connected = false;

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
	// usually RAM is all behind the system...
MCHEmul::PhysicalStorage* RAM = 
		new MCHEmul::PhysicalStorage (_RAM,
			MCHEmul::PhysicalStorage::Type::_RAM, 0x10000);														// 64k RAM

	// Standard ROM...
	// This is common in C16 and in C116...
	MCHEmul::PhysicalStorage* BASICROM = 
		new MCHEmul::PhysicalStorage (_BASICROM,
			MCHEmul::PhysicalStorage::Type::_ROM, 0x4000);														// 16k ROM
	MCHEmul::PhysicalStorage* KERNELROM	= 
		new MCHEmul::PhysicalStorage (_KERNELROM,
			MCHEmul::PhysicalStorage::Type::_ROM, 0x4000);														// 16k ROM
	// These two are not initialized because they will filled up with the ROM after creation...

	// The ROM instead the standard when 3+1 configuration is used...
	// This is only in C264...
	MCHEmul::PhysicalStorage* THREEPLUS1ROM1 = 
		new MCHEmul::PhysicalStorage (_3PLUS1ROM1,
			MCHEmul::PhysicalStorage::Type::_ROM, 0x4000);														// 16k ROM
	MCHEmul::PhysicalStorage* THREEPLUS1ROM2 =
		new MCHEmul::PhysicalStorage (_3PLUS1ROM2,
			MCHEmul::PhysicalStorage::Type::_ROM, 0x4000);														// 16k ROM

	// External Cartridges but empty that can be added later...
	// In some configurations the external cartridges are used...
	// Cartridge 1...
	MCHEmul::PhysicalStorage* CARTRIDGE1LOWROM = 
		new MCHEmul::PhysicalStorage (_CARTRIDGE1LOWROM, 
			MCHEmul::PhysicalStorage::Type::_ROM, 0x4000);														// 16k ROM
	MCHEmul::PhysicalStorage* CARTRIDGE1HIGHROM = 
		new MCHEmul::PhysicalStorage (_CARTRIDGE1HIGHROM, 
			MCHEmul::PhysicalStorage::Type::_ROM, 0x4000);														// 16k ROM
	// Cartridge 2...
	MCHEmul::PhysicalStorage* CARTRIDGE2LOWROM = 
		new MCHEmul::PhysicalStorage (_CARTRIDGE2LOWROM, 
			MCHEmul::PhysicalStorage::Type::_ROM, 0x4000);														// 16k ROM
	MCHEmul::PhysicalStorage* CARTRIDGE2HIGHROM = 
		new MCHEmul::PhysicalStorage (_CARTRIDGE2HIGHROM, 
			MCHEmul::PhysicalStorage::Type::_ROM, 0x4000);														// 16k ROM

	// All are initialized as the emulator VICE recognize them...
	for (size_t i = 0; i < 0x4000; i++)
	{
		THREEPLUS1ROM1		-> set (i, MCHEmul::UByte::_FF);
		THREEPLUS1ROM2		-> set (i, MCHEmul::UByte::_FF);
		CARTRIDGE1LOWROM	-> set (i, MCHEmul::UByte::_FF);
		CARTRIDGE1HIGHROM	-> set (i, MCHEmul::UByte::_FF);
		CARTRIDGE2LOWROM	-> set (i, MCHEmul::UByte::_FF);
		CARTRIDGE2HIGHROM	-> set (i, MCHEmul::UByte::_FF);
	}

	// The map of phisical storages, used later...
	MCHEmul::PhysicalStorages storages (
		{
			{ _RAM,					RAM },
			{ _BASICROM,			BASICROM },
			{ _3PLUS1ROM1,			THREEPLUS1ROM1 },
			{ _CARTRIDGE1LOWROM,	CARTRIDGE1LOWROM },
			{ _CARTRIDGE2LOWROM,	CARTRIDGE2LOWROM },
			{ _KERNELROM,			KERNELROM },
			{ _3PLUS1ROM2,			THREEPLUS1ROM2 },
			{ _CARTRIDGE1HIGHROM,	CARTRIDGE1HIGHROM },
			{ _CARTRIDGE2HIGHROM,	CARTRIDGE2HIGHROM }
		});

	// ----
	// B1
	// $0000-$0001: The two registers of the IO expansion port...
	// This subset exists always...
	MCHEmul::PhysicalStorageSubset* IO7501Port = 
		new F6500::IO7501PortRegisters (_IO7501PORT_SUBSET, RAM);												// 2 bytes
	IO7501Port -> setName ("IO7501 Port Registers");
	// $0002-$00FF: Page 0.The quickest possible access...
	// This subset exists always too...
	MCHEmul::PhysicalStorageSubset* pageZero = new MCHEmul::PhysicalStorageSubset 
		(_PAGEZERO_SUBSET, RAM, 0x0002, MCHEmul::Address ({ 0x02, 0x00 }, false), 0xfe);						// 254 bytes
	pageZero -> setName ("Page Zero");
	// $0100-$01FF: Page 1.Stack, Where the CPU stores info...
	// This subset exists always too...
	MCHEmul::Stack* stack = new MCHEmul::Stack 
		(_STACK_SUBSET, RAM, 0x0100, MCHEmul::Address ({ 0x00, 0x01 }, false), 0x0100,							// 256 bytes
			MCHEmul::Stack::Configuration (true, true /** Pointing to the first empty position. */, 
				false /** No overflow detection. */, -1));
	stack -> setName ("Stack");
	// After the stack a set of RAM is followed, divided in two blocks...
	// From $0200 - $3FFF: The first block of memory exists always, whatever the computer emulated is...
	MCHEmul::PhysicalStorageSubset* RAM1 = new MCHEmul::PhysicalStorageSubset 
		(_RAM1_SUBSET, RAM, 0x0200, MCHEmul::Address ({ 0x00, 0x02 }, false), 0x3e00);							// 16k - 512 bytes (2 previous pages)
	RAM1 -> setName ("RAM 1");
	// ----

	// ----
	// B2
	// From $4000-$7FFF:
	// The possibilites are either new RAM or a mirror of the first block of RAM...
	MCHEmul::PhysicalStorageSubset* RAM2 = new MCHEmul::PhysicalStorageSubset 
		(_RAM2_SUBSET, RAM, 0x4000, MCHEmul::Address ({ 0x00, 0x40 }, false), 0x4000);							// 16k
	RAM2 -> setName ("RAM 2");
	// ...or the mirror or the RAM 1 has to be defined in pieces also...
	MCHEmul::MirrorPhysicalStorageSubset* RAM2MirrorIO7501Port = new MCHEmul::MirrorPhysicalStorageSubset 
		(_RAM2MIRRORIO7501PORT_SUBSET, IO7501Port, MCHEmul::Address ({ 0x00, 0x40 }, false));
	RAM2MirrorIO7501Port -> setName ("RAM 2 Mirror of IO7501 Port Registers");
	MCHEmul::MirrorPhysicalStorageSubset* RAM2MirrorPageZero = new MCHEmul::MirrorPhysicalStorageSubset
		(_RAM2MIRRORPAGEZERO_SUBSET, pageZero, MCHEmul::Address ({ 0x02, 0x40 }, false));
	RAM2MirrorPageZero -> setName ("RAM 2 Mirror of Page Zero");
	MCHEmul::MirrorPhysicalStorageSubset* RAM2MirrorStack = new MCHEmul::MirrorPhysicalStorageSubset
		(_RAM2MIRRORSTACK_SUBSET, stack, MCHEmul::Address ({ 0x00, 0x41 }, false));
	RAM2MirrorStack -> setName ("RAM 2 Mirror of Stack");
	MCHEmul::MirrorPhysicalStorageSubset* RAM2MirrorRAM1 = new MCHEmul::MirrorPhysicalStorageSubset 
		(_RAM2MIRRORRAM1_SUBSET, RAM1, MCHEmul::Address ({ 0x00, 0x42 }, false));								
	RAM2MirrorRAM1 -> setName ("RAM 2 Mirror of RAM 1");
	// ----

	// ----
	// B3
	// From $8000-$BFFF:
	// There might be different possibilities:
	// BASIC ROM, 3+1 ROM (part 1), CARTRIDGE1LOW, CARTRIDGE2LOW or RAM (that can be a mirror of the one in B1)...
	MCHEmul::PhysicalStorageSubset* basicROM = new MCHEmul::PhysicalStorageSubset
		(_BASICROM_SUBSET, BASICROM, 0x0000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000);					// 16k ROM
	basicROM -> setName ("BASIC ROM");
	MCHEmul::PhysicalStorageSubset* threePlus1ROM1 = new MCHEmul::PhysicalStorageSubset 
		(_3PLUS1ROM1_SUBSET, THREEPLUS1ROM1, 0x0000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000);
	threePlus1ROM1 -> setName ("3+1 ROM 1");
	MCHEmul::PhysicalStorageSubset* cartridge1Low = new MCHEmul::PhysicalStorageSubset 
		(_CARTRIDGE1LOW_SUBSET, CARTRIDGE1LOWROM, 0x0000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000);
	cartridge1Low -> setName ("Cartridge 1 Low");
	MCHEmul::PhysicalStorageSubset* cartridge2Low = new MCHEmul::PhysicalStorageSubset 
		(_CARTRIDGE2LOW_SUBSET, CARTRIDGE2LOWROM, 0x0000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000);
	cartridge2Low -> setName ("Cartridge 1 Low");
	// Notice that there is no an equivalent of this element in the high part of the ROM
	// In the system, when there is nothing there connected, the KERNEL is valid!
	MCHEmul::EmptyPhysicalStorageSubset* noExtension = new MCHEmul::EmptyPhysicalStorageSubset 
		(_NOEXTENSION_SUBSET, MCHEmul::UByte::_FF, RAM, 0x8000, 
			MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000);
	noExtension -> setName ("Nothing connected in Low Memory");
	// ...or RAM 3...
	MCHEmul::PhysicalStorageSubset* RAM3 = new MCHEmul::PhysicalStorageSubset 
		(_RAM3_SUBSET, RAM, 0x8000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x4000);							// 16k ROM
	RAM3 -> setName ("RAM 3 (Under BASIC)");
	// ...or a mirror of the RAM 1 that has to be defined also in pieces...
	MCHEmul::MirrorPhysicalStorageSubset* RAM3MirrorIO7501Port = new MCHEmul::MirrorPhysicalStorageSubset 
		(_RAM3MIRRORIO7501PORT_SUBSET, IO7501Port, MCHEmul::Address ({ 0x00, 0x80 }, false));
	RAM3MirrorIO7501Port -> setName ("RAM 3 Mirror of IO7501 Port Registers");
	MCHEmul::MirrorPhysicalStorageSubset* RAM3MirrorPageZero = new MCHEmul::MirrorPhysicalStorageSubset
		(_RAM3MIRRORPAGEZERO_SUBSET, pageZero, MCHEmul::Address ({ 0x02, 0x80 }, false));
	RAM3MirrorPageZero -> setName ("RAM 3 Mirror of Page Zero");
	MCHEmul::MirrorPhysicalStorageSubset* RAM3MirrorStack = new MCHEmul::MirrorPhysicalStorageSubset
		(_RAM3MIRRORSTACK_SUBSET, stack, MCHEmul::Address ({ 0x00, 0x81 }, false));
	RAM3MirrorStack -> setName ("RAM 3 Mirror of Stack");
	MCHEmul::MirrorPhysicalStorageSubset* RAM3MirrorRAM1 = new MCHEmul::MirrorPhysicalStorageSubset 
		(_RAM3MIRRORRAM1_SUBSET, RAM1, MCHEmul::Address ({ 0x00, 0x82 }, false));
	RAM3MirrorRAM1 -> setName ("RAM 3 (Under BASIC) Mirror of RAM 1");

	// ----
	// B4 (Part 1)
	// From $C000-$FCFF: 
	// There might different possibilities:
	// KERNEL ROM, 3+1 ROM (part 2), CARTRIDGE1HIGH, CARTRIDGE2HIGH or RAM (that can be a mirror of the one in B1 or B2)...
	// In the middle the IO area...
	MCHEmul::PhysicalStorageSubset* kernelROM1 = new MCHEmul::PhysicalStorageSubset 
		(_KERNELROM1_SUBSET, KERNELROM, 0x0000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x3d00);				// 16k - 768 bytes
	kernelROM1 -> setName ("KERNEL ROM Part 1");
	MCHEmul::PhysicalStorageSubset* threePlus1ROM21 = new MCHEmul::PhysicalStorageSubset 
		(_3PLUS1ROM21_SUBSET, THREEPLUS1ROM2, 0x0000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x3d00);
	threePlus1ROM21 -> setName ("3+1 ROM 2 Part 1");
	MCHEmul::PhysicalStorageSubset* cartridge1High1 = new MCHEmul::PhysicalStorageSubset 
		(_CARTRIDGE1HIGH1_SUBSET, CARTRIDGE1HIGHROM, 0x0000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x3d00);
	cartridge1High1 -> setName ("High Cartridge 1 Part 1");
	MCHEmul::PhysicalStorageSubset* cartridge2High1 = new MCHEmul::PhysicalStorageSubset 
		(_CARTRIDGE2HIGH1_SUBSET, CARTRIDGE2HIGHROM, 0x0000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x3d00);
	cartridge2High1 -> setName ("High Cartridge 2 Part 1");
	MCHEmul::PhysicalStorageSubset* RAM41 = new MCHEmul::PhysicalStorageSubset 
		(_RAM41_SUBSET, RAM, 0xc000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x3d00);
	RAM41 -> setName ("RAM 4 Part 1 (Under KERNEL)");
	// ...or a mirror or the RAM 1 that has to be defined in pieces...
	MCHEmul::MirrorPhysicalStorageSubset* RAM41MirrorIO7501Port = new MCHEmul::MirrorPhysicalStorageSubset 
		(_RAM41MIRRORIO7501PORT_SUBSET, IO7501Port, MCHEmul::Address ({ 0x00, 0xc0 }, false));
	RAM41MirrorIO7501Port -> setName ("RAM 41 Mirror of IO7501 Port Registers");
	MCHEmul::MirrorPhysicalStorageSubset* RAM41MirrorPageZero = new MCHEmul::MirrorPhysicalStorageSubset
		(_RAM41MIRRORPAGEZERO_SUBSET, pageZero, MCHEmul::Address ({ 0x02, 0xc0 }, false));
	RAM41MirrorPageZero -> setName ("RAM 41 Mirror of Page Zero");
	MCHEmul::MirrorPhysicalStorageSubset* RAM41MirrorStack = new MCHEmul::MirrorPhysicalStorageSubset
		(_RAM41MIRRORSTACK_SUBSET, stack, MCHEmul::Address ({ 0x00, 0xc1 }, false));
	RAM41MirrorStack -> setName ("RAM 41 Mirror of Stack");
	MCHEmul::MirrorPhysicalStorageSubset* RAM41MirrorRAM1 = new MCHEmul::MirrorPhysicalStorageSubset 
		(_RAM41MIRRORRAM1_SUBSET, RAM1, MCHEmul::Address ({ 0x00, 0xc2 }, false), 
			0 /** From the beginning. */, (int) (0x3e00 - 0x0300) /** The full length except the IO part + the part 2. */);
	RAM41MirrorRAM1 -> setName ("RAM 4 Part 1 (Under KERNEL) Mirror of RAM 1");
	// or a mirror or the RAM 2...
	MCHEmul::MirrorPhysicalStorageSubset* RAM41MirrorRAM2 = new MCHEmul::MirrorPhysicalStorageSubset 
		(_RAM41MIRRORRAM2_SUBSET, RAM2, MCHEmul::Address ({ 0x00, 0xc0 }, false),
			0 /** From the beginning. */, (int) (0x4000 - 0x0300) /** The full length except the IO part + part 2. */);
	RAM41MirrorRAM2 -> setName ("RAM 4 Part 1 (Under KERNEL) Mirror of RAM 2");
	// ----

	// ----
	// B4 (IO Part)
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
	// ----

	// ----
	// B4 (Part 2)
	// From $FF40-$FFFF: 
	// There might different possibilities:
	// KERNEL ROM, 3+1 ROM (part 2), CARTRIDGE1HIGH, CARTRIDGE2HIGH or RAM (that can be a mirror of the one in B1 or B2)...
	MCHEmul::PhysicalStorageSubset* kernelROM2 = new MCHEmul::PhysicalStorageSubset 
		(_KERNELROM2_SUBSET, KERNELROM, 0x3f40, MCHEmul::Address ({ 0x40, 0xff }, false), 0xc0);				// 192 bytes...
	kernelROM2 -> setName ("KERNEL ROM Part 2");
	MCHEmul::PhysicalStorageSubset* threePlus1ROM22 = new MCHEmul::PhysicalStorageSubset 
		(_3PLUS1ROM22_SUBSET, THREEPLUS1ROM2, 0x3f40, MCHEmul::Address ({ 0x40, 0xff }, false), 0xc0);
	threePlus1ROM22 -> setName ("3+1 ROM 2 Part 2");
	MCHEmul::PhysicalStorageSubset* cartridge1High2 = new MCHEmul::PhysicalStorageSubset 
		(_CARTRIDGE1HIGH2_SUBSET, CARTRIDGE1HIGHROM, 0x3f40, MCHEmul::Address ({ 0x40, 0xff }, false), 0xc0);
	cartridge1High2 -> setName ("High Cartridge 1 Part 2");
	MCHEmul::PhysicalStorageSubset* cartridge2High2 = new MCHEmul::PhysicalStorageSubset 
		(_CARTRIDGE2HIGH2_SUBSET, CARTRIDGE2HIGHROM, 0x3f40, MCHEmul::Address ({ 0x40, 0xff }, false), 0xc0);
	cartridge2High2 -> setName ("High Cartridge 2 Part 2");
	MCHEmul::PhysicalStorageSubset* RAM42 = new MCHEmul::PhysicalStorageSubset 
		(_RAM42_SUBSET, RAM, 0xff40, MCHEmul::Address ({ 0x40, 0xff }, false), 0xc0);
	RAM42 -> setName ("RAM 4 Part 2 (Under KERNEL)");
	// ...or the rest of the mirror over RAM 1. This last piece is the last part over the last piece...
	// From position 15680! (Only last 192 positions)
	MCHEmul::MirrorPhysicalStorageSubset* RAM42MirrorRAM1 = new MCHEmul::MirrorPhysicalStorageSubset 
		(_RAM42MIRRORRAM1_SUBSET, RAM1, MCHEmul::Address ({ 0x40, 0xff }, false), 
			(0x3e00 - 0x00c0) /** From almost the end, except part 2. */, (int) 0xc0 /** the length of the part 2. */);
	RAM42MirrorRAM1 -> setName ("RAM 4 Part 2 (Under KERNEL) Mirror of RAM 1");
	// ...or the rest of the mirror over RAM 2...
	MCHEmul::MirrorPhysicalStorageSubset* RAM42MirrorRAM2 = new MCHEmul::MirrorPhysicalStorageSubset 
		(_RAM42MIRRORRAM2_SUBSET, RAM2, MCHEmul::Address ({ 0x40, 0xff }, false),
			(0x4000 - 0x00c0) /** From almost the end except part 2. */, (int) 0xc0 /** The length of the part 2. */);
	RAM42MirrorRAM2 -> setName ("RAM 4 Part 2 (Under KERNEL) Mirror of RAM 2");

	// A map with all the possible subsets...
	MCHEmul::PhysicalStorageSubsets allsubsets (
		{
			// B1
			{ _IO7501PORT_SUBSET,										IO7501Port }, 
			{ _PAGEZERO_SUBSET,											pageZero }, 
			{ _STACK_SUBSET,											stack },
			{ _RAM1_SUBSET,												RAM1 },
			// B2
			{ _RAM2_SUBSET,												RAM2 },
			{ _RAM2MIRRORIO7501PORT_SUBSET,								RAM2MirrorIO7501Port }, 
			{ _RAM2MIRRORPAGEZERO_SUBSET,								RAM2MirrorPageZero }, 
			{ _RAM2MIRRORSTACK_SUBSET,									RAM2MirrorStack },
			{ _RAM2MIRRORRAM1_SUBSET,									RAM2MirrorRAM1 },
			// B3
			{ _BASICROM_SUBSET,											basicROM },
			{ _3PLUS1ROM1_SUBSET,										threePlus1ROM1 },
			{ _CARTRIDGE1LOW_SUBSET,									cartridge1Low },
			{ _CARTRIDGE2LOW_SUBSET,									cartridge2Low },
			{ _NOEXTENSION_SUBSET,										noExtension },
			{ _RAM3_SUBSET,												RAM3 },
			{ _RAM3MIRRORIO7501PORT_SUBSET,								RAM3MirrorIO7501Port }, 
			{ _RAM3MIRRORPAGEZERO_SUBSET,								RAM3MirrorPageZero }, 
			{ _RAM3MIRRORSTACK_SUBSET,									RAM3MirrorStack },
			{ _RAM3MIRRORRAM1_SUBSET,									RAM3MirrorRAM1 },
			// B4 (Part 1)
			{ _KERNELROM1_SUBSET,										kernelROM1 },
			{ _3PLUS1ROM21_SUBSET,										threePlus1ROM21 },
			{ _CARTRIDGE1HIGH1_SUBSET,									cartridge1High1 },
			{ _CARTRIDGE2HIGH1_SUBSET,									cartridge2High1 },
			{ _RAM41_SUBSET,											RAM41 },
			{ _RAM41MIRRORIO7501PORT_SUBSET,							RAM41MirrorIO7501Port }, 
			{ _RAM41MIRRORPAGEZERO_SUBSET,								RAM41MirrorPageZero }, 
			{ _RAM41MIRRORSTACK_SUBSET,									RAM41MirrorStack },
			{ _RAM41MIRRORRAM1_SUBSET,									RAM41MirrorRAM1 },
			{ _RAM41MIRRORRAM2_SUBSET,									RAM41MirrorRAM2 },
			// B4 (IO Part)
			{ _IONOMAPPED2_SUBSET,										IONOMAPPED2 },
			{ C264::C6529B1Registers::_C6529B1REGS_SUBSET,				IO6529B1 },
			{ _IONOMAPPED3_SUBSET,										IONOMAPPED3 },
			{ C264::C1C2SelectorRegisters::_C1C2SELECTORREGS_SUBSET,	IOC1C2Selector },
			{ _IONOMAPPED5_SUBSET,										IONOMAPPED5 },
			{ COMMODORE::TIARegisters::_TIAREGS_SUBSET,					IOTIA9 },
			{ _IOTIA8_SUBSET,											IOTIA8 },
			{ C264::TEDRegisters::_TEDREGS_SUBSET,						IOTED },
			{ C264::ROMRAMSwitchRegisters::_ROMRAMSWITCHREGS_SUBSET,	ROMRAMSWITCH },
			// B4 (Part 2)
			{ _KERNELROM2_SUBSET,										kernelROM2 },
			{ _3PLUS1ROM22_SUBSET,										threePlus1ROM22 },
			{ _CARTRIDGE1HIGH2_SUBSET,									cartridge1High2 },
			{ _CARTRIDGE2HIGH2_SUBSET,									cartridge2High2 },
			{ _RAM42_SUBSET,											RAM42 },
			{ _RAM42MIRRORRAM1_SUBSET,									RAM42MirrorRAM1 },
			{ _RAM42MIRRORRAM2_SUBSET,									RAM42MirrorRAM2 }
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

	setConfiguration (configuration (), ROMactive (), memoryConfiguration ());
}
	
// ---
void C264::C16_116Memory::setConfiguration (unsigned int cfg, bool a, unsigned char mcfg) 
{
	if (cfg != 0 /** 16k. */ && 
		cfg != 1 /** 32k. */ && 
		cfg != 2 /** 64k. */) // very rare this last one...
	{
		_LOG ("Configuration mode:" + std::to_string (cfg) + 
			" not valid in C16/C116 memory");

		return;
	}

	C264::Memory::setConfiguration (cfg, a, mcfg);

	// These two areas are always active...
	// ...they are specific of the C16/116...
	_IOnomapped0			-> setActive (true);	// Always on...
	_IOnomapped0			-> setActiveForReading (true);
	_IOnomapped1			-> setActive (true);	// Always on...
	_IOnomapped1			-> setActiveForReading (true);
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
	: C264::Memory (C264::CPlus4Memory::standardMemoryContent (), C264::Type::_CPLUS4, cfg, lang)
{
	// In the content...
	if (error () != MCHEmul::_NOERROR)
		return;

	_IOACIA			= dynamic_cast <COMMODORE::ACIARegisters*> 
		(subset (COMMODORE::ACIARegisters::_ACIAREGS_SUBSET));
	_IO6529B2		= dynamic_cast <C264::C6529B2Registers*> 
		(subset (C264::C6529B2Registers::_C6529B2REGS_SUBSET));

	// None can be nullptr...
	assert (
		_IOACIA != nullptr &&
		_IO6529B2 != nullptr);

	// The roms with the standard code for this machine!
	std::string THREEPLUS11FILE = "./bios/CP4/3-plus-1.317053-01.bin";
	std::string THREEPLUS12FILE = "./bios/CP4/3-plus-1.317054-01.bin";

	// In this case there different options per languaje...

	bool ok = true;
	ok &= physicalStorage (_3PLUS1ROM1) -> loadInto (THREEPLUS11FILE);
	subset (_3PLUS1ROM1_SUBSET) -> fixDefaultValues (); // Fix the values for further initializations...
	ok &= physicalStorage (_3PLUS1ROM2) -> loadInto (THREEPLUS12FILE);
	subset (_3PLUS1ROM21_SUBSET) -> fixDefaultValues ();
	subset (_3PLUS1ROM22_SUBSET) -> fixDefaultValues ();

	if (!ok)
		_error = MCHEmul::_INIT_ERROR;

	setConfiguration (configuration (), ROMactive (), memoryConfiguration ());
}
	
// ---
void C264::CPlus4Memory::setConfiguration (unsigned int cfg, bool a, unsigned char mcfg) 
{
	if (cfg != 0)
	{
		_LOG ("Configuration mode:" + std::to_string (cfg) + 
			" not valid in CPlus4 memory");

		return;
	}

	C264::Memory::setConfiguration (cfg, a, mcfg);

	// These two areas are always active...
	// ...they are specific of the CPlus4...
	_IOACIA			-> setActive (true);	// Always on...
	_IOACIA			-> setActiveForReading (true);
	_IO6529B2		-> setActive (true);	// Always on...
	_IO6529B2		-> setActiveForReading (true);
}

// ---
MCHEmul::Memory::Content C264::CPlus4Memory::standardMemoryContent ()
{
	MCHEmul::Memory::Content result = 
		std::move (C264::Memory::standardMemoryContent ());

	// The ACIA chip (used in communications)
	MCHEmul::PhysicalStorageSubset* nSS = nullptr;
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
