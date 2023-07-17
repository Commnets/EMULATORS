/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: Memory.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 15/05/2021 \n
 *	Description: C64 Memory
 *	Versions: 1.0 Initial
 */

#ifndef __C64_MEMORY__
#define __C64_MEMORY__

#include <CORE/incs.hpp>

namespace C64
{
	class Cartridge;

	/** The memory itself for the commodore 64... */
	class Memory final : public MCHEmul::Memory
	{
		public:
		friend Cartridge;

		// Phisical Storages
		static const int _RAM					= 0;
		static const int _BASICROM				= 1;
		static const int _CHARROM				= 2;
		static const int _KERNELROM				= 3;

		// Subsets
		// Fom CPU
		static const int _PAGEZERO_SUBSET		= 100;
		static const int _STACK_SUBSET			= 101;
		static const int _RAM00_SUBSET			= 102;
		static const int _RAM01_SUBSET			= 103;
		static const int _BASICROM_SUBSET		= 104;
		static const int _BASICRAM_SUBSET		= 105;
		static const int _RAM1_SUBSET			= 106;
		static const int _CHARROM_SUBSET		= 107;
		static const int _CHARRAM_SUBSET		= 108;
		static const int _COLOR_SUBSET			= 109;
		static const int _KERNELROM_SUBSET		= 112;
		static const int _KERNELRAM_SUBSET		= 113;
		/** The id for the registers VICII, SID, IO Expansion,... are defined in those. */
		// From VICII
		static const int _BANK0RAM0_SUBSET		= 200;
		static const int _BANK0CHARROM_SUBSET	= 201;
		static const int _BANK0RAM1_SUBSET		= 202;
		static const int _BANK1RAM_SUBSET		= 203;
		static const int _BANK2RAM0_SUBSET		= 204;
		static const int _BANK2CHARROM_SUBSET	= 205;
		static const int _BANK2RAM1_SUBSET		= 206;
		static const int _BANK3RAM_SUBSET		= 207;

		// Memory locations for important areas of the memory...
		static const MCHEmul::Address _POS0_ADDRESS;
		static const MCHEmul::Address _POS1_ADDRESS;
		static const MCHEmul::Address _BASICROMINIT_ADDRESS;
		static const MCHEmul::Address _BASICROMEND_ADDRESS;
		static const MCHEmul::Address _CHARROMINIT_ADDRESS;
		static const MCHEmul::Address _CHARROMEND_ADDRESS;
		static const MCHEmul::Address _KERNELROMINIT_ADDRESS;
		static const MCHEmul::Address _KERNELROMEND_ADDRESS;

		// Views
		static const int _CPU_VIEW				= 0;
		static const int _VICII_VIEW			= 1;

		Memory (const std::string& lang = MCHEmul::_DEFAULTLANGUAGE);

		/** To activate the right subsets in the CPU view. */
		virtual bool initialize () override;

		/** 
		  *	In C64 memory several parts can be defined as RAM or ROM (from the CPU view). \n
		  *	This method is to switch on / off the different options. \n
		  *	The paremeters are:
		  * @param BASIC	:	True when the BASIC ROM is on.
		  * @param KERNEL	:	True when the KERNEL ROM is on.
		  * @param CHARROM	:	True when the CHARACTER SET ROM is on.
		  * @param ROML		:	True when the EXPANSION PORT ROML is on.
		  * @param ROMH1	:	True when the EXPANSION PORT ROMH1 (zone at BASIC ROM) is on.
		  * @param ROMH2	:	True when the EXPANSION PORT ROMH2 (zone at KERNEL ROM) is on.
		  * To determine whether the RAM in those places is on or off,
		  * some logic has to be applied:
		  * RAM at $8000 = !ROML
		  * RAM at $a000 = !ROMH1 AND !BASIC
		  * RAM at $e000 = !ROMH2 AND !KERNEL
		  *	https://www.c64-wiki.com/wiki/Bank_Switching (very important to know all combinations possible)
		  */
		void configureMemoryStructure (bool BASIC, bool KERNEL, bool CHARROM, 
			bool ROML, bool ROMH1, bool ROMH2);

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
		MCHEmul::MemoryView* _VICIIView;
		Cartridge* _cartridge;

		// Implementation
		/** To speed up the access to the different zones of the memory. */
		// The expansion elements in the C64 affects the way the memory is configured
		// ..activating or desactivating the different parts of it...
		MCHEmul::PhysicalStorageSubset* _basicROM;
		MCHEmul::PhysicalStorageSubset* _basicRAM;
		MCHEmul::PhysicalStorageSubset* _kernelROM;
		MCHEmul::PhysicalStorageSubset* _kernelRAM;
		MCHEmul::PhysicalStorageSubset* _charROM;
		MCHEmul::PhysicalStorageSubset* _charRAM;
		MCHEmul::PhysicalStorageSubset* _vicIIRegisters;
		MCHEmul::PhysicalStorageSubset* _sidRegisters;
		MCHEmul::PhysicalStorageSubset* _colorRAM;
		MCHEmul::PhysicalStorageSubset* _cia1Registers;
		MCHEmul::PhysicalStorageSubset* _cia2registers;
		MCHEmul::PhysicalStorageSubset* _io1Registers;
		MCHEmul::PhysicalStorageSubset* _io2registers;
		MCHEmul::PhysicalStorageSubset* _expansionRAMLO;
	};
}

#endif
  
// End of the file
/*@}*/
