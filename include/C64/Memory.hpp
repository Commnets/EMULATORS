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
		static const int _VICREGS_SUBSET		= 107;
		static const int _SIDREGS_SUBSET		= 108;
		static const int _COLOR_SUBSET			= 109;
		static const int _CIA1_SUBSET			= 110;
		static const int _CIA2_SUBSET			= 111;
		static const int _IO1_SUBSET			= 112;
		static const int _IO2_SUBSET			= 113;
		static const int _KERNELROM_SUBSET		= 114;
		static const int _KERNELRAM_SUBSET		= 115;
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

		Memory ();

		/** To set the VICII view, useb by the VICII. */
		void setVICIIView ()
							{ _activeView = (_VICIIView == nullptr) ? (_VICIIView = lookForVICIIView ()) : _VICIIView; }

		/** To activate the right subsets in the CPU view. */
		virtual bool initialize () override;

		private:
		virtual MCHEmul::Stack* lookForStack () override
							{ return (dynamic_cast <MCHEmul::Stack*> (subset (_STACK_SUBSET))); }
		virtual MCHEmul::MemoryView* lookForCPUView () override
							{ return (view (_CPU_VIEW)); }
		MCHEmul::MemoryView* lookForVICIIView ()
							{ return (view (_VICII_VIEW)); }

		static MCHEmul::Memory::Content standardMemoryContent ();

		private:
		MCHEmul::MemoryView* _VICIIView;
	};
}

#endif
  
// End of the file
/*@}*/
