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
#include <MSX/Model.hpp>

namespace MSX
{
	class MSXComputer;

	/** The memory itself for the MSX. \n
		The memory is made up of elements (RAM, cartridge, Disks,...) connected in 4 slots and 4 subslots possible. \n
		In other side, the memory accesible from the Z80 (64k) is splitted in 4 banks of 16k bytes each. \n
		In each of these banks only one slot/subslot can be active at the same time,
		an only one element cab ne active at that position simultaneously. \n
		If there were not anything connected, the memory at that bank would be empty. */
	class Memory final : public MCHEmul::Memory
	{
		public:
		// ID used to refer the different positions in the memory...
		// The physycal elements...
		static const int _ROM_SET = 100;				// Initially there is only ROM in the slot 0 bank 0...
		static const int _RAM_SET = 110;				// 110, 111, 112, 113,....125 (16) The RAM of the 16 slots/subslots possible = 16 * 64k = 1M
		// The subsets of the memory...
		// The id determines in which slot/subslot it belongs to...
		// For every slot/subslot there could be up to 10 different elements connected...
		// e.g. 100, 110, 120, 130 will be ids for slot 0
		// e.g. 200, 210, 220, 230 will be ids for slot 1...and so on and so forth...
		static const int _SLOT0_SUBSET = 100;					// Slot 0
		static const int _SLOTSUBSLOTBASE_SUBSET = 100;			// The base id for the slot/subslot (used in calculus)
		static const int _ROMBIOS_SUBSET = 100;					// Slot 0, Subslot 0, Bank 0 & 1. Minimum configuration...
		static const int _ERAM16KSLOT0SUBSLOT0_SUBSET = 102;	// Slot 0, Subslot 0, Bank 2. Minimum configuration...
		static const int _RAM16KSLOT0SUBSLOT0_SUBSET = 103;		// Slot 0, Subslot 0, Bank 3. Minimum configuration...
		static const int _SLOT0BASE_SUBSET = 100;				// Slot 0
		static const int _SLOT0SUBSLOT0BASE_SUBSET = 100;		// Slot 0, Subslot 0
		static const int _SLOT0SUBSLOT1BASE_SUBSET = 110;		// Slot 0, Subslot 1
		static const int _SLOT0SUBSLOT2BASE_SUBSET = 120;		// Slot 0, Subslot 2
		static const int _SLOT0SUBSLOT3BASE_SUBSET = 130;		// Slot 0, Subslot 3
		static const int _SLOT1BASE_SUBSET = 200;				// Slot 1
		static const int _SLOT1SUBSLOT0BASE_SUBSET = 200;		// Slot 1, Subslot 0
		static const int _SLOT1SUBSLOT1BASE_SUBSET = 210;		// Slot 1, Subslot 1
		static const int _SLOT1SUBSLOT2BASE_SUBSET = 220;		// Slot 1, Subslot 2
		static const int _SLOT1SUBSLOT3BASE_SUBSET = 230;		// Slot 1, Subslot 3
		static const int _SLOT2BASE_SUBSET = 300;				// Slot 2
		static const int _SLOT2SUBSLOT0BASE_SUBSET = 300;		// Slot 2, Subslot 0
		static const int _SLOT2SUBSLOT1BASE_SUBSET = 310;		// Slot 2, Subslot 1
		static const int _SLOT2SUBSLOT2BASE_SUBSET = 320;		// Slot 2, Subslot 2
		static const int _SLOT2SUBSLOT3BASE_SUBSET = 330;		// Slot 2, Subslot 3
		static const int _SLOT3BASE_SUBSET = 400;				// Slot 3
		static const int _SLOT3SUBSLOT0BASE_SUBSET = 400;		// Slot 3, Subslot 0
		static const int _SLOT3SUBSLOT1BASE_SUBSET = 410;		// Slot 3, Subslot 1
		static const int _SLOT3SUBSLOT2BASE_SUBSET = 420;		// Slot 3, Subslot 2
		static const int _SLOT3SUBSLOT3BASE_SUBSET = 430;		// Slot 3, Subslot 3
		// The views of the memory...
		static const int _CPU_VIEW = 0;	// The view from the CPU...

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
		void setConfiguration (unsigned int cfg)
							{ _model -> configureMemory (this, cfg); }

		// Knowing and managing the id of the elements connected in the memory...
		/** To get the id base for a element connected in a slot and in a subslot. */
		int idElementBaseSlotSubslot (unsigned char slot, unsigned char sslot) const
							{ return (_SLOTSUBSLOTBASE_SUBSET + (slot * 100) + (sslot * 10)); }
		/** To get the first id free in a slot and subslot to define and element. */
		inline int firstIdElementFreeSlotSubSlot (unsigned char slot, unsigned char sslot) const;

		// To desactivate elements connected in the slots and subslots per bank.
		// These methods are used in running, so no boundary check is done.
		/** Desactivate all elements connected in a bank of a subslot of a slot. \n
			the parameter std indicates that either 
			the EMPTY RAMs or ROM or 16k basic system are available in slot, subslot 0, bank 0,1 & 3. */
		inline void desactivateElementsSubSlotAndBank 
			(unsigned char slot, unsigned char sslot, unsigned char bank, bool std = false);
		/** Desactivate all elements in every bank of a subslot of a slot. */
		void desactivateElementsSubSlot (unsigned char slot, unsigned char sslot, bool std = false)
							{ for (unsigned char i = 0; i < 4; 
								desactivateElementsSubSlotAndBank (slot, sslot, i++, std)); }
		/** Desactivate all elements in every subslot and bank of a slot. */
		void desactivateElementsSlot (unsigned char slot, bool std = false)
							{ for (unsigned char i = 0; i < 4; 
								desactivateElementsSubSlot (slot, i++, std)); }
		/** Desactivate all elements in all slots and subslots. */
		void desactivateAllElements (bool std = false)
							{ for (unsigned char i = 0; i < 4; 
								desactivateElementsSlot (i++, std)); }

		// To connect elements in the memory...
		/** To connect and element given by its id, only if it exists.
			Desactivate all other elements connected in the same slot, subslot and bank, if any. */
		inline void connectElement (int id);
		/** To connect a copuple of elements. */
		void connectElements (const std::vector <int>& ids)
							{ for (const auto& i : ids) connectElement (i); }

		/** Change the stack subset. */
		void setStackSubset (int id)
							{ _STACK_SUBSET = id; }

		/** To activate the right subsets in the CPU view. */
		virtual bool initialize () override;

		private:
		virtual MCHEmul::Stack* lookForStack () override
							{ return (dynamic_cast <MCHEmul::Stack*> (subset (_STACK_SUBSET))); }
		virtual MCHEmul::MemoryView* lookForCPUView () override
							{ return (view (_CPU_VIEW)); }

		// Implementation
		/** To get the slot, subslot and bank of a element id. */
		inline void getSlotSubSlotAndBank (int id, unsigned char& slot, unsigned char& sslot, unsigned char& bank) const;
		/** To create the internal _bank variable. */
		void createBanks ();

		private:
		MSXModel* _model;
		unsigned int _configuration;

		// Implementation
		/** A reference to the basic elements. */
		MCHEmul::PhysicalStorageSubset* _ROM;
		MCHEmul::PhysicalStorageSubset* _BASICRAM;
		MCHEmul::PhysicalStorageSubset* _EMPTYBASICRAM;
		/** The list of subsets that there is in every slot/subslot. \n
			This class is not the woner of the elements in this cube. \n
			The variable is filled up at construction time. */
		MCHEmul::PhysicalStorageSubsetsList _bank [4][4][4];
		/** The id of the subset used for the stack... 
			that will depend on the configuration! */
		int _STACK_SUBSET;
	};

	// ---
	inline int Memory::firstIdElementFreeSlotSubSlot (unsigned char slot, unsigned char sslot) const
	{
		bool found = false;
		int result = idElementBaseSlotSubslot (slot, sslot);
		for (size_t i = 0; i < 4; i++)
		{
			for (const auto& j : _bank [slot][sslot][i])
			{ 
				found = true; // There is something...
				if (j -> id () > result)
					result = j -> id ();
			}
		}

		return (found ? result + 1 : result);
	}

	// ---
	inline void Memory::desactivateElementsSubSlotAndBank 
		(unsigned char slot, unsigned char sslot, unsigned char bank, bool std)
	{
		if (slot == 0 && sslot == 0)
		{
			for (const auto& i : _bank [slot][sslot][bank])
				{ i -> setActive (false); i -> setActiveForReading (false); }
			if (std)
			{
				if (bank == 0 || bank == 1)	{ _ROM -> setActive (true); _ROM -> setActiveForReading (true); }
				else if (bank == 2)			{ _EMPTYBASICRAM -> setActive (true); _EMPTYBASICRAM -> setActiveForReading (true); }
				else if (bank == 3)			{ _BASICRAM -> setActive (true); _BASICRAM -> setActiveForReading (true); }
			}
		}
		else
		{
			for (const auto& i : _bank [slot][sslot][bank])
			{ 
				bool a = std && // Activate the basic element when it is selected!
					(dynamic_cast <MCHEmul::EmptyPhysicalStorageSubset*> (i) != nullptr);
				
				i -> setActive (a); 
				i -> setActiveForReading (a);
			}
		}
	}

	// ---
	inline void Memory::connectElement (int id)
	{
		// First of all, does the element exist?
		MCHEmul::PhysicalStorageSubset* elemt = subset (id);
		if (elemt == nullptr)
			return; // It it doesn't, do nothing...

		// An elements can be live in many banks attending to the size of the element...
		// However the element is connected in only one bank at the same time...
		unsigned char slot, sslot, bank;
		getSlotSubSlotAndBank (id, slot, sslot, bank);

		// Desactivate all other elements connected in the same slot, subslot and bank, if any...
		desactivateElementsSubSlotAndBank (slot, sslot, bank);

		// ...and finally connected the element...
		elemt -> setActive (true);
		elemt -> setActiveForReading (true);
	}

	// ---
	inline void Memory::getSlotSubSlotAndBank (int id, unsigned char& slot, unsigned char& sslot, unsigned char& bank) const
	{
		slot = (unsigned char) (id / 100); slot--;
		assert (slot >= 0 && slot < 4);		// Just in case...
		sslot = (unsigned char) ((id % 100) / 10);
		assert (sslot >= 0 && sslot < 4);	// Just in case...
		bank = (unsigned char) (id % 10);
		assert (bank >= 0 && bank < 4);		// Just in case...
	}
}

#endif

// End of the file
/*@}*/
