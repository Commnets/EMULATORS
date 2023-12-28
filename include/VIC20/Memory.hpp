/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: Memory.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 09/12/2023 \n
 *	Description: VIC20 Memory.
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_MEMORY__
#define __VIC20_MEMORY__

#include <CORE/incs.hpp>

namespace VIC20
{
	/** The memory itself for the VIC 20... */
	class Memory final : public MCHEmul::Memory
	{
		public:
		/** The different memory configurations. */
		enum class Configuration
		{
			_NOEXPANDED		= 0,
			_3KEXPANSION	= 1,
			_8KEXPANSION	= 2,
			_16KEXPANSION	= 3,
			_24KEXPANSION	= 4,
		};

		// Phisical Storages
		static const int _RAM					= 0;
		static const int _BASICROM				= 1;
		static const int _CHARROM				= 2;
		static const int _KERNELROM				= 3;

		// Subsets
		static const int _PAGEZERO_SUBSET		= 100;
		static const int _STACK_SUBSET			= 101;
		static const int _BANK0_SUBSET			= 102;
		static const int _BANK1_SUBSET			= 103;
		static const int _BANK2_SUBSET			= 104;
		static const int _BANK3_SUBSET			= 105;
		static const int _VICIRAFTER_SUBSET		= 106;
		static const int _VIAAFTER_SUBSET		= 107;
		static const int _CHARROM_SUBSET		= 108;
		static const int _BANK4_SUBSET			= 109;
		static const int _BANK5_SUBSET			= 110;
		static const int _BASICROM_SUBSET		= 111;
		static const int _KERNELROM_SUBSET		= 112;

		// Views
		static const int _CPU_VIEW				= 0;

		/** The constructor receives the posible memory configuration. */
		Memory (Configuration cfg, 
			const std::string& lang = MCHEmul::_DEFAULTLANGUAGE);

		/** To get the configuration. \n
			It has only be changed at initialization. */
		Configuration configuration () const
							{ return (_configuration); }

		/** To activate the right subsets in the CPU view. */
		virtual bool initialize () override;

		private:
		virtual MCHEmul::Stack* lookForStack () override
							{ return (dynamic_cast <MCHEmul::Stack*> (subset (_STACK_SUBSET))); }
		virtual MCHEmul::MemoryView* lookForCPUView () override
							{ return (view (_CPU_VIEW)); }

		static MCHEmul::Memory::Content standardMemoryContent (Configuration cfg);

		private:
		Configuration _configuration;
	};
}

#endif
  
// End of the file
/*@}*/
