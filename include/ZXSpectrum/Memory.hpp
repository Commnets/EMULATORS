/** \ingroup ZXSPECTRUM */
/*@{*/

/**	
 *	@file	
 *	File: Memory.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/08/2024 \n
 *	Description: ZXSpectrum Memory.
 *	Versions: 1.0 Initial
 */

#ifndef __ZXSPECTRUM_MEMORY__
#define __ZXSPECTRUM_MEMORY__

#include <CORE/incs.hpp>
#include <ZXSpectrum/Type.hpp>

namespace ZXSPECTRUM
{
	class SinclairZXSpectrum;

	/** The memory itself for the ZXSpectrum. */
	class Memory final : public MCHEmul::Memory
	{
		public:
		/** The different memory configurations. */
		enum class Configuration
		{
			_16K	= 0,
			_48K	= 1
		};

		// Phisical Storages
		static const int _ROM_SET					= 1;
		static const int _RAM_SET					= 2;

		// Subsets
		// From the CPU view...
		static const int _ROM48KBASIC_SUBSET		= 100;
		static const int _RAM16K_SUBSET				= 101;
		static const int _RAM32KE_SUBSET			= 102;
		static const int _RAM48K_SUBSET				= 103;
		// From the ULA view...
		static const int _RAMULA_SUBSET				= 110;

		// Views
		static const int _CPU_VIEW					= 0;
		static const int _ULA_VIEW					= 1;

		Memory (Configuration cfg, Type t, const std::string& lang);

		/** Gets the type. */
		Type type () const
							{ return (_type); }

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
		Type _type;
		Configuration _configuration;

		// Implementation
		// From the CPU view...
		MCHEmul::PhysicalStorageSubset* _ROMBasic;
		MCHEmul::Stack* _RAM16k;
		MCHEmul::EmptyPhysicalStorageSubset* _RAM32k_E;
		MCHEmul::Stack* _RAM48k;
		// From the ULA view...
		MCHEmul::PhysicalStorageSubset* _RAMULA;
	
		// The id of the subset used for the stack...
		// that will depend on the configuration!
		int _STACK_SUBSET;
	};
}

#endif

// End of the file
/*@}*/
