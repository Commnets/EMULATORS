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
	/** The memory itself for the commodore 64... */
	class Memory final : public MCHEmul::Memory
	{
		public:
		// Phisical Storages
		static const int _RAM					= 0;
		static const int _BASICROM				= 1;
		static const int _CHARROM				= 2;
		static const int _KERNELROM				= 3;

		// Subsets
		// Fom CPU
		static const int _PAGEZERO_SUBSET		= 100;
		static const int _STACK_SUBSET			= 101;
		static const int _RAM0_SUBSET			= 102;
		static const int _BASICROM_SUBSET		= 103;
		static const int _BASICRAM_SUBSET		= 104;
		static const int _RAM1_SUBSET			= 105;
		static const int _CHARROM_SUBSET		= 106;
		static const int _COLOR_SUBSET			= 107;
		static const int _IO1_SUBSET			= 112;
		static const int _IO2_SUBSET			= 113;
		static const int _KERNELROM_SUBSET		= 114;
		static const int _KERNELRAM_SUBSET		= 115;
		/** The id for the registers VICII, SID, ... are defined in those. */
		// From VICII
		static const int _BANK0RAM0_SUBSET		= 200;
		static const int _BANK0CHARROM_SUBSET	= 201;
		static const int _BANK0RAM1_SUBSET		= 202;
		static const int _BANK1RAM_SUBSET		= 203;
		static const int _BANK2RAM0_SUBSET		= 204;
		static const int _BANK2CHARROM_SUBSET	= 205;
		static const int _BANK2RAM1_SUBSET		= 206;
		static const int _BANK3RAM_SUBSET		= 207;

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
		  *	@param lR	: LORAM access ($a000 - $bfff). true = BASIC ROM, false = RAM. 
		  *	@param hR	: HIRAM access ($e000 - $ffff). true = KERNEL ROM, false = RAM. 
		  * @param c	: CHAREN acsess ($d000 - $dfff). true = CHARROM (from VICII) & IO (from CPU), false = RAM.
		  */
		void configureMemoryAccess (bool lR, bool hR, bool c);

		private:
		virtual MCHEmul::Stack* lookForStack () override
							{ return (dynamic_cast <MCHEmul::Stack*> (subset (_STACK_SUBSET))); }
		virtual MCHEmul::MemoryView* lookForCPUView () override
							{ return (view (_CPU_VIEW)); }

		static MCHEmul::Memory::Content standardMemoryContent ();

		private:
		MCHEmul::MemoryView* _VICIIView;

		// Implementation
		/** To speed up the access to the different zones of the memory. */
		// The expansion elements in the C64 affects the way the memory is configured
		// ..activating or desactivating the different parts of it...
		MCHEmul::PhysicalStorageSubset* _basicROM;
		MCHEmul::PhysicalStorageSubset* _basicRAM;
		MCHEmul::PhysicalStorageSubset* _kernelROM;
		MCHEmul::PhysicalStorageSubset* _kernelRAM;
		MCHEmul::PhysicalStorageSubset* _charROM;
		MCHEmul::PhysicalStorageSubset* _vicIIRegisters;
		MCHEmul::PhysicalStorageSubset* _sidRegisters;
		MCHEmul::PhysicalStorageSubset* _colorRAM;
		MCHEmul::PhysicalStorageSubset* _cia1Registers;
		MCHEmul::PhysicalStorageSubset* _cia2registers;
		MCHEmul::PhysicalStorageSubset* _io1Registers;
		MCHEmul::PhysicalStorageSubset* _io2registers;
	};
}

#endif
  
// End of the file
/*@}*/
