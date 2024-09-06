/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: Memory.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 15/05/2021 \n
 *	Description: C64 Memory
 *	Versions: 1.0 Initial
 */

#ifndef __C64_MEMORY__
#define __C64_MEMORY__

#include <COMMODORE/incs.hpp>
#include <C64/ColorMemory.hpp>

namespace C64
{
	class Cartridge;

	/** The memory itself for the commodore 64... \n
		Ultimax schema also emulated, following the definitions at:
		https://www.zimmers.net/anonftp/pub/cbm/c64/html/ultimax.html */
	class Memory final : public MCHEmul::Memory
	{
		public:
		friend Cartridge;

		// Phisical Storages
		static const int _RAM					= 0;
		static const int _BASICROM				= 1;
		static const int _CHARROM				= 2;
		static const int _KERNELROM				= 3;
		static const int _IORAM					= 4;

		// Subsets
		// Fom CPU
		static const int _PAGEZERO_SUBSET		= 100;
		static const int _STACK_SUBSET			= 101;
		static const int _RAM00A_SUBSET			= 102;
		static const int _RAM00B_SUBSET			= 103;
		static const int _RAM00B_D_SUBSET		= 10300;
		static const int _RAM01_SUBSET			= 104;
		static const int _BASICROM_SUBSET		= 105;
		static const int _BASICRAM_SUBSET		= 106;
		static const int _BASICRAM_D_SUBSET		= 10600;
		static const int _RAM1_SUBSET			= 107;
		static const int _RAM1_D_SUBSET			= 10700;
		static const int _CHARROM_SUBSET		= 108;
		static const int _CHARRAM_SUBSET		= 109;
		static const int _KERNELROM_SUBSET		= 111;
		static const int _KERNELRAM_SUBSET		= 112;
		/** The id for the registers VICII, SID, IO Expansion,... are defined in those. */
		// From VICII
		static const int _BANK0RAM0_SUBSET		= 200;
		static const int _BANK0CHARROM_SUBSET	= 201;
		static const int _BANK0CHARRAM_SUBSET	= 20100;
		static const int _BANK0RAM1_SUBSET		= 202;
		static const int _BANK0RAM2_SUBSET		= 203;
		static const int _BANK1RAM_SUBSET		= 204;
		static const int _BANK1BRAM_SUBSET		= 205;
		static const int _BANK2RAM0_SUBSET		= 206;
		static const int _BANK2CHARROM_SUBSET	= 207;
		static const int _BANK2CHARRAM_SUBSET	= 20700;
		static const int _BANK2RAM1_SUBSET		= 208;
		static const int _BANK2RAM2_SUBSET		= 209;
		static const int _BANK3RAM_SUBSET		= 210;
		static const int _BANK3BRAM_SUBSET		= 211;

		// Memory locations for important areas of the memory...
		static const MCHEmul::Address _POS0_ADDRESS;
		static const MCHEmul::Address _POS1_ADDRESS;
		static const MCHEmul::Address _BASICROMINIT_ADDRESS;
		static const MCHEmul::Address _BASICROMEND_ADDRESS;
		static const MCHEmul::Address _CHARROMINIT_ADDRESS;
		static const MCHEmul::Address _CHARROMEND_ADDRESS;
		static const MCHEmul::Address _KERNELROMINIT_ADDRESS;
		static const MCHEmul::Address _KERNELROMEND_ADDRESS;

		/** Static address. The color memory can not be changed. */
		static const MCHEmul::Address _COLORMEMORY_ADDRESS;

		// Views
		static const int _CPU_VIEW				= 0;
		static const int _VICII_VIEW			= 1;

		Memory (const std::string& lang = MCHEmul::_DEFAULTLANGUAGE);

		/** To get a reference to the color RAM. */
		const ColorRAMMemory* colorRAM () const
							{ return (_colorRAM); }
		ColorRAMMemory* colorRAM ()
							{ return (_colorRAM); }

		/** To get a reference to the VICII Registers. */
		const COMMODORE::VICIIRegisters* vicIIRegisters () const
							{ return (_vicIIRegisters); }
		COMMODORE::VICIIRegisters* vicIIRegisters ()
							{ return (_vicIIRegisters); }

		/** To get a reference to the SID Registers. */
		const COMMODORE::SIDRegisters* sidRegisters () const
							{ return (_sidRegisters); }
		COMMODORE::SIDRegisters* sidRegisters ()
							{ return (_sidRegisters); }

		/** To activate the right subsets in the CPU view. */
		virtual bool initialize () override;

		/** 
		  *	In C64 memory several parts can be defined as RAM or ROM (from the CPU view). \n
		  *	This method is to switch on / off the different options. \n
		  *	The paremeters are:
		  * @param basic	:	True when the BASIC ROM is on.
		  * @param kernel	:	True when the KERNEL ROM is on.
		  * @param chrRomCPU:	True when the CHARACTER SET ROM is on from the CPU perspective.
		  * @param chrRomVIC:	True when the CHARACTER SET ROM is on from the VIC perspective.
		  * @param io		:	True when the IO access is on.
		  * @param romL		:	True when the EXPANSION PORT ROML is on.
		  * @param romH1	:	True when the EXPANSION PORT ROMH1 (zone at BASIC ROM) is on.
		  * @param romH2CPU	:	True when the EXPANSION PORT ROMH2 (zone at KERNEL ROM first 4k) is on from the CPU perspective.
		  * @param romH2VIC	:	True when the EXPANSION PORT ROMH2 (zone at KERNEL ROM second 4k) is on from the VIC perspective.
		  *	https://www.c64-wiki.com/wiki/Bank_Switching (very important to know all combinations possible)
		  */
		void configureMemoryStructure (bool basic, bool kernel, bool chrRomCPU, bool chrRomVIC, bool io, 
			bool romL, bool romH1, bool romH2CPU, bool romH2VIC, bool uM);

		/** Load in memory RAM a data block. \n
			In C64 the memory can be configured to have some RAM zones no directly accesible in writting like IO. \n
			This method is to load a data block just in the memory RAM. */
		void loadDataBlockInRAM (const MCHEmul::DataMemoryBlock& dB);

		private:
		virtual MCHEmul::Stack* lookForStack () override
							{ return (dynamic_cast <MCHEmul::Stack*> (subset (_STACK_SUBSET))); }
		virtual MCHEmul::MemoryView* lookForCPUView () override
							{ return (view (_CPU_VIEW)); }

		static MCHEmul::Memory::Content standardMemoryContent ();

		/** Invoked from the cartridge. */
		void setExtensionAt (Cartridge* c)
							{ _cartridge = c; }

		private:
		Cartridge* _cartridge;

		// Implementation
		/** To speed up the access to the different zones of the memory. */
		// The expansion elements in the C64 affects the way the memory is configured
		// ..activating or desactivating the different parts of it...
		MCHEmul::PhysicalStorageSubset* _ram00b;
		MCHEmul::EmptyPhysicalStorageSubset* _ram00b_d;
		MCHEmul::PhysicalStorageSubset* _expansionRAMLO;
		MCHEmul::PhysicalStorageSubset* _basicROM;
		MCHEmul::PhysicalStorageSubset* _basicRAM;
		MCHEmul::EmptyPhysicalStorageSubset* _basicRAM_d;
		MCHEmul::PhysicalStorageSubset* _ram1;
		MCHEmul::EmptyPhysicalStorageSubset* _ram1_d;
		MCHEmul::PhysicalStorageSubset* _kernelROM;
		MCHEmul::PhysicalStorageSubset* _kernelRAM;
		MCHEmul::PhysicalStorageSubset* _charROM;
		MCHEmul::PhysicalStorageSubset* _charRAM;
		COMMODORE::VICIIRegisters* _vicIIRegisters;
		COMMODORE::SIDRegisters* _sidRegisters;
		ColorRAMMemory* _colorRAM;
		MCHEmul::PhysicalStorageSubset* _cia1Registers;
		MCHEmul::PhysicalStorageSubset* _cia2Registers;
		MCHEmul::PhysicalStorageSubset* _io1Registers;
		MCHEmul::PhysicalStorageSubset* _io2Registers;

		// Similar thing from the VICII
		MCHEmul::PhysicalStorageSubset* _bank0CharROM;
		MCHEmul::PhysicalStorageSubset* _bank0CharRAM;
		MCHEmul::PhysicalStorageSubset* _bank0RAM2;
		MCHEmul::PhysicalStorageSubset* _bank1BRAM;
		MCHEmul::PhysicalStorageSubset* _bank2CharROM;
		MCHEmul::PhysicalStorageSubset* _bank2CharRAM;
		MCHEmul::PhysicalStorageSubset* _bank2RAM2;
		MCHEmul::PhysicalStorageSubset* _bank3BRAM;

		// Used when debugging...
		// All that possibilities together...
		MCHEmul::PhysicalStorageSubsetsList _memStrList;
		// ...and with their names...
		MCHEmul::Strings _memStrNamesList;
	};
}

#endif
  
// End of the file
/*@}*/
