/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: Memory.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/03/2025 \n
 *	Description: MSX Memory.
 *	Versions: 1.0 Initial
 */

#ifndef __MSX_MEMORY__
#define __MSX_MEMORY__

#include <CORE/incs.hpp>
#include <MSX/Type.hpp>

namespace MSX
{
	class MSXComputer;

	/** The memory itself for the MSX. */
	class Memory final : public MCHEmul::Memory
	{
		public:
		// Phisical Storages
		static const int _ROMBIOS_SET				= 0;

		// Subsets
		// From the CPU view...
		static const int _ROMBIOS_SUBSET			= 100; // 32K...

		// The CPU View
		static const int _CPU_VIEW					 = 0;

		/** Creates the memory based on the model, the configuartion and the language. 
			The language is used mainly to load the right type of ROM. */
		Memory (MSXModel* m, unsigned int cfg, const std::string& lang);

		/** Gets the type. */
		MSXModel* model () const
							{ return (_model); }

		/** To get/set the configuration. \n
			It shows change only at initialization, otherwise the consecuencues are not clear. */
		unsigned int configuration () const
							{ return (_configuration); }
		void setConfiguration (unsigned int cfg);

		/** To activate the right subsets in the CPU view. */
		virtual bool initialize () override;

		private:
		virtual MCHEmul::Stack* lookForStack () override
							{ return (dynamic_cast <MCHEmul::Stack*> (subset (_STACK_SUBSET))); }
		virtual MCHEmul::MemoryView* lookForCPUView () override
							{ return (view (_CPU_VIEW)); }

		private:
		MSXModel* _model;
		unsigned int _configuration;

		// Implementation
		// The id of the subset used for the stack...
		// that will depend on the configuration!
		int _STACK_SUBSET;
	};
}

#endif

// End of the file
/*@}*/
