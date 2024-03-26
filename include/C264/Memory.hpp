/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: Memory.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 04/02/2024 \n
 *	Description: C264 Memory.
 *				 The memory is configured in C264 using a combination of PLA (7700-10) and
 *				 a set of flip-flops (74LS175) and decodifiers (74LS139)
 *				 I haven't found detail info about how they works, but "reading" the shematic-diagrams
 *				 something can be programmed!
 *				 see: https://myoldcomputer.nl/wp-content/uploads/2016/02/c16_251788_1.jpg
 *				 see: https://pdf1.alldatasheet.es/datasheet-pdf/download/5674/MOTOROLA/74LS175.html
 *				 see: https://pdf1.alldatasheet.com/datasheet-pdf/download/5657/MOTOROLA/74LS139.html
 *	Versions: 1.0 Initial
 */

#ifndef __C264_MEMORY__
#define __C264_MEMORY__

#include <COMMODORE/incs.hpp>

namespace C264
{
	class TED;

	/** The memory itself for the C264 Series... 
		There will be some classes inheriting this, 
		as the memory of the different machines is not the same, 
		because they don't event have the same chips attached!.  */
	class Memory : public MCHEmul::Memory
	{
		public:
		friend TED;

		// Phisical Storages
		static const int _RAM					= 0;
		static const int _BASICROM				= 1;
		static const int _KERNELROM				= 2;

		// Subsets that exists from the beginning in ant type of machine...
		static const int _IO7501PORT_SUBSET		= 100;
		static const int _PAGEZERO_SUBSET		= 101;
		static const int _STACK_SUBSET			= 102;
		static const int _RAM1_SUBSET			= 103;
		static const int _RAM2_SUBSET			= 104;
		static const int _BASICROM_SUBSET		= 105;
		static const int _RAM3_SUBSET			= 106;
		static const int _CARLOW1_SUBSET		= 107;	// Memory not connected...
		static const int _CARLOW2_SUBSET		= 108;	// Memory not connected...
		static const int _KERNELROM1_SUBSET		= 109;
		static const int _KERNELROM2_SUBSET		= 110;
		static const int _RAM4_SUBSET			= 111;
		static const int _CARHIGH1_SUBSET		= 112;	// Memory not connected...
		static const int _CARHIGH2_SUBSET		= 113;	// Memory not connected...
		static const int _IORAM0_SUBSET			= 114;	// Memory not connected...
		static const int _IORAM1_SUBSET			= 115;	// Memory not connected...
		static const int _IORAM2_SUBSET			= 116;	// Memory not connected...
		static const int _IORAM3_SUBSET			= 117;	// Memory not connected...
		static const int _IORAM4_SUBSET			= 118;	// Memory not connected...
		static const int _KERNELROM3_SUBSET		= 119;
		static const int _RAM5_SUBSET			= 120;

		// Subsets that should be used in case cartridges are connected 
		// The configuration method uses this to llok for they to exist!
		static const int _CARLOW1CON_SUBSET		= 150;
		static const int _CARLOW2CON_SUBSET		= 151;
		static const int _CARHIGH1CON_SUBSET	= 152;
		static const int _CARHIGH2CON_SUBSET	= 153;

		// Views
		static const int _CPU_VIEW				= 0;

		/** The constructor receives the configuration type. */
		Memory (const MCHEmul::Memory::Content& cnt,
			const std::string& lang = MCHEmul::_DEFAULTLANGUAGE);

		/** To get/set the configuration type. */
		const MCHEmul::UByte& configuration () const
							{ return (_configuration); }
		/** It can be overloaded for different types of memory. \n
			There is implementation by default. */
		virtual void setConfiguration (const MCHEmul::UByte& cfg);

		/** To activate the right subsets in the CPU view. */
		virtual bool initialize () override;

		protected:
		virtual MCHEmul::Stack* lookForStack () override
							{ return (dynamic_cast <MCHEmul::Stack*> (subset (_STACK_SUBSET))); }
		virtual MCHEmul::MemoryView* lookForCPUView () override
							{ return (view (_CPU_VIEW)); }

		/** Just to activate or desactivate the ROM. \n
			TED chip might need this methods if it has been configured to always
			look for char info in the ROM. */
		bool kernelROMActive () const
							{ return (_kernelROM1 -> active ()); }
		void activeKernelROM (bool a) // This method 
							{ _kernelROM1 -> setActive (a); }

		static MCHEmul::Memory::Content standardMemoryContent ();

		protected:
		MCHEmul::UByte _configuration;

		// Implementation
		MCHEmul::PhysicalStorageSubset* _RAM1;
		MCHEmul::PhysicalStorageSubset* _RAM2;
		MCHEmul::PhysicalStorageSubset* _basicROM;
		MCHEmul::PhysicalStorageSubset* _RAM3;
		MCHEmul::PhysicalStorageSubset* _carlow1;
		MCHEmul::PhysicalStorageSubset* _carlow2;
		MCHEmul::PhysicalStorageSubset* _kernelROM1;
		MCHEmul::PhysicalStorageSubset* _kernelROM2;
		MCHEmul::PhysicalStorageSubset* _RAM4;
		MCHEmul::PhysicalStorageSubset* _carhigh1;
		MCHEmul::PhysicalStorageSubset* _carhigh2;
		MCHEmul::PhysicalStorageSubset* _kernelROM3;
		MCHEmul::PhysicalStorageSubset* _RAM5;
	};

	/** The memory for the C16/116. */
	class C16_116Memory final : public Memory
	{
		public:
		// Subsets...
		static const int _IOACIARAM_SUBSET		= 200;
		static const int _IO6529B1RAM_SUBSET	= 201;

		C16_116Memory (const std::string& lang = MCHEmul::_DEFAULTLANGUAGE);

		private:
		static MCHEmul::Memory::Content standardMemoryContent ();
	};

	/** And the one for the powerful machine C264. */
	class CPlus4Memory final : public Memory
	{
		public:
		// More physical storage with a predefined code...
		static const int _FUNCTIONLOW			= 3;
		static const int _FUNCTIONHIGH			= 4;

		// The correspondant subsets...
		static const int _FUNCTIONLOW_SUBSET	= 300;
		static const int _FUNCTIONHIGH1_SUBSET	= 301;

		CPlus4Memory (const std::string& lang = MCHEmul::_DEFAULTLANGUAGE);

		virtual void setConfiguration (const MCHEmul::UByte& cfg) override;

		private:
		static MCHEmul::Memory::Content standardMemoryContent ();

		private:
		MCHEmul::PhysicalStorageSubset* _functionLow;
		MCHEmul::PhysicalStorageSubset* _functionHigh1;
	};
}

#endif
  
// End of the file
/*@}*/
