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
		static const int _PAGEZERO_SUBSET		= 100;
		static const int _STACK_SUBSET			= 101;

		// Views
		static const int _CPU_VIEW				= 0;
		static const int _VICI_VIEW				= 1;

		Memory (const std::string& lang = MCHEmul::_DEFAULTLANGUAGE);

		/** To activate the right subsets in the CPU view. */
		virtual bool initialize () override;

		private:
		virtual MCHEmul::Stack* lookForStack () override
							{ return (dynamic_cast <MCHEmul::Stack*> (subset (_STACK_SUBSET))); }
		virtual MCHEmul::MemoryView* lookForCPUView () override
							{ return (view (_CPU_VIEW)); }

		static MCHEmul::Memory::Content standardMemoryContent ();
	};
}

#endif
  
// End of the file
/*@}*/
