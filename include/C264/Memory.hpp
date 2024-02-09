/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: Memory.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 04/02/2024 \n
 *	Description: C264 Memory.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_MEMORY__
#define __C264_MEMORY__

#include <CORE/incs.hpp>

namespace C264
{
	/** When a C264 is not expanded, there are seveal memory zones not connected.
		That zones, doesn't respond to poke and always return the same value when peeking 
		(at least it is as anothe emulators in the market now behave)
		This is the way (i guess) C264 determines how much free memory the system has. 
		So this class is to replicate that behaviour. */
	class NotConnectedPhysicalStorageSubset final : public MCHEmul::PhysicalStorageSubset
	{
		public:
		NotConnectedPhysicalStorageSubset (int id, const MCHEmul::UByte& dV,
				MCHEmul::PhysicalStorage* pS, size_t pp /** link a phisical */, const MCHEmul::Address& a, size_t s)
			: MCHEmul::PhysicalStorageSubset (id, pS, pp, a, s),
			  _defaultValue (dV)
							{ }

		const MCHEmul::UByte& defaultValue () const
							{ return (_defaultValue); }

		private:
		virtual void setValue (size_t nB, const MCHEmul::UByte& d) override
							{ /** Do nothing. */ }
		virtual const MCHEmul::UByte& readValue (size_t nB) const override
							{ return (_defaultValue); }

		private:
		MCHEmul::UByte _defaultValue;
	};

	/** The memory itself for the VIC 20... */
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
		// TODO
		static const int _BASICROM_SUBSET		= 104;
		static const int _BASICRAM_SUBSET		= 105;
		static const int _CHARROM_SUBSET		= 107;
		static const int _CHARRAM_SUBSET		= 108;
		static const int _KERNELROM_SUBSET		= 112;
		static const int _KERNELRAM_SUBSET		= 113;
		// TODO

		// Views
		static const int _CPU_VIEW				= 0;

		/** The constructor receives the posible memory configuration. */
		Memory (unsigned int cfg, 
			const std::string& lang = MCHEmul::_DEFAULTLANGUAGE);

		/** To get/set the configuration. \n
			It show change only at at initialization, otherwise the consecuencues are not clear. */
		unsigned int configuration () const
							{ return (_configuration); }
		/** a0 = true when 0xa000 must be active. 
			It is false by default. */
		void setConfiguration (unsigned int cfg, bool a0 = false);
		/** In this version every bit active will actibe a bank of expansion. 
			bit 1 = Block 1, bit 2 = block 2,... */
		void setConfiguration (unsigned char cfg);

		/** To activate the right subsets in the CPU view. */
		virtual bool initialize () override;

		private:
		virtual MCHEmul::Stack* lookForStack () override
							{ return (dynamic_cast <MCHEmul::Stack*> (subset (_STACK_SUBSET))); }
		virtual MCHEmul::MemoryView* lookForCPUView () override
							{ return (view (_CPU_VIEW)); }

		static MCHEmul::Memory::Content standardMemoryContent ();

		private:
		unsigned int _configuration;

		// Implementation
		MCHEmul::PhysicalStorageSubset* _basicROM;
		MCHEmul::PhysicalStorageSubset* _basicRAM;
		MCHEmul::PhysicalStorageSubset* _kernelROM;
		MCHEmul::PhysicalStorageSubset* _kernelRAM;
		MCHEmul::PhysicalStorageSubset* _charROM;
		MCHEmul::PhysicalStorageSubset* _charRAM;
		// TODO
	};
}

#endif
  
// End of the file
/*@}*/
