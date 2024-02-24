/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: Memory.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 04/02/2024 \n
 *	Description: C264 Memory.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_MEMORY__
#define __C264_MEMORY__

#include <COMMODORE/incs.hpp>

namespace C264
{
	/** When a C264 series computer is not expanded (usually come configurations of C16/16), 
		there are several memory zones not connected.
		That zones, doesn't respond to poke and always return the same value when peeking 
		(at least it is as anothe emulators in the market now behave)
		This is the way (i guess) C264 non expanded series computer determines how much free memory the system has.
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

	/** The memory itself for the C264 Series... 
		There will be some classes inheriting this, 
		as the memory of the different machines is not the same, 
		because they don't event have the same chips attached!.  */
	class Memory : public MCHEmul::Memory
	{
		public:
		// Phisical Storages
		static const int _RAM					= 0;
		static const int _BASICROM				= 1;
		static const int _KERNELROM				= 2;

		// Subsets
		static const int _PAGEZERO_SUBSET		= 100;
		static const int _STACK_SUBSET			= 101;
		static const int _RAM1_SUBSET			= 102;
		static const int _RAM2_SUBSET			= 103;
		static const int _RAM3_SUBSET			= 104;
		static const int _BASICROM_SUBSET		= 105;
		static const int _RAM4_SUBSET			= 106;
		static const int _KERNELROM1_SUBSET		= 107;
		static const int _IORAM0_SUBSET			= 108;
		static const int _IORAM1_SUBSET			= 109;
		static const int _IORAM2_SUBSET			= 110;
		static const int _IORAM3_SUBSET			= 111;
		static const int _IORAM4_SUBSET			= 112;
		static const int _IORAM5_SUBSET			= 113;
		static const int _RAM5_SUBSET			= 114;
		static const int _KERNELROM2_SUBSET		= 115;

		// Views
		static const int _CPU_VIEW				= 0;

		/** The constructor receives the configuration type. */
		Memory (unsigned int cfg, const MCHEmul::Memory::Content& cnt,
			const std::string& lang = MCHEmul::_DEFAULTLANGUAGE);

		/** To get/set the configuration type. */
		unsigned int configuration () const
							{ return (_configuration); }
		/** It can be overloaded for different types of memory. */
		virtual void setConfiguration (unsigned int cfg) = 0;

		/** To activate the right subsets in the CPU view. */
		virtual bool initialize () override;

		protected:
		virtual MCHEmul::Stack* lookForStack () override
							{ return (dynamic_cast <MCHEmul::Stack*> (subset (_STACK_SUBSET))); }
		virtual MCHEmul::MemoryView* lookForCPUView () override
							{ return (view (_CPU_VIEW)); }

		static MCHEmul::Memory::Content standardMemoryContent ();

		protected:
		unsigned int _configuration;

		// Implementation
		MCHEmul::PhysicalStorageSubset* _RAM1;
		MCHEmul::PhysicalStorageSubset* _RAM2;
		MCHEmul::PhysicalStorageSubset* _RAM3;
		MCHEmul::PhysicalStorageSubset* _RAM4;
		MCHEmul::PhysicalStorageSubset* _RAM5;
		MCHEmul::PhysicalStorageSubset* _basicROM;
		MCHEmul::PhysicalStorageSubset* _kernelROM1;
		MCHEmul::PhysicalStorageSubset* _kernelROM2;
	};

	/** The memory for the C16/116. */
	class C16_116Memory final : public Memory
	{
		public:
		// Subsets...
		static const int _IOACIARAM_SUBSET		= 200;
		static const int _IO6529B1RAM_SUBSET	= 201;

		C16_116Memory (unsigned int cfg,
			const std::string& lang = MCHEmul::_DEFAULTLANGUAGE);

		virtual void setConfiguration (unsigned int cfg) override;

		private:
		static MCHEmul::Memory::Content standardMemoryContent ();
	};

	/** And the one for the powerful machine C264. */
	class CPlus4Memory final : public Memory
	{
		public:
		CPlus4Memory (unsigned int cfg,
			const std::string& lang = MCHEmul::_DEFAULTLANGUAGE);

		virtual void setConfiguration (unsigned int cfg) override;

		private:
		static MCHEmul::Memory::Content standardMemoryContent ();

		private:
		MCHEmul::PhysicalStorageSubset* _code1;
		MCHEmul::PhysicalStorageSubset* _code2;
	};
}

#endif
  
// End of the file
/*@}*/
