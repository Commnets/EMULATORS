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
	/** 
	  * The video info has to be set from D_FILE location.
	  * The location D_FILE with the bit 7 on is where the CPU "executes" the video info.
	  * D_FILE is usually at some position in the range 0x4000 - onwards.
	  * This class is to mirror from C000 - onwards that previous position, 
	  * but returning "0" (= NOP) always the bit 6 of the operation is not set.
	  * When it is set the normal opcode is returned.
	  * This mirror class will be only visible from the CPU. */
	class MemoryVideoCode final : public MCHEmul::MirrorPhysicalStorageSubset
	{
		public:
		MemoryVideoCode (int id, 
			PhysicalStorageSubset* pSS, const MCHEmul::Address& a)
			: MCHEmul::MirrorPhysicalStorageSubset (id, pSS, a),
			  _lastValueRead (MCHEmul::UByte::_0)
							{ }

		private:
		virtual void setValue (size_t nB, const MCHEmul::UByte& d) override
							{ /** not for writtin anything. */ }
		virtual const MCHEmul::UByte& readValue (size_t nB) const override;

		private:
		mutable MCHEmul::UByte _lastValueRead;
	};


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
		static const int _RAM_SET					= 1;
		static const int _ROM_SET					= 2;

		// Subsets
		static const int _ROM_SUBSET				= 101;
		static const int _ROMSHADOW1_SUBSET			= 102;
		static const int _RAM1K_SUBSET				= 103;
		static const int _RAM1K_S_SUBSET			= 104; // From 104 to 118
		static const int _RAM16K_SUBSET				= 119;
		static const int _ROMSHADOW2_SUBSET			= 120;
		static const int _ROMSHADOW3_SUBSET			= 121;
		static const int _RAM1KSHADOW_SUBSET		= 122;
		static const int _RAM1KSHADOW_S_SUBSET		= 123; // From 123 to 137
		static const int _RAM16KSHADOW_SUBSET		= 138;
		static const int _RAM1K_V_SUBSET			= 139;
		static const int _RAM1K_V_S_SUBSET			= 140; // From 140 to 154
		static const int _RAM16K_V_SUBSET			= 155;

		// Views
		static const int _CPU_VIEW					= 0;
		static const int _ULA_VIEW					= 1;

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
		MCHEmul::PhysicalStorageSubset* _ROM;	// To load the data...
		// Either this 2... (and the equivalent in video zone)
		MCHEmul::Stack* _RAM1K;
		MCHEmul::MirrorPhysicalStorageSubset* _RAM1K_S;
		ZX81::MemoryVideoCode* _RAM1K_V;
		std::vector <MCHEmul::MirrorPhysicalStorageSubset*> _RAM15K_UC;
		std::vector <MCHEmul::MirrorPhysicalStorageSubset*> _RAM15K_UC_S;
		std::vector <ZX81::MemoryVideoCode*> _RAM15K_V;
		// ...or this one (and the equivalent in the video zone)
		MCHEmul::Stack* _RAM16K;
		MCHEmul::MirrorPhysicalStorageSubset* _RAM16K_S;
		ZX81::MemoryVideoCode* _RAM16K_V;
		// The id of the subset used for the stack...
		// that will depend on the configuration!
		int _STACK_SUBSET;
	};
}

#endif
  
// End of the file
/*@}*/
