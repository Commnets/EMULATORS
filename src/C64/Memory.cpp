#include <C64/Memory.hpp>
#include <C64/ColorMemory.hpp>
#include <C64/IO6510PortRegisters.hpp>
#include <C64/CIA1Registers.hpp>
#include <C64/CIA2Registers.hpp>
#include <C64/IOExpansionMemory.hpp>
#include <C64/Cartridge.hpp>

const MCHEmul::Address C64::Memory::_POS0_ADDRESS = MCHEmul::Address ({ 0x00, 0x00 }, false);
const MCHEmul::Address C64::Memory::_POS1_ADDRESS = MCHEmul::Address ({ 0x01, 0x00 }, false);
const MCHEmul::Address C64::Memory::_BASICROMINIT_ADDRESS = MCHEmul::Address ({ 0x00, 0xa0 }, false);
const MCHEmul::Address C64::Memory::_BASICROMEND_ADDRESS = MCHEmul::Address ({ 0xff, 0xbf }, false);
const MCHEmul::Address C64::Memory::_CHARROMINIT_ADDRESS = MCHEmul::Address ({ 0x00, 0xd0 }, false);
const MCHEmul::Address C64::Memory::_CHARROMEND_ADDRESS = MCHEmul::Address ({ 0xff, 0xdf }, false);
const MCHEmul::Address C64::Memory::_KERNELROMINIT_ADDRESS = MCHEmul::Address ({ 0x00, 0xe0 }, false);
const MCHEmul::Address C64::Memory::_KERNELROMEND_ADDRESS = MCHEmul::Address ({ 0xff, 0xff }, false);
const MCHEmul::Address C64::Memory::_COLORMEMORY_ADDRESS ({ 0x00, 0xd8 }, false);

// ---
C64::Memory::Memory (const std::string& lang)
	: MCHEmul::Memory (0, C64::Memory::standardMemoryContent (), { }),
	  _cartridge (nullptr),
	  _ram00b (nullptr),
	  _ram00b_d (nullptr),
	  _expansionRAMLO (nullptr),
	  _basicROM (nullptr),
	  _basicRAM (nullptr),
	  _basicRAM_d (nullptr),
	  _ram1 (nullptr),
	  _ram1_d (nullptr),
	  _kernelROM (nullptr),
	  _kernelRAM (nullptr),
	  _charROM (nullptr),
	  _charRAM (nullptr),
	  _vicIIRegisters (nullptr),
	  _sidRegisters (nullptr),
	  _colorRAM (nullptr),
	  _cia1Registers (nullptr),
	  _cia2Registers (nullptr),
	  _io1Registers (nullptr),
	  _io2Registers (nullptr),
	  _bank0CharROM (nullptr),
	  _bank0CharRAM (nullptr),
	  _bank0RAM2 (nullptr),
	  _bank1BRAM (nullptr),
	  _bank2CharROM (nullptr),
	  _bank2CharRAM (nullptr),
	  _bank2RAM2 (nullptr),
	  _bank3BRAM (nullptr),
	  _memStrList (),
	  _memStrNamesList ()
{
	// In the content...
	if (error () != MCHEmul::_NOERROR)
		return;

	_ram00b				= subset (_RAM00B_SUBSET);
	_ram00b_d			= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> (subset (_RAM00B_D_SUBSET));
	_expansionRAMLO		= subset (_RAM01_SUBSET);
	_basicROM			= subset (_BASICROM_SUBSET);
	_basicRAM			= subset (_BASICRAM_SUBSET);
	_basicRAM_d			= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> (subset (_BASICRAM_D_SUBSET));
	_ram1				= subset (_RAM1_SUBSET);
	_ram1_d				= dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> (subset (_RAM1_D_SUBSET));
	_kernelROM			= subset (_KERNELROM_SUBSET);
	_kernelRAM			= subset (_KERNELRAM_SUBSET);
	_charROM			= subset (_CHARROM_SUBSET);
	_charRAM			= subset (_CHARRAM_SUBSET);
	_vicIIRegisters		= subset (COMMODORE::VICIIRegisters::_VICREGS_SUBSET);
	_sidRegisters		= subset (COMMODORE::SIDRegisters::_SIDREGS_SUBSET);
	_colorRAM			= subset (C64::ColorRAMMemory::_COLOR_SUBSET);
	_cia1Registers		= subset (C64::CIA1Registers::_CIA1_SUBSET);
	_cia2Registers		= subset (C64::CIA2Registers::_CIA2_SUBSET);
	_io1Registers		= subset (C64::IOExpansionMemoryI::_IO1_SUBSET);
	_io2Registers		= subset (C64::IOExpansionMemoryII::_IO2_SUBSET);

	_bank0CharROM		= subset (_BANK0CHARROM_SUBSET);
	_bank0CharRAM		= subset (_BANK0CHARRAM_SUBSET);
	_bank0RAM2			= subset (_BANK0RAM2_SUBSET);
	_bank1BRAM			= subset (_BANK1BRAM_SUBSET);
	_bank2CharROM		= subset (_BANK2CHARROM_SUBSET);
	_bank2CharRAM		= subset (_BANK2CHARRAM_SUBSET);
	_bank2RAM2			= subset (_BANK2RAM2_SUBSET);
	_bank3BRAM			= subset (_BANK3BRAM_SUBSET);

	// Just in case...
	assert (_ram00b			!= nullptr &&
			_ram00b_d		!= nullptr &&
			_expansionRAMLO != nullptr &&
			_basicROM		!= nullptr &&
			_basicRAM		!= nullptr &&
			_basicRAM_d		!= nullptr &&
			_ram1			!= nullptr &&
			_ram1_d			!= nullptr &&
			_kernelROM		!= nullptr &&
			_kernelRAM		!= nullptr &&
			_charROM		!= nullptr &&
			_charRAM		!= nullptr &&
			_vicIIRegisters != nullptr &&
			_sidRegisters	!= nullptr &&
			_colorRAM		!= nullptr &&
			_cia1Registers	!= nullptr &&
			_cia2Registers	!= nullptr &&
			_io1Registers	!= nullptr &&
			_io2Registers	!= nullptr);

	assert (_bank0CharROM	!= nullptr &&
			_bank0CharRAM	!= nullptr &&
			_bank0RAM2		!= nullptr &&
			_bank1BRAM		!= nullptr &&
			_bank2CharROM	!= nullptr &&
			_bank2CharRAM	!= nullptr &&
			_bank2RAM2		!= nullptr &&
			_bank3BRAM		!= nullptr);

	_memStrList = {
		_ram00b,
		_ram00b_d,
		_expansionRAMLO,
		_basicROM,
		_basicRAM,
		_basicRAM_d,
		_ram1,	
		_ram1_d,
		_kernelROM,
		_kernelRAM,		
		_charROM,	
		_charRAM,		
		_vicIIRegisters,
		_sidRegisters,
		_colorRAM,
		_cia1Registers,
		_cia2Registers,
		_io1Registers,
		_io2Registers,	
		_bank0CharROM,	
		_bank0CharRAM,	
		_bank0RAM2,
		_bank1BRAM,
		_bank2CharROM,
		_bank2CharRAM,
		_bank2RAM2,
		_bank3BRAM	};

	_memStrNamesList = {
		"_ram00b",
		"_ram00b_d",
		"_expansionRAMLO",
		"_basicROM",
		"_basicRAM",
		"_basicRAM_d",
		"_ram1",	
		"_ram1_d",
		"_kernelROM",
		"_kernelRAM",		
		"_charROM",	
		"_charRAM",		
		"_vicIIRegisters",
		"_sidRegisters",
		"_colorRAM",
		"_cia1Registers",
		"_cia2Registers",
		"_io1Registers",
		"_io2Registers",	
		"_bank0CharROM",	
		"_bank0CharRAM",	
		"_bank0RAM2",
		"_bank1BRAM",
		"_bank2CharROM",
		"_bank2CharRAM",
		"_bank2RAM2",
		"_bank3BRAM" };

	// The default ROMS...
	// They might change depending on the language
	std::string ROMFILE = "./basic.901226-01.bin";
	std::string CHARROMFILE = "./characters.901225-01-ENG.bin";
	std::string KERNELFILE = "./kernal.901227-03-ENG.bin";

	// If the languaje selected is not ENG...
	// then anither char rom file is selected!
	if (lang == "ESP") { CHARROMFILE = "./characters.325056-03-ESP.bin"; }
	else if (lang == "JAP") { CHARROMFILE = "./characters.906143-02-JAP.bin"; KERNELFILE = "./kernal.906145-02-JAP.bin"; }
	else if (lang == "SWE") { CHARROMFILE = "./characters.325018-02-SWE.bin"; KERNELFILE = "./kernal.325017-02-SWE.bin"; }
	else if (lang == "DKA") { CHARROMFILE = "./characters.901225-01-DKA.bin"; KERNELFILE = "./kernal.901227-03-DKA.bin"; }

	// The info is load into the physical storage directly, 
	// but fixed into the subset above!

	bool ok = true;
	ok &= physicalStorage (_BASICROM) -> loadInto (ROMFILE);
	subset (_BASICROM_SUBSET) -> fixDefaultValues (); // Fix the values for further initializations...
	ok &= physicalStorage (_CHARROM) -> loadInto (CHARROMFILE);
	subset (_CHARROM_SUBSET) -> fixDefaultValues ();
	subset (_BANK0CHARROM_SUBSET) -> fixDefaultValues ();
	subset (_BANK2CHARROM_SUBSET) -> fixDefaultValues ();
	ok &= physicalStorage (_KERNELROM) -> loadInto (KERNELFILE);
	subset (_KERNELROM_SUBSET) -> fixDefaultValues ();

	if (!ok)
		_error = MCHEmul::_INIT_ERROR;
}

// ---
bool C64::Memory::initialize ()
{
	bool result = MCHEmul::Memory::initialize ();
	if (!result)
		return (false);

	// The active view has to be initially the CPU view...
	// ...and fill it up with initial information...
	// The ROM is not filled it up (because written is blocked using fillWith command)...
	setCPUView ();
	// In this configuration, neither the IO zone nor the CharROM are active but the RAM behind instead...
	configureMemoryStructure (false, false, false, true, false, false, false, false, false, false);
	// Just to fillup all RAM with the initial value!
	for (size_t i = 0; i < 0x10000; i += 0x40)
		fillWith (MCHEmul::Address ({ 0x00, 0x00 }) + i, 
			(((i / 0x40) % 2) == 0) ? MCHEmul::UByte::_0 : MCHEmul::UByte::_FF, 0x40);
	// In this other configuration only the IO zone is active, 
	// ...to be initialized using the 0!
	configureMemoryStructure (false, false, false, true, true, false, false, false, false, false);
	fillWith (MCHEmul::Address ({ 0x00, 0xd0 }, false), MCHEmul::UByte::_0, 0x1000);

	// Once the initialization is done, it is time to set up the right initial configuration...
	// The initial basic configuration of the memory is the standard
	// BASIC, KERNEL connected, CHARROM not connected, and no cartridge extension...
	configureMemoryStructure (true, true, false, true, true, false, false, false, false, false);

	return (true);
}

// ---
void C64::Memory::configureMemoryStructure (bool basic, bool kernel, bool chrRomCPU, bool chrRomVIC, bool io, 
	bool romL, bool romH1, bool romH2CPU, bool romH2VIC, bool uM)
{
	// ULTIMAX situation when ROMH2 = true
	// https://www.zimmers.net/anonftp/pub/cbm/c64/html/ultimax.html

	// ---
	// From the CPU perspective
	// $0000 - $0001: 6510 IOPorts always on, nothing to activate or desactivate.
	// $0002 - $0fff: up to 4k RAM always on, nothing to activate or desactivate.
	// $1000 - $7fff: 28k RAM, active and accesible when no Ultimax...
	_ram00b				-> setActive (!uM);
	_ram00b				-> setActiveForReading (!uM);
	// $1000 - $7fff: 28k "disconnected", active and accesible when Ultimax...
	_ram00b_d			-> setActive (uM);
	_ram00b_d			-> setActiveForReading (uM);
	// $8000 - $9fff: 8k RAM, active when no Ultimax...
	_expansionRAMLO		-> setActive (!uM);
	_expansionRAMLO		-> setActiveForReading (!uM && !romL); // ...and accesible when there is not 8k cartridge connected either...
	// A cartridge (ROML = true) could be connected here instead (@see cartridge)
	// $a000 - $bfff: 8k BASIC.
	//				  Bear in mind that BASIC is "on" when no cartridge or 8k cartridge is in, 
	//				  otherwise BASIC is off and either c16K cartridge is on or ultimax mode is active...
	_basicROM			-> setActive (basic);
	_basicROM			-> setActiveForReading (basic);
	// $a000 - $bfff: 8k RAM, active when no Ultimax...
	_basicRAM			-> setActive (!uM);
	_basicRAM			-> setActiveForReading (!uM && !basic && !romH1); // ...and accesible when basic is off and no 16k cartridge is connected there...
	// $a000 - $bfff: 8k "disconnected", active and accesible in Ultimax mode...
	_basicRAM_d			-> setActive (uM);
	_basicRAM_d			-> setActiveForReading (uM);
	// $c000 - $cfff: 4k RAM, active and accesible when no Ultimax...
	_ram1				-> setActive (!uM);
	_ram1				-> setActiveForReading (!uM);
	// $c000 - $cfff: 4k "disconnected", active in Ultimax mode...
	_ram1_d				-> setActive (uM);
	_ram1_d				-> setActiveForReading (uM);
	// $d000 - $dfff: 4k CHARROM.
	_charROM			-> setActive (chrRomCPU);
	_charROM			-> setActiveForReading (chrRomCPU);
	// $d000 - $dfff: 4K IO.
	//				  Bear in mind than when charROM is on io is off and viceversa...
	//				  But there might be "circunstances" where both are off and the RAM behind y full accesible!
	_vicIIRegisters		-> setActive (io);
	_vicIIRegisters		-> setActiveForReading (io);
	_sidRegisters		-> setActive (io);
	_sidRegisters		-> setActiveForReading (io);
	_colorRAM			-> setActive (io);
	_colorRAM			-> setActiveForReading (io);
	_cia1Registers		-> setActive (io);
	_cia1Registers		-> setActiveForReading (io);
	_cia2Registers		-> setActive (io);
	_cia2Registers		-> setActiveForReading (io);
	_io1Registers		-> setActive (io);
	_io1Registers		-> setActiveForReading (io);
	_io2Registers		-> setActive (io);
	_io2Registers		-> setActiveForReading (io);
	// $d000 - $dfff: 4k RAM, active when no io is accesible (it is RAM too with major priority)
	_charRAM			-> setActive (!uM && !io);
	_charRAM			-> setActiveForReading (!uM && !chrRomCPU && !io); // ...and accesible when no charROM and io is accesible either...
	// $e000 - $ffff: 8k KERNEL.
	_kernelROM			-> setActive (kernel);
	_kernelROM			-> setActiveForReading (kernel);
	// $e000 - $ffff: 8k RAM, active when no Ultimax mode...
	_kernelRAM			-> setActive (!uM);
	_kernelRAM			-> setActiveForReading (!uM && !kernel && !romH2CPU); // ...and accesible when no cartridge connected there either...
	// ---

	// ---
	// From VICII perspective
	_bank0CharROM		-> setActive (chrRomVIC);
	_bank0CharROM		-> setActiveForReading (chrRomVIC);
	_bank0CharRAM		-> setActive (!chrRomVIC);
	_bank0CharRAM		-> setActiveForReading (!chrRomVIC);
	_bank0RAM2			-> setActive (!romH2VIC); // Reference to ROMH instead...
	_bank0RAM2			-> setActiveForReading (!romH2VIC);
	_bank1BRAM			-> setActive (!romH2VIC); // Reference to ROMH instead...
	_bank1BRAM			-> setActiveForReading (!romH2VIC);
	_bank2CharROM		-> setActive (chrRomVIC);
	_bank2CharROM		-> setActiveForReading (chrRomVIC);
	_bank2CharRAM		-> setActive (!chrRomVIC);
	_bank2CharRAM		-> setActiveForReading (!chrRomVIC);
	_bank2RAM2			-> setActive (!romH2VIC); // Reference to ROMH instead...
	_bank2RAM2			-> setActiveForReading (!romH2VIC);
	_bank3BRAM			-> setActive (!romH2VIC); // Reference to ROMH instead...
	_bank3BRAM			-> setActiveForReading (!romH2VIC);
	// ---

	if (_cartridge != nullptr)
		_cartridge -> configureMemoryStructure (romL, romH1, romH2CPU, romH2VIC);

	// Print out the status of the configuration if debug active...
	if (deepDebugActive ())
	{
		auto bN =  [](bool st) -> std::string 
			{ return (st ? "YES" : "NO"); };
		auto mSts = [=](MCHEmul::PhysicalStorageSubset* p) -> std::string 
			{ return ("(" + bN (p -> active ()) + "," + bN (p -> activeForReading ()) + ")"); };

		bool fl = true;
		std::string sts;
		for (size_t i = 0; i < _memStrList.size (); i++)
		{
			if (((i % 4) == 0))
			{
				sts += "\n\t\t\t\t"; // Every 4 change the line but maintain the indent...

				fl = true;
			}

			sts += (!fl ? "," : "") + _memStrNamesList [i] + mSts (_memStrList [i]);

			fl = false;
		}

		*_deepDebugFile
			// Where
			<< "C64 Memory\t" 
			// When
			<< "-" << "\t" // clock cycles at that point
			// What
			<< "Change Configuration\t\t"
			// Data
			<< sts << "\n";
	}
}

// ---
void C64::Memory::loadDataBlockInRAM (const MCHEmul::DataMemoryBlock& dB)
{
	// Keep what the situation of the memory is now...
	std::vector <bool> aM, aRM;
	for (const auto& i : _memStrList)
	{
		aM.push_back (i -> active ());

		aRM.push_back (i -> activeForReading ());
	}

	// All RAM open...
	configureMemoryStructure (false, false, false, true, false, false, false, false, false, false);

	// Load the information...
	set (dB);

	// Restore the previous situation...
	for (size_t i = 0; i < aM.size (); i++)
	{
		_memStrList [i] -> setActive (aM [i]);

		_memStrList [i] -> setActiveForReading (aRM [i]);
	}
}

// ---
MCHEmul::Memory::Content C64::Memory::standardMemoryContent ()
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
	MCHEmul::PhysicalStorage* IORAM =
		new MCHEmul::PhysicalStorage (_IORAM, MCHEmul::PhysicalStorage::Type::_RAM, 0x1000);			// 4k

	// The map of phisical storages, used later...
	MCHEmul::PhysicalStorages storages (
		{
			{ _RAM, RAM },
			{ _BASICROM, BASICROM },
			{ _CHARROM, CHARROM },
			{ _KERNELROM, KERNELROM },
			{ _IORAM, IORAM }
		});

	// Subsets
	// The view from the CPU
	// ------
	// ...Over the RAM and ROM as the CPU sees it
	// First two bytes of the memory that are used by the 6510
	MCHEmul::PhysicalStorageSubset* IO6510 = new C64::IO6510PortRegisters
		(/** id = C64::IO6510PortRegisters::_IO6510REGISTERS_SUBSET */ RAM);
	
	// 0.25K: Page 0 (except the two first bytes).
	MCHEmul::PhysicalStorageSubset* PageZero = new MCHEmul::PhysicalStorageSubset 
		(_PAGEZERO_SUBSET, RAM, 0x0002, MCHEmul::Address ({ 0x02, 0x00 }, false), 0x00fe);
	
	// 0.25K: Stack
	MCHEmul::Stack* Stack = new MCHEmul::Stack 
		(_STACK_SUBSET, RAM, 0x0100, MCHEmul::Address ({ 0x00, 0x01 }, false), 0x0100);
	
	// 39.5k: RAM
	// Pure RAM. A piece used by BASIC
	// This piece is divided in two: The rest of the page 1 up to the first 4k (3.5k) and from 4k to 32k...
	// This is because in mode ULTIMAX the only accesible RAM is the first 4k, so the rest must be disconnected...
	MCHEmul::PhysicalStorageSubset* RAM00A = new MCHEmul::PhysicalStorageSubset 
		(_RAM00A_SUBSET, RAM, 0x0200, MCHEmul::Address ({ 0x00, 0x02 }, false), 0x0e00); 				// 3,5k pure (a bit used by BASIC)
	MCHEmul::PhysicalStorageSubset* RAM00B = new MCHEmul::PhysicalStorageSubset 
		(_RAM00B_SUBSET, RAM, 0x1000, MCHEmul::Address ({ 0x00, 0x10 }, false), 0x7000); 				// 28k pure (a bit used by BASIC)
	MCHEmul::EmptyPhysicalStorageSubset* RAM00B_D = new MCHEmul::EmptyPhysicalStorageSubset
		(_RAM00B_D_SUBSET, MCHEmul::UByte::_0, RAM, 0x1000, MCHEmul::Address ({ 0x00, 0x10 }, false), 0x7000); 	// The version of that previous 28k in ULTIMAX
	// The next 8k of RAM can be shared with an expansion element (ROML) when active...
	MCHEmul::PhysicalStorageSubset* RAM01 = new MCHEmul::PhysicalStorageSubset
		(_RAM01_SUBSET, RAM, 0x8000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x2000);					// 8k

	// 8k: BASIC
	// Where the basic is can be either ROM or RAM also (depends on bits in position 1 of the page 0) or even
	// being ocuppied by a expansion rom (when uses it. @see cartridge)
	MCHEmul::PhysicalStorageSubset* BasicROM = new MCHEmul::PhysicalStorageSubset
		(_BASICROM_SUBSET, BASICROM, 0x0000, MCHEmul::Address ({ 0x00, 0xa0 }, false), 0x2000);	
	MCHEmul::PhysicalStorageSubset* BasicRAM = new MCHEmul::PhysicalStorageSubset
		(_BASICRAM_SUBSET, RAM, 0x0a000, MCHEmul::Address ({ 0x00, 0xa0 }, false), 0x2000);				// 8k (over Basic ROM)
	// And this portion of the ram can be disconnected when the Ultimax mode is active (@see Cartridge)
	MCHEmul::EmptyPhysicalStorageSubset* BasicRAM_D = new MCHEmul::EmptyPhysicalStorageSubset
		(_BASICRAM_D_SUBSET, MCHEmul::UByte::_0, RAM, 0x0a000, MCHEmul::Address ({ 0x00, 0xa0 }, false), 0x2000);

	// 4k: RAM
	MCHEmul::PhysicalStorageSubset* RAM1 = new MCHEmul::PhysicalStorageSubset
		(_RAM1_SUBSET, RAM, 0x0c000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x1000);					// 4k
	// The same 4k but disconnected when the Ultimax mode is active (@see Cartridge)
	MCHEmul::EmptyPhysicalStorageSubset* RAM1_D = new MCHEmul::EmptyPhysicalStorageSubset
		(_RAM1_D_SUBSET, MCHEmul::UByte::_0, RAM, 0x0c000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x1000);

	// 4k: CHARROM
	// Where the CharROM is defined we have also the access to the chips (VIC, SID,...)
	MCHEmul::PhysicalStorageSubset* CharROM = new MCHEmul::PhysicalStorageSubset
		(_CHARROM_SUBSET, CHARROM, 0x0000, MCHEmul::Address ({ 0x00, 0xd0 }, false), 0x1000);
	MCHEmul::PhysicalStorageSubset* CharRAM = new MCHEmul::PhysicalStorageSubset
		(_CHARRAM_SUBSET, RAM, 0xd000, MCHEmul::Address ({ 0x00, 0xd0 }, false), 0x1000);				// 4k behing the char ROM (initially desactivated)...

	// 4k: IO Zone
	MCHEmul::PhysicalStorageSubset* VICIIRegisters = new COMMODORE::VICIIRegisters 
		(/** id = COMMODORE::VICIIRegisters::_VICREGS_SUBSET */ IORAM, 0x0000, MCHEmul::Address ({ 0x00, 0xd0 }, false), 0x0400);
	MCHEmul::PhysicalStorageSubset* SIDRegisters = new COMMODORE::SIDRegisters 
		(/** id = COMMODORE::SIDRegisters::_SIDREGS_SUBSET */ IORAM, 0x0400, MCHEmul::Address ({ 0x00, 0xd4 }, false), 0x0400);
	MCHEmul::PhysicalStorageSubset* ColorRAM = new C64::ColorRAMMemory 
		(/** id = C64::ColorRam::_COLOR_SUBSET */ IORAM, 0x0800, MCHEmul::Address ({ 0x00, 0xd8 }, false), 0x0400);
	MCHEmul::PhysicalStorageSubset* CIA1 = new C64::CIA1Registers 
		(/** id = C64::CIA1Registers::_CIA1_SUBSET */ IORAM, 0x0c00, MCHEmul::Address ({ 0x00, 0xdc }, false), 0x0100);
	MCHEmul::PhysicalStorageSubset* CIA2 = new C64::CIA2Registers 
		(/** id = C64::CIA2Registers::_CIA2_SUBSET */ IORAM, 0x0d00, MCHEmul::Address ({ 0x00, 0xdd }, false), 0x0100);
	// Used by the expansion cartridges...
	MCHEmul::PhysicalStorageSubset* IO1 = new C64::IOExpansionMemoryI
		(/** id = C64::IOExpansionMemoryI::_IO1_SUBSET */ IORAM, 0x0e00, MCHEmul::Address ({ 0x00, 0xde }, false), 0x0100);
	MCHEmul::PhysicalStorageSubset* IO2 = new C64::IOExpansionMemoryII
		(/** id = C64::IOExpansionMemoryII::_IO2_SUBSET */ IORAM, 0x0f00, MCHEmul::Address ({ 0x00, 0xdf }, false), 0x0100);

	// 8k: KERNEL
	// Where the kernel is defined can be either RAM or ROM (depending on the bits 0, 1, 2 at 0x01 position)
	// And also this positions can be replaced by an external expansion (@see Cartridge)
	MCHEmul::PhysicalStorageSubset* KernelROM = new MCHEmul::PhysicalStorageSubset 
		(_KERNELROM_SUBSET, KERNELROM, 0x0000, MCHEmul::Address ({ 0x00, 0xe0 }, false), 0x2000);
	MCHEmul::PhysicalStorageSubset* KernelRAM = new MCHEmul::PhysicalStorageSubset 
		(_KERNELRAM_SUBSET, RAM, 0xe000, MCHEmul::Address ({ 0x00, 0xe0 }, false), 0x2000);				// 8k (over Kernel ROM)
	// ------

	// To set the names of the different memory zones...
	IO6510			-> setName ("6510 Ports");
	PageZero		-> setName ("Page Zero");
	Stack			-> setName ("Stack");
	RAM00A			-> setName ("RAM Rest up to first 4k");
	RAM00B			-> setName ("RAM from 4k to 32k");
	RAM00B_D		-> setName ("Hole from 4k to 32k Ultimax");
	RAM01			-> setName ("RAM from 32k to 40k");
	BasicROM		-> setName ("Basic ROM");
	BasicRAM		-> setName ("Basic RAM");
	BasicRAM_D		-> setName ("Hole Basic RAM Ultimax");
	RAM1			-> setName ("RAM from 48k to 52k");
	RAM1_D			-> setName ("Hole from 48k to 52 Ultimax");
	CharROM			-> setName ("Character ROM");
	CharRAM			-> setName ("Character RAM");
	VICIIRegisters	-> setName ("VICII");
	SIDRegisters	-> setName ("SID");
	ColorRAM		-> setName ("Color RAM");
	CIA1			-> setName ("CIA1");
	CIA2			-> setName ("CIA2");
	IO1				-> setName ("IO1");
	IO2				-> setName ("IO2");
	KernelROM		-> setName ("Kernel ROM");
	KernelRAM		-> setName ("Kernel RAM");

	// A map with the subsets from the CPU perspective
	MCHEmul::PhysicalStorageSubsets cpusubsets (
		{
			{ C64::IO6510PortRegisters::_IO6510REGISTERS_SUBSET,	IO6510}, 
			{ _PAGEZERO_SUBSET,										PageZero }, 
			{ _STACK_SUBSET,										Stack }, 
			{ _RAM00A_SUBSET,										RAM00A }, 
			{ _RAM00B_SUBSET,										RAM00B }, 
			{ _RAM00B_D_SUBSET,										RAM00B_D }, 
			{ _RAM01_SUBSET,										RAM01 }, 
			{ _BASICROM_SUBSET,										BasicROM }, 
			{ _BASICRAM_SUBSET,										BasicRAM }, 
			{ _BASICRAM_D_SUBSET,									BasicRAM_D }, 
			{ _RAM1_SUBSET,											RAM1 }, 
			{ _RAM1_D_SUBSET,										RAM1_D }, 
			{ _CHARROM_SUBSET,										CharROM }, 
			{ _CHARRAM_SUBSET,										CharRAM }, 
			{ COMMODORE::VICIIRegisters::_VICREGS_SUBSET,			VICIIRegisters }, 
			{ COMMODORE::SIDRegisters::_SIDREGS_SUBSET,				SIDRegisters }, 
			{ C64::ColorRAMMemory::_COLOR_SUBSET,					ColorRAM }, 
			{ C64::CIA1Registers::_CIA1_SUBSET,						CIA1 }, 
			{ C64::CIA2Registers::_CIA2_SUBSET,						CIA2 }, 
			{ C64::IOExpansionMemoryI::_IO1_SUBSET,					IO1}, 
			{ C64::IOExpansionMemoryII::_IO2_SUBSET,				IO2}, 
			{ _KERNELROM_SUBSET,									KernelROM }, 
			{ _KERNELRAM_SUBSET,									KernelRAM }
		});

	// ------
	// And same like the VICII chips sees it...
	// Bank 0
	MCHEmul::PhysicalStorageSubset* Bank0RAM0 = new MCHEmul::PhysicalStorageSubset 
		(_BANK0RAM0_SUBSET, RAM, 0x0000, MCHEmul::Address ({ 0x00, 0x00 }, false), 0x1000);
	MCHEmul::PhysicalStorageSubset* Bank0CharROM = new MCHEmul::PhysicalStorageSubset 
		(_BANK0CHARROM_SUBSET, CHARROM, 0x0000, MCHEmul::Address ({ 0x00, 0x10 }, false), 0x1000); // 2k per char set...
	// When the Ultimax video is set, the previous subset is off and this one is on...
	MCHEmul::PhysicalStorageSubset* Bank0CharRAM = new MCHEmul::PhysicalStorageSubset
		(_BANK0CHARRAM_SUBSET, RAM, 0x1000, MCHEmul::Address ({ 0x00, 0x10 }, false), 0x1000);
	MCHEmul::PhysicalStorageSubset* Bank0RAM1 = new MCHEmul::PhysicalStorageSubset 
		(_BANK0RAM1_SUBSET, RAM, 0x2000, MCHEmul::Address ({ 0x00, 0x20 }, false), 0x1000);
	MCHEmul::PhysicalStorageSubset* Bank0RAM2 = new MCHEmul::PhysicalStorageSubset 
		(_BANK0RAM2_SUBSET, RAM, 0x3000, MCHEmul::Address ({ 0x00, 0x30 }, false), 0x1000);

	// Bank 1
	MCHEmul::PhysicalStorageSubset* Bank1RAM = new MCHEmul::PhysicalStorageSubset 
		(_BANK1RAM_SUBSET, RAM, 0x4000, MCHEmul::Address ({ 0x00, 0x40 }, false), 0x3000);
	MCHEmul::PhysicalStorageSubset* Bank1BRAM = new MCHEmul::PhysicalStorageSubset 
		(_BANK1BRAM_SUBSET, RAM, 0x7000, MCHEmul::Address ({ 0x00, 0x70 }, false), 0x1000);

	// Bank 2
	MCHEmul::PhysicalStorageSubset* Bank2RAM0 = new MCHEmul::PhysicalStorageSubset 
		(_BANK2RAM0_SUBSET, RAM, 0x8000, MCHEmul::Address ({ 0x00, 0x80 }, false), 0x1000);
	MCHEmul::PhysicalStorageSubset* Bank2CharROM = new MCHEmul::PhysicalStorageSubset 
		(_BANK2CHARROM_SUBSET, CHARROM, 0x0000, MCHEmul::Address ({ 0x00, 0x90 }, false), 0x1000); // 2K per char set...
	// When the Ultimax video is set, the previous subset is off and this one is on...
	MCHEmul::PhysicalStorageSubset* Bank2CharRAM = new MCHEmul::PhysicalStorageSubset
		(_BANK2CHARRAM_SUBSET, RAM, 0x9000, MCHEmul::Address ({ 0x00, 0x90 }, false), 0x1000);
	MCHEmul::PhysicalStorageSubset* Bank2RAM1 = new MCHEmul::PhysicalStorageSubset 
		(_BANK2RAM1_SUBSET, RAM, 0xa000, MCHEmul::Address ({ 0x00, 0xa0 }, false), 0x1000);
	MCHEmul::PhysicalStorageSubset* Bank2RAM2 = new MCHEmul::PhysicalStorageSubset 
		(_BANK2RAM2_SUBSET, RAM, 0xb000, MCHEmul::Address ({ 0x00, 0xb0 }, false), 0x1000);

	// Bank 3
	MCHEmul::PhysicalStorageSubset* Bank3RAM = new MCHEmul::PhysicalStorageSubset 
		(_BANK3RAM_SUBSET, RAM, 0xc000, MCHEmul::Address ({ 0x00, 0xc0 }, false), 0x3000);
	MCHEmul::PhysicalStorageSubset* Bank3BRAM = new MCHEmul::PhysicalStorageSubset 
		(_BANK3BRAM_SUBSET, RAM, 0xf000, MCHEmul::Address ({ 0x00, 0xf0 }, false), 0x1000);
	// ------

	// To set the names of the different memory zones
	Bank0RAM0		-> setName ("Bank 0 RAM from 0k to 4k");
	Bank0CharROM	-> setName ("Bank 0 Character ROM");
	Bank0CharRAM	-> setName ("Bank 0 RAM from 4k to 8k Ultimax");
	Bank0RAM1		-> setName ("Bank 0 RAM from 8k to 12k");
	Bank0RAM2		-> setName ("Bank 0 RAM from 12k to 16k");
	Bank1RAM		-> setName ("Bank 1 RAM from 0k to 12k");
	Bank1BRAM		-> setName ("Bank 1 RAM from 12k to 16k");
	Bank2RAM0		-> setName ("Bank 2 RAM from 0k to 4k");
	Bank2CharROM	-> setName ("Bank 2 Character ROM");
	Bank2CharRAM	-> setName ("Bank 2 RAM from 4k to 8k");
	Bank2RAM1		-> setName ("Bank 2 RAM from 8k to 12k");
	Bank2RAM2		-> setName ("Bank 2 RAM from 12k to 16k");
	Bank3RAM		-> setName ("Bank 1 RAM from 0k to 12k");
	Bank3BRAM		-> setName ("Bank 1 RAM from 12k to 16k");

	// The map with the subsets from the VICII perspective
	MCHEmul::PhysicalStorageSubsets vicIIsubsets (
		{
			{ _BANK0RAM0_SUBSET,	Bank0RAM0 },
			{ _BANK0CHARROM_SUBSET,	Bank0CharROM },
			{ _BANK0CHARRAM_SUBSET,	Bank0CharRAM },
			{ _BANK0RAM1_SUBSET,	Bank0RAM1 },
			{ _BANK0RAM2_SUBSET,	Bank0RAM2 },
			{ _BANK1RAM_SUBSET,		Bank1RAM },
			{ _BANK1BRAM_SUBSET,	Bank1BRAM },
			{ _BANK2RAM0_SUBSET,	Bank2RAM0 },
			{ _BANK2CHARROM_SUBSET,	Bank2CharROM },
			{ _BANK2CHARRAM_SUBSET,	Bank2CharRAM },
			{ _BANK2RAM1_SUBSET,	Bank2RAM1 },
			{ _BANK2RAM2_SUBSET,	Bank2RAM2 },
			{ _BANK3RAM_SUBSET,		Bank3RAM },
			{ _BANK3BRAM_SUBSET,	Bank3BRAM }
		});

	// A map with all the subsets possible...
	MCHEmul::PhysicalStorageSubsets allsubsets (
		{
			{ C64::IO6510PortRegisters::_IO6510REGISTERS_SUBSET,	IO6510}, 
			{ _PAGEZERO_SUBSET,										PageZero }, 
			{ _STACK_SUBSET,										Stack }, 
			{ _RAM00A_SUBSET,										RAM00A }, 
			{ _RAM00B_SUBSET,										RAM00B }, 
			{ _RAM00B_D_SUBSET,										RAM00B_D }, 
			{ _RAM01_SUBSET,										RAM01 }, 
			{ _BASICROM_SUBSET,										BasicROM }, 
			{ _BASICRAM_SUBSET,										BasicRAM }, 
			{ _BASICRAM_D_SUBSET,									BasicRAM_D }, 
			{ _RAM1_SUBSET,											RAM1 }, 
			{ _RAM1_D_SUBSET,										RAM1_D }, 
			{ _CHARROM_SUBSET,										CharROM }, 
			{ _CHARRAM_SUBSET,										CharRAM }, 
			{ COMMODORE::VICIIRegisters::_VICREGS_SUBSET,			VICIIRegisters }, 
			{ COMMODORE::SIDRegisters::_SIDREGS_SUBSET,				SIDRegisters }, 
			{ C64::ColorRAMMemory::_COLOR_SUBSET,					ColorRAM }, 
			{ C64::CIA1Registers::_CIA1_SUBSET,						CIA1 }, 
			{ C64::CIA2Registers::_CIA2_SUBSET,						CIA2 }, 
			{ C64::IOExpansionMemoryI::_IO1_SUBSET,					IO1}, 
			{ C64::IOExpansionMemoryII::_IO2_SUBSET,				IO2}, 
			{ _KERNELROM_SUBSET,									KernelROM }, 
			{ _KERNELRAM_SUBSET,									KernelRAM },
			{ _BANK0RAM0_SUBSET,									Bank0RAM0 },
			{ _BANK0CHARROM_SUBSET,									Bank0CharROM},
			{ _BANK0CHARRAM_SUBSET,									Bank0CharRAM},
			{ _BANK0RAM1_SUBSET,									Bank0RAM1 },
			{ _BANK0RAM2_SUBSET,									Bank0RAM2 },
			{ _BANK1RAM_SUBSET,										Bank1RAM },
			{ _BANK1BRAM_SUBSET,									Bank1BRAM },
			{ _BANK2RAM0_SUBSET,									Bank2RAM0 },
			{ _BANK2CHARROM_SUBSET,									Bank2CharROM},
			{ _BANK2CHARRAM_SUBSET,									Bank2CharRAM},
			{ _BANK2RAM1_SUBSET,									Bank2RAM1 },
			{ _BANK2RAM2_SUBSET,									Bank2RAM2 },
			{ _BANK3RAM_SUBSET,										Bank3RAM },
			{ _BANK3BRAM_SUBSET,									Bank3BRAM }
		});

	// Then the views...
	// From the CPU side...
	MCHEmul::MemoryView* CPUView = new MCHEmul::MemoryView (_CPU_VIEW, cpusubsets);
	// ...and from the VICII
	MCHEmul::MemoryView* VICIIView = new MCHEmul::MemoryView (_VICII_VIEW, vicIIsubsets);

	// ...and finally the memory that is the result...
	MCHEmul::Memory::Content result;
	result._physicalStorages = storages;
	result._subsets = allsubsets;
	result._views = MCHEmul::MemoryViews (
		{
			{ _CPU_VIEW, CPUView },
			{ _VICII_VIEW, VICIIView }
		});

	return (result);
}
