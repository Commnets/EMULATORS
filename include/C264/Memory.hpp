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

#include <CORE/incs.hpp>

namespace C264
{
	/** When a C264 series computer is not expanded, there are several memory zones not connected.
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

	/** The memory itself for the C64 Series... */
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
		static const int _KERNELROM_SUBSET		= 112;
		static const int _KERNELRAM_SUBSET		= 113;
		// TODO

		// Views
		static const int _CPU_VIEW				= 0;

		/** The constructor receives the type of machine the memory is for. */
		Memory (unsigned int mt, 
			const std::string& lang = MCHEmul::_DEFAULTLANGUAGE);

		/** To get/set the machine type. */
		unsigned int machineType () const
							{ return (_machineType); }
		void setMachineType (unsigned int mT);

		/** To activate the right subsets in the CPU view. */
		virtual bool initialize () override;

		private:
		virtual MCHEmul::Stack* lookForStack () override
							{ return (dynamic_cast <MCHEmul::Stack*> (subset (_STACK_SUBSET))); }
		virtual MCHEmul::MemoryView* lookForCPUView () override
							{ return (view (_CPU_VIEW)); }

		static MCHEmul::Memory::Content standardMemoryContent ();

		private:
		unsigned int _machineType;

		// Implementation
		MCHEmul::PhysicalStorageSubset* _basicROM;
		MCHEmul::PhysicalStorageSubset* _basicRAM;
		MCHEmul::PhysicalStorageSubset* _kernelROM;
		MCHEmul::PhysicalStorageSubset* _kernelRAM;
		// TODO
	};
}

#endif
  
// End of the file
/*@}*/
