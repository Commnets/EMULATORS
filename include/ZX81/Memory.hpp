/** \ingroup ZX81 */
/*@{*/

/**	
 *	@file	
 *	File: Memory.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 25/08/2024 \n
 *	Description: ZX81 Memory.
 *	Versions: 1.0 Initial
 */

#ifndef __ZX81_MEMORY__
#define __ZX81_MEMORY__

#include <CORE/incs.hpp>

namespace ZX81
{
	/** The memory itself for the ZX81. */
	class Memory final : public MCHEmul::Memory
	{
		public:
		/** The different memory configurations. */
		enum class Configuration
		{
			_NOEXPANDED		= 0,
			_16KEXPANSION	= 1
		};

		// Phisical Storages
		static const int _RAM_SET				= 1;
		static const int _ROM_SET				= 2;

		// Subsets
		static const int _STACK_SUBSET			= 100;	// By default the full RAM!
		static const int _ROM_SUBSET			= 101;
		static const int _ROMSHADOW_SUBSET		= 102;
		static const int _RAM1K_SUBSET			= 103;
		static const int _RAM1K_UC_SUBSET		= 104;
		static const int _RAM16K_SUBSET			= 105;

		// Views
		static const int _CPU_VIEW				= 0;

		/** The constructor receives the posible memory configuration
			and also the type of machine (indicated by the type of ROM): \n
			0 = ZX81 type 0 (with the error SQR (25)). \n
			1 = ZX81 type 1 (without the error SQR (25)). very rare. \n
			2 = ZX81 type 2 (the newest). */
		Memory (Configuration cfg, unsigned int m = 0);

		/** To get/set the configuration. \n
			It shows change only at initialization, otherwise the consecuencues are not clear. */
		Configuration configuration () const
							{ return (_configuration); }
		void setConfiguration (Configuration cfg);

		/** To activate the right subsets in the CPU view. */
		virtual bool initialize () override;

		private:
		virtual MCHEmul::Stack* lookForStack () override
							{ return (dynamic_cast <MCHEmul::Stack*> (subset (_STACK_SUBSET))); }
		virtual MCHEmul::MemoryView* lookForCPUView () override
							{ return (view (_CPU_VIEW)); }

		static MCHEmul::Memory::Content standardMemoryContent ();

		private:
		Configuration _configuration;

		// Implementation
		// The banks of memory that are configurable...
		MCHEmul::PhysicalStorageSubset* _ROM;
		MCHEmul::MirrorPhysicalStorageSubset* _ROMSHADOW;
		MCHEmul::PhysicalStorageSubset* _RAM1K;
		MCHEmul::EmptyPhysicalStorageSubset* _RAM1K_UC;
		MCHEmul::PhysicalStorageSubset* _RAM16K;
	};
}

#endif
  
// End of the file
/*@}*/
