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
	class VIA1Registers;
	class VIA2Registers;

	/** When a VIC20 is not expanded, there are seveal memory zones not connected.
		That zones, doesn't respond to poke and always return the same value when peeking 
		(at least it is as anothe emulators in the market now behave)
		This is the way (i guess) VIC20 determines how much free memory the system has. 
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

	/** Some parts of the memory (usually behind the CIA/VIA registers)
		seems to be part of them, but return very random numbers when peeking. \n
		The don't respond either to the poke instructions. \n
		This class is done to emulate that. */
	class RandomPhysicalStorageSubset final : public MCHEmul::PhysicalStorageSubset
	{
		public:
		RandomPhysicalStorageSubset (int id,
				MCHEmul::PhysicalStorage* pS, size_t pp /** link a phisical */, const MCHEmul::Address& a, size_t s)
			: MCHEmul::PhysicalStorageSubset (id, pS, pp, a, s)
							{ }

		private:
		virtual void setValue (size_t nB, const MCHEmul::UByte& d) override
							{ /** Do nothing. */ }
		virtual const MCHEmul::UByte& readValue (size_t nB) const override
							{ return (_lastValueRead = MCHEmul::UByte ((unsigned char) (std::rand () % 256))); }

		private:
		mutable MCHEmul::UByte _lastValueRead;
	};

	/** The reflection of the VIA1/VIA2 registers is a little bit strange... \n
		VIA1 registers seem to be reflected in address from $9130 to $93ff every $10 (= 16) bytes.
		But VIA2 registers seem not be reflected and they always return 0, and they don't anser either to poke.
		This class is just to replicate this behaviour. */
	class VIA1VIA2RegistersReflection final : public MCHEmul::PhysicalStorageSubset
	{
		public:
		VIA1VIA2RegistersReflection (int id, VIA1Registers* v1r, VIA2Registers* v2r,
				MCHEmul::PhysicalStorage* pS, size_t pp /** link a phisical */, const MCHEmul::Address& a, size_t s)
			: MCHEmul::PhysicalStorageSubset (id, pS, pp, a, s),
			  _via1Registers (v1r), _via2Registers (v2r),
			  _lastValue (MCHEmul::UByte::_0)
							{ assert (_via1Registers != nullptr && _via2Registers != nullptr); }

		private:
		virtual void setValue (size_t nB, const MCHEmul::UByte& d) override;
		virtual const MCHEmul::UByte& readValue (size_t nB) const override;

		private:
		/** References to the memory to replicate sometimes. */
		VIA1Registers* _via1Registers;
		VIA2Registers* _via2Registers;
		/** The last value read. */
		mutable MCHEmul::UByte _lastValue;
	};

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
		static const int _PAGE2AND3_SUBSET		= 102;
		static const int _BANK0_SUBSET			= 103;
		static const int _BANK0UC_SUBSET		= 104;	// The connected... (when not expanded)
		static const int _BASICAREA_SUBSET		= 105;
		static const int _BANK1_SUBSET			= 106;
		static const int _BANK1UC_SUBSET		= 107;	// The connected... (when not expanded)
		static const int _BANK2_SUBSET			= 108;
		static const int _BANK2UC_SUBSET		= 109;	// The connected... (when not expanded)
		static const int _BANK3_SUBSET			= 110;
		static const int _BANK3UC_SUBSET		= 111;	// The connected... (when not expanded)
		static const int _CHARROM_SUBSET		= 112;
		static const int _VICIRAFTER_SUBSET		= 113;
		static const int _VIAAFTER_SUBSET		= 114;
		static const int _SCREENC1RAM_SUBSET	= 115;
		static const int _SCREENC1RAMUC_SUBSET	= 116;
		static const int _SCREENC2RAM_SUBSET	= 117;
		static const int _SCREENC2RAMUC_SUBSET	= 118;
		static const int _BANK4_SUBSET			= 119;
		static const int _BANK5_SUBSET			= 120;
		static const int _BANK5UC_SUBSET		= 121;	// The not connected...
		static const int _BASICROM_SUBSET		= 122;
		static const int _KERNELROM_SUBSET		= 123;

		// Views
		static const int _CPU_VIEW				= 0;

		/** The constructor receives the posible memory configuration. */
		Memory (Configuration cfg, 
			const std::string& lang = MCHEmul::_DEFAULTLANGUAGE);

		/** To get/set the configuration. \n
			It show change only at at initialization, otherwise the consecuencues are not clear. */
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
		MCHEmul::PhysicalStorageSubset* _BANK0;
		NotConnectedPhysicalStorageSubset* _BANK0UC;
		MCHEmul::PhysicalStorageSubset* _BANK1;
		NotConnectedPhysicalStorageSubset* _BANK1UC;
		MCHEmul::PhysicalStorageSubset* _BANK2;
		NotConnectedPhysicalStorageSubset* _BANK2UC;
		MCHEmul::PhysicalStorageSubset* _BANK3;
		NotConnectedPhysicalStorageSubset* _BANK3UC;
		MCHEmul::PhysicalStorageSubset* _SCREENC1RAM;
		RandomPhysicalStorageSubset* _SCREENC1RAMUC;
		MCHEmul::PhysicalStorageSubset* _SCREENC2RAM;
		RandomPhysicalStorageSubset* _SCREENC2RAMUC;
		MCHEmul::PhysicalStorageSubset* _BANK5;
		NotConnectedPhysicalStorageSubset* _BANK5UC;
	};
}

#endif
  
// End of the file
/*@}*/
