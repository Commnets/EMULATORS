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
#include <array>

namespace MSX
{
	class MSXComputer;

	/** This class represents the subslot subsregisters that can be read from the memory of the MSX. \n
		When at the page 3 (0xc000 - 0xffff) the slot connected has "nothing" there, 
		the position 0xffff has "subslots" that are connected to the different pages. \n
		This class leeps that information with a Singleton design pattern and ten can be accessed from anyplace. \n
		The object is instanciated in the Memory constructor and destroyed in the Memory destructor. */
	class SubSlotRegisters final
	{
		public:
		static SubSlotRegisters* instance (const std::array <bool, 4>& sL)
							{ return ((_instance == nullptr) 
								? _instance = new SubSlotRegisters (sL) : _instance); }
		/** To get the instance already created. \n
			It is assumed that the instance has been already created. */
		static SubSlotRegisters * instance ()
							{ return (_instance); }
		static void destroyInstance ()
							{ delete (_instance); _instance = nullptr; }

		inline void setSubSlotRegister (size_t nR, const MCHEmul::UByte& v);
		/** When the slot is expanded, the value returned is always the complement of the value kept. \n
			This is the way that the ROM will know whether the slot is or not expanded. \n
			In other case, the value returned is just the value kept. */
		MCHEmul::UByte subSlotRegister (size_t nR) const
							{ return (_slotsExpanded [nR] 
								? ~_subSlotRegister [nR] : _subSlotRegister [nR]); }

		/** To know whether the subslots have changed or not. */
		bool changed () const
							{ return (_changed); }

		private:
		// Declared private to implement the Singleton design pattern...
		// Once created it is not possible to redefine the slots expanded...
		// ...it is a hard definition of the machine...
		SubSlotRegisters (const std::array <bool, 4>& sL)
			: _slotsExpanded (sL),
			  _subSlotRegister { MCHEmul::UByte::_0, MCHEmul::UByte::_0, 
								 MCHEmul::UByte::_0, MCHEmul::UByte::_0 },
			  _changed (false)
							{ }

		private:
		/** A reference to the instance created. */
		static SubSlotRegisters* _instance;
		/** The slots that are expanded. */
		std::array <bool, 4> _slotsExpanded;
		/** The value of the subSlot registers. \n
			If the slot were not expanded, the value of the registers wouldn't have any effect. */
		MCHEmul::UByte _subSlotRegister [4]; // 1 per slot...
		
		// Implementation
		/** When the subslot register is changed. */
		MCHEmul::OBool _changed;
	};

	// ---
	inline void SubSlotRegisters::setSubSlotRegister (size_t nR, const MCHEmul::UByte& v)
	{
		// Keep the previous value...
		MCHEmul::UByte oV = _subSlotRegister [nR];
		// ...then the value is changed...
		_subSlotRegister [nR] = v;
		// ...but only if the slot is expanded the change will have effect in the configuration...
		// The changed in analyzed in the specificCyle of the Computer (@see MSXComputer class)...
		if (_slotsExpanded [nR])
			_changed = (v != oV);
	}

	/** Any RAM memory setting the last page (0xc000 - 0xffff),
		has to manage carefully the position 0xffff because it is special. \n
		This class is used to represent that last page of RAM memory. */
	class LastPagePhysicalStorageSubset : public MCHEmul::Stack
	{
		public:
		LastPagePhysicalStorageSubset (int id, 
				MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& iA, size_t s, 
				const Configuration& cfg = MCHEmul::Stack::Configuration ());

		protected:
		virtual void setValue (size_t nB, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t nB) const override;

		protected:
		unsigned char _slotNumber;
		// Implementation
		mutable MCHEmul::UByte _lastValueRead;
	};

	/** Create a special empty phisical storage that can work as a stack. \n
		This makes no sense from a theoretical point of view, but in a MSX the stack could be located in that places. \n
		This class is used to repesent a part of the memory (usually of 0x4000 bytes long or a multiple) that has nothing. \n
		There must be one and only one of this (or someone inheriting from this) present at every page of memory. \n
		This situation is verified at construction time one the element _memoryElements is created. \n
		The id received as parameter is very very special. \n
		Has to be built using the method "firstIdMemoryElementFreeSlotSubSlot" that creates the id from the slot and subslot 
		which the memory element belongs to. */
	class EmptyPhysicalStorageSubset final : public MCHEmul::Stack
	{
		public:
		EmptyPhysicalStorageSubset (int id, const MCHEmul::UByte& fV, 
				MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& iA, size_t s, 
				const Configuration& cfg = MCHEmul::Stack::Configuration ());

		private:
		virtual void setValue (size_t nB, const MCHEmul::UByte& v) override
							{  } // Does nothing...
		virtual const MCHEmul::UByte& readValue (size_t nB) const override
							{ return (_fixValue); } // ...and always returns the same value...

		private:
		MCHEmul::UByte _fixValue;
	};

	/** This class is simular to the previous one but representing the last accesible part of the memory. \n
		This is special to be able to read the position 0xffff and other ones special when nothing is connected. */
	class EmptyPhysicalStorageLastPageSubset final : public LastPagePhysicalStorageSubset
	{
		public:
		EmptyPhysicalStorageLastPageSubset (int id, const MCHEmul::UByte& fV,
				MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& iA, size_t s, 
				const Configuration& cfg = MCHEmul::Stack::Configuration ());

		private:
		virtual const MCHEmul::UByte& readValue (size_t nB) const override;

		private:
		const MCHEmul::UByte _fixValue;
	};

	/** The memory for the MSX. \n
		The memory is made up of elements (RAM, cartridge, Disks,...) connected in 4 slots and 4 subslots possible each. \n
		Those elements will be represented in the memory by a memory element that can be either RAM, ROM or nothing. \n
		In other side, the memory accesible from the Z80 (64k) is splitted in 4 pages of 16k bytes each. \n
		In each of these pages only one slot/subslot can be active at the same time,
		an only one element can be active at that block of positions (page) simultaneously. \n
		If there were not anything connected, the memory at that page would be empty. \n
		The port 0xa8 (@see PPIPortManager class) is sused to select the active slot per block of memory. \n
		In every slot, the position 0xffff is presented from the page 3 of the slot to indicate 
		the active subslots per page of memoty when the slot has a expander. \n
		e.g. 0xffff will have 0b00000001 to indicate that the subslot 0 is active for pages 3,2 and 1 and the subslot 1 is active for page 0. \n
		Reading that position returns the value inverted when written. So reading it when there is no expansion will return 0x00. */
	class Memory final : public MCHEmul::Memory
	{
		public:
		struct SlotSublotActive { unsigned char _slot, _subSlot; };

		// ID used to refer the different positions in the memory...
		// The physycal elements...
		static const int _ROM_SET = 100;				// Initially there is only ROM in the slot 0 page 0...
		static const int _RAM_SET = 110;				// 110, 111, 112, 113,....125 (16) The RAM of the 16 slots/subslots possible = 16 * 64k = 1M
		// The subsets of the memory...
		// The id determines in which slot/subslot it belongs to...
		// For every slot/subslot there could be up to 10 different elements connected...
		// e.g. 100, 110, 120, 130 will be ids for slot 0
		// e.g. 200, 210, 220, 230 will be ids for slot 1...and so on and so forth...
		// The detail of the slot/sublot is given by the first two digits of the id...
		// so 100 is slot 0, subslot 0, 110 is slot 0, subslot 1...and so on and so forth...
		// The detault element of the page is given by the last digit of the id...
		// so 100 is slot 0, subslot 0, page 0, 101 is slot 0, subslot 0, page 1...and so on and so forth...
		// If there would be more than one element connected in the same slot/subslot/page,
		// The multiple of 1000 is used, so 1100 is the first element in the the slot 0, subslot 0, page 0,
		// and 2100 is the second element in the slot 0, subslot 0, page 0, and so on and so forth...
		// There can be a maxuim of two elements in the same slot/subslot/page, and one of them have to be a empty memory piece!...
		static const int _SLOT0_SUBSET = 100;					// Slot 0
		static const int _SLOTSUBSLOTBASE_SUBSET = 100;			// The base id for the slot/subslot (used in calculus)
		static const int _ROMBIOS_SUBSET = 100;					// Slot 0, Subslot 0, Page 0 & 1. Minimum configuration...
		static const int _ERAM16KSLOT0SUBSLOT0_SUBSET = 102;	// Slot 0, Subslot 0, Page 2. Minimum configuration...
		static const int _RAM16KSLOT0SUBSLOT0_SUBSET = 103;		// Slot 0, Subslot 0, Page 3. Minimum configuration...
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

		/** Given a memory element id, to get the slot, subslot and page of a element id. 
			This system is based on maintining a rigorous way of naming the memory elements. */
		static inline void getSlotSubSlotAndPageForMemoryElement 
			(int id, unsigned char& slot, unsigned char& sslot, unsigned char& page);

		/** Creates the memory based on the model, the configuartion and the language. 
			The language is used mainly to load the right type of ROM. \n
			Creates the structure to keep the value of the subslots. */
		Memory (MSXModel* m, unsigned int cfg, const std::string& lang);

		/** Destroy the structure creates to maintain the value of the subSlot registers. */
		~Memory ();

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
		/** To get the id base for a memory element connected in a slot and in a subslot. */
		int idMemoryElementBaseSlotSubSlot (unsigned char slot, unsigned char sslot) const
							{ return (_SLOTSUBSLOTBASE_SUBSET + (slot * 100) + (sslot * 10)); }
		/** To get the first memory id free in a slot and subslot to define and element. */
		inline int firstIdMemoryElementFreeSlotSubSlot (unsigned char slot, unsigned char sslot) const;

		// To desactivate elements connected in the different slots and subslots per page.
		// These methods are used in running, so no boundary check is done.
		/** Desactivate all memory elements connected in a page of a subslot of a slot. \n
			the parameter std indicates that either the EMPTY RAMs or ROM or 16k basic system 
			are available in slot, subslot 0, pages 0,1 & 3 when everything else is desactivated there. */
		inline void desactivateMemoryElementsSubSlotAndPage 
			(unsigned char slot, unsigned char sslot, unsigned char page, bool std = false);
		/** Desactivate all memory elements in every page of a subslot of a slot,
			following the same rules described desactivateMemoryElementsSubSlotAndBank. */
		void desactivateMemoryElementsSubSlot (unsigned char slot, unsigned char sslot, bool std = false)
							{ for (unsigned char i = 0; i < 4; 
								desactivateMemoryElementsSubSlotAndPage (slot, sslot, i++, std)); }
		/** Desactivate all memory elements in every subslot and page of a slot,
			following the same rules described in desactivateMemoryElementsSubSlotAndBank. */
		void desactivateMemoryElementsSlot (unsigned char slot, bool std = false)
							{ for (unsigned char i = 0; i < 4; 
								desactivateMemoryElementsSubSlot (slot, i++, std)); }
		/** Desactivate all elements in all slots and subslots
			following the same rules described int desactivateMemoryElementsSubSlotAndBank. */
		void desactivateAllMemoryElements (bool std = false)
							{ for (unsigned char i = 0; i < 4; 
								desactivateMemoryElementsSlot (i++, std)); }

		// To connect memory elements in the MSX...
		/** To connect a memory element given by its id, only if it exists.
			Desactivate all other elements connected in the same slot, subslot and page, if any. */
		inline void connectMemoryElement (int id);
		/** To connect a set of elements in the MSX memory structure,
			following the same rules described above. */
		void connectMemoryElements (const std::vector <int>& ids)
							{ for (const auto& i : ids) connectMemoryElement (i); }

		/** To get the memory configuration. 
			it can get only as a const, that can not be changed... */
		const SlotSublotActive* const slotSubSlotsActive () const
							{ return (_slotSubSlotActive); }

		// To change the memory configuration...
		/** Active the memory element is a page, slot and sublot.
			Because there can be only a maximum of two elements in that situation and 
			only one of them can be different than empty, activates it if that situation exists. \n
			The method returns a reference to the element returned. */
		MCHEmul::PhysicalStorageSubset* activeMemoryElementInSlotSubSlotAndPage 
			(unsigned char slot, unsigned char sslot, unsigned char page);
		/** Change the slot active per page of memory. \n 
			The subslot finally selected (expanded) will depend on the last memory structure active. 
			That memory structure is managed acting over direction $ffff of every slot (subslot register). 
			@see in the definition of the class. */
		void activateSlotsPerPage (unsigned char spage0, unsigned char spage1, unsigned char spage2, unsigned spage3);
		/** To reactivate the slots per page.
			This is used when the subslots configuration changes, 
			and then it is needed to reassign that configuration in the memory. */
		void reactivateSlotsPerPage ()
							{ activateSlotsPerPage 
								(_slotSubSlotActive [0]._slot, // The ones that already active...
								 _slotSubSlotActive [1]._slot,
								 _slotSubSlotActive [2]._slot, 
								 _slotSubSlotActive [3]._slot); }

		/** Change the stack subset. */
		void setStackSubset (int id)
							{ resetStack (); _STACK_SUBSET = id; }

		/** To activate the right subsets in the CPU view. */
		virtual bool initialize () override;

		private:
		virtual MCHEmul::Stack* lookForStack () override
							{ return (dynamic_cast <MCHEmul::Stack*> (subset (_STACK_SUBSET))); }
		virtual MCHEmul::MemoryView* lookForCPUView () override
							{ return (view (_CPU_VIEW)); }

		// Implementation
		/** To create the internal _memoryElements variable. 
			There must be only one valid memory element (different than EmptyPhysicalStorage) per page slot and subslot.
			If the result follows this rule, the method returns true, and false in other circunstance. */
		bool createMemoryElementsEntity ();

		private:
		MSXModel* _model;
		unsigned int _configuration;

		// Implementation
		/** A reference to the basic elements. */
		MCHEmul::PhysicalStorageSubset* _ROM;
		MCHEmul::PhysicalStorageSubset* _BASICRAM;
		MCHEmul::PhysicalStorageSubset* _EMPTYBASICRAM;
		/** The list of memory elements that there are in every slot/subslot/page. \n
			This class is not the owner of the elements in this cube. \n
			The variable is filled up at construction time. */
		MCHEmul::PhysicalStorageSubsetsList _memoryElements [4][4][4];
		/** The id of the subset used for the stack... 
			that will depend on the configuration! */
		int _STACK_SUBSET;
		/** To indicate what slot and subslot is active per page. 
			e.g. _slotSubSlotActive [0] will keep the slot/subslot active in the page 0 ($0000 - $40000). 
			At construction time, all pages pointed out to slot and subslot 0. */
		SlotSublotActive _slotSubSlotActive [4];
	};

	// ---
	inline void Memory::getSlotSubSlotAndPageForMemoryElement 
		(int id, unsigned char& slot, unsigned char& sslot, unsigned char& page)
	{
		unsigned char nE = (unsigned char) (id / 1000); // The number of elements in the slot/subslot/page...
		// This number is not used for anything anymore in this routine, it just for information!
		id = id % 1000; // The id of the element in the slot/subslot/page...
		slot = (unsigned char) (id / 100); slot--;
		assert (slot >= 0 && slot < 4);		// Just in case...
		sslot = (unsigned char) ((id % 100) / 10);
		assert (sslot >= 0 && sslot < 4);	// Just in case...
		page = (unsigned char) (id % 10);
		assert (page >= 0 && page < 4);		// Just in case...
		// There can be many elements in the same slot/subslot/page...
	}

	// ---
	inline int Memory::firstIdMemoryElementFreeSlotSubSlot (unsigned char slot, unsigned char sslot) const
	{
		bool found = false;
		int result = idMemoryElementBaseSlotSubSlot (slot, sslot);
		for (size_t i = 0; i < 4; i++)
		{
			for (const auto& j : _memoryElements [slot][sslot][i])
			{ 
				found = true; // There is something...
				if (j -> id () > result)
					result = j -> id ();
			}
		}

		return (found ? result + 1 : result);
	}

	// ---
	inline void Memory::desactivateMemoryElementsSubSlotAndPage 
		(unsigned char slot, unsigned char sslot, unsigned char page, bool std)
	{
		if (slot == 0 && sslot == 0)
		{
			for (const auto& i : _memoryElements [slot][sslot][page])
				{ i -> setActive (false); i -> setActiveForReading (false); }
			if (std)
			{
				if (page == 0 || page == 1)	
					{ _ROM -> setActive (true); _ROM -> setActiveForReading (true); }
				// The RAM can cary...the onky fixed point is always the ROM...
			}
		}
		else
		{
			for (const auto& i : _memoryElements [slot][sslot][page])
			{ 
				bool a = std && // Activate the basic element when it is selected!
					(dynamic_cast <MSX::EmptyPhysicalStorageSubset*> (i) != nullptr &&
					 dynamic_cast <MSX::EmptyPhysicalStorageLastPageSubset*> (i) != nullptr);
				
				i -> setActive (a); 
				i -> setActiveForReading (a);
			}
		}
	}

	// ---
	inline void Memory::connectMemoryElement (int id)
	{
		// First of all, does the element exist?
		MCHEmul::PhysicalStorageSubset* elemt = subset (id);
		if (elemt == nullptr)
			return; // It it doesn't, do nothing...

		// An elements can be live in many pages attending to the size of the element...
		// However the element is connected in only one page at the same time...
		unsigned char slot, sslot, page;
		getSlotSubSlotAndPageForMemoryElement (id, slot, sslot, page);

		// Desactivate all other elements connected in the same slot, subslot and page, if any...
		desactivateMemoryElementsSubSlotAndPage (slot, sslot, page);

		// ...and finally connected the element...
		elemt -> setActive (true);
		elemt -> setActiveForReading (true);
	}
}

#endif

// End of the file
/*@}*/
