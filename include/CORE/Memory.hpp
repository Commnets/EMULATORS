/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: Memory.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 07/04/2021 \n
 *	Description: The memory managed by the machine.
 *	Versions: 1.0 Initial.
 *			  2.0 Radical change to speed up accesses and to try to reflect 
 *				  better what the real memory structure is.	
 */

#ifndef __MCHEMUL_MEMORY__
#define __MCHEMUL_MEMORY__

#include <CORE/global.hpp>
#include <CORE/MBElement.hpp>
#include <CORE/Address.hpp>
#include <CORE/UByte.hpp>
#include <CORE/UBytes.hpp>
#include <CORE/DtMemoryBlock.hpp>
#include <CORE/DebugFile.hpp>

namespace MCHEmul
{
	class PhysicalStorageSubset;
	class MemoryView;
	class SetMemoryCommand;
	class Stack;

	/** Represents what phisically speaking the memoy is: \n
		Just a place to keep info. \n
		It can be either RAM or ROM depending whether is possible to modify data. \n
		The phisical storage can only be accesed through a subset, controlling the boundaries of the phisical storage. */
	class PhysicalStorage final
	{
		public:
		/** The default value which is used to initialize the memory, can be changed. */
		static UByte _DEFAULTVALUE;

		enum class Type 
		{ 
			_ROM, 
			_RAM 
		};

		PhysicalStorage () = delete;

		PhysicalStorage (int id, Type t, size_t s)
			: _id (id), _type (t), 
			  _data (s, UByte::_0)
							{ }

		int id () const
							{ return (_id); }
		Type type () const
							{ return (_type); }
		size_t size () const
							{ return (_data.size ()); }

		const std::vector <MCHEmul::UByte>& data () const
							{ return (_data); }
		
		bool canBeWriten (bool f = false /** force the writting. */) const
							{ return (_type == Type::_RAM || (_type == Type::_ROM && f)); }

		// This methods should be only invoked from PhisicalMemorySubset class
		// IMPORTANT NOTE: 
		// They go direclty to the memory and can be dangerous for the stability of the system!

		/** Managing the content of the memory. \n
			Take into account that whether it is a RAM or a ROM is not controlled here. \n
			It is something that has to be guarantteed by the subsets built on top of this. */
		// Take care using it direclty because the boundaries are not controlled!
		// It is not controlled either whether to write is not possible!. 
		// It is controlled only throught out the @see PhysicalStorageSubset class!
		void set (size_t pB, const UByte& d)
							{ _data [pB] = d; /** The very real write type access. */}
		const UByte& value (size_t pB) const
							{ return (_data [pB]); /** The very real read type access. */}
		UBytes values (size_t pB, size_t nB) const
							{ return (UBytes (bytes (pB, nB))); }
		void set (size_t pB, const UBytes& v)
							{ set (pB, v.bytes ()); }
		inline std::vector <UByte> bytes (size_t pB, size_t nB) const;
		void set (size_t pB, const std::vector <UByte>& v)
							{ for (size_t i = 0; i < v.size (); i++) set (pB + i, v [i]); }

		/** Load the info, as many bytes as possible, from a specific byte of the memory. \n
			If nothing is said, the info is load from the very first byte. \n
			If the info couldn't be loaded, because it were over the boundaries, 
			the method would return false. True in other circunstances. */
		bool loadInto (const std::string& fN, size_t p = 0);

		/** Save the a number of bytes "nB" of the memory into a file, string from a position p. \n
			If nothing is said the position choosen is the initial one. \n
			If the info couldn't be saved, the method would return false. True in other circunstances. */
		bool saveFrom (const std::string& fN, size_t nB, size_t p = 0) const;

		protected:
		const int _id = -1; // Modified at construction level
		const Type _type = Type::_RAM; // Modified also at construction level
		std::vector <MCHEmul::UByte> _data;
	};

	// ---
	inline std::vector <UByte> MCHEmul::PhysicalStorage::bytes (size_t pB, size_t nB) const
	{
		std::vector <UByte> result;
		for (size_t i = 0; i < nB; i++)
			result.emplace_back (value (pB + i));
		return (result);
	}

	/** To simplify the way a map of elements is managed. */
	using PhysicalStorages = std::map <int, PhysicalStorage*>;

	/** To DUMP the content (partially) of a subset of memory. */
	struct PhysicalStorageSubsetDUMP final
	{
		int _id;
		std::string _name;
		bool _RAM; // Or ROM?
		bool _active;
		bool _activeForReading;
		Address _from, _to;
		UBytes _bytes;

		InfoStructure getInfoStructure () const;
	};

	/** Represents a subset of the physical storage. \n
		Many subsets can be created over the same physical location. \n
		This class might send and receive events to be connected to other elements of the computer. */
	class PhysicalStorageSubset : public InfoClass, public Observer, public Notifier
	{
		public:
		friend MemoryView;
		friend SetMemoryCommand;

		static size_t _GETINFOBYTESLIMIT; // Bytes maximum to be printed out when getInfoStructure is invoked...

		/** It is guarantteed that the subset is always within the boundaries of the phisical storage behind. \n
			Otherwise the view will be have the same size than that. \n
			The reference to the phisical storage can't be null at all. \n 
			The subset is not the owner of the phisical storage. \n
			The physical storage can be either active or inactive. */
		PhysicalStorageSubset (int id, PhysicalStorage* pS, size_t pp /** link a phisical */, const Address& a, size_t s);

		PhysicalStorageSubset (const PhysicalStorageSubset&) = delete;

		PhysicalStorageSubset& operator = (const PhysicalStorageSubset&) = delete;

		virtual ~PhysicalStorageSubset () override
							{ /** Nothing to do by default. */ }

		PhysicalStorageSubset (PhysicalStorageSubset&&) = delete;

		PhysicalStorageSubset& operator = (PhysicalStorageSubset&&) = delete;

		/** Id for the view. 
			It is diiferent than the id of the phisical storage behind. */
		int id () const
							{ return (_id); }
		/** The name is optional. */
		const std::string name () const
							{ return (_name); }
		void setName (const std::string& n)
							{ _name = n; }
		const PhysicalStorage* physicalStorage () const
							{ return (_physicalStorage); }
		PhysicalStorage* physicalStorage ()
							{ return (_physicalStorage); }
		PhysicalStorage::Type type () const
							{ return (_physicalStorage -> type ()); }
		size_t initialPhysicalPosition () const
							{ return (_initialPhisicalPosition);  }
		const Address& initialAddress () const
							{ return (_initialAddress); }
		size_t size () const
							{ return (_size); }
		Address lastAddress () const
							{ return (initialAddress () + size () - 1); }
		
		bool active () const
							{ return (_active); }
		void setActive (bool a)
							{ _active = a; }
		bool activeForReading () const
							{ return (_activeForReading); }
		void setActiveForReading (bool aR)
							{ _activeForReading = aR; }

		bool canBeWriten (bool f) const
							{ return (_active && _physicalStorage -> canBeWriten (f)); }

		// The subset could be excluded from buffering the set commands
		// in case this action were active. If weren't this group of methods is useless...
		bool bufferMemorySetCommands () const
							{ return (_bufferMemorySetCommands); }
		void setBufferMemorySetCommands (bool bS)
							{ _bufferMemorySetCommands = bS; }

		/** The address may only be "in" when the subset is active. */
		bool isIn (const Address& a, int& dt) const
							{ return (_active && (a >= _initialAddress && (dt = _initialAddress.distanceWith (a)) < (int) _size)); }

		// Manages the memory...
		/** The internal method "setValue" is invoked when possible. \n
			When the address requested is not "in" the subset, nothing happens. */
		void set (const Address& a, const UByte& d, bool f = false);
		/** The internal method "readValue" is invoked when possible. \n
			When the address requested is not "in" the subset, _DEFUALTVALUE is returned. */
		const UByte& value (const Address& a) const;
		/** Sometimes is needed to read directly the value 
			not taken into account whether it is active for reading. */
		const UByte& valueDirect (const Address& a) const;
		UBytes values (const Address& a, size_t nB) const
							{ return (UBytes (bytes (a, nB))); }
		void set (const Address& a, const UBytes& v, bool f = false)
							{ set (a, v.bytes (), f); }
		/** When some of the requested position is out the boundaries, an empty map is returned. */
		std::vector <UByte> bytes (const Address& a, size_t nB) const;
		std::vector <UByte> bytes () const
							{ return (bytes (initialAddress (), size ())); }
		void set (const Address& a, const std::vector <UByte>& v, bool f = false);

		// To put a value/values in the memory...
		// They are like "set" (see above), but not taking into account buffering behaviour if any!
		void put (const Address& a, const UByte& d, bool f = false)
							{ int dt = 0; if (_physicalStorage -> canBeWriten (f) && isIn (a, dt)) setValue (dt, d); }
		void put (const Address& a, const UBytes& v, bool f = false)
							{ put (a, v.bytes (), f); }
		void put (const Address& a, const std::vector <UByte>& v, bool f = false);
		void put (const DataMemoryBlock& mb, bool f = false)
							{ put (mb.startAddress (), mb.bytes (), f); }
		void put (const DataMemoryBlocks& mb, bool f = false)
							{ for (const auto& i : mb) put (i, f); }

		// To fill up the memory with values...
		/** To fill the memory with value/values. */
		void fillWith (const UByte& b)
							{ for (size_t i = 0; i < _size; i++) setValue (i, b); }
		void fillWith (const Address& addr, const UByte& b, size_t nB);

		/** To get a DUMP. */
		PhysicalStorageSubsetDUMP dump (const Address& f, const Address& t) const;

		/** To init the memory. \n
			It might be overloaded. By default the "defaultData" value is assigned. \n
			Uses method "fixDefaultValues" to chage the info that is used to initilized the memory in further ocassions. \n
			It doesn't matter whether the subset is or not active. */
		virtual void initialize ()
							{ for (size_t i = 0; i < _size; i++) setValue (i, _defaultData [i]); }

		/** First "sA" bytes of the file defining the address where to load the bytes. \n
			It is also necessary to define whether that address is or not big endian. */
		bool load (const std::string& fN, size_t sA, bool bE = true);
		/** Everything the file fN to the memory position parameter. */
		bool loadInto (const std::string& fN, const Address& a)
							{ return ((a >= _initialAddress) 
								? _physicalStorage -> loadInto (fN, _initialAddress.distanceWith (a)) : false); }
		/** The address where to load things into is always the initial address of the memory. */
		bool loadInto (const std::string& fN) // So it invoked the previous one...
							{ return (loadInto (fN, initialAddress ())); }

		/** Save the content of full PhysycalStorageSubset into a file. 
			The first "sA" bytes will indicate the address of the first position in big endian of little endian format. */
		bool save (const std::string& fN, size_t sA, bool bE = true);
		bool saveFrom (const std::string& fN, size_t nB, const Address& a)
							{ return ((a >= _initialAddress && (a + nB) < lastAddress ()) 
								? _physicalStorage -> saveFrom (fN, _initialAddress.distanceWith (a), nB) : false); }

		/** To keep the values for further initialization. */
		void fixDefaultValues ()
							{ _defaultData = _physicalStorage -> values (_initialPhisicalPosition, _size).bytes (); }

		/**
		  *	The name of the fields are: \n
		  *	ID			= Attribute: Id of the memory. \n
		  * NAME		= Attribute: Name of the memory. \n
		  *	ADDRESS		= Attribute: Address where the storage starts. \n
		  *	SIZE		= Attribute: Size in bytes. \n
		  * ACTIVE		= Attribute: YES when access operations are possible and NO in other case. \n
		  * READ		= Attribute: YES when read operations are possible and NO in other case. \n
		  * BYTES		= Attribute: Info of the memory. It is limited by the variable _GETINFOBYTESLIMIT. \n
		  */
		virtual InfoStructure getInfoStructure () const override;

		/** Manages the deep debug file. \n
			Take care it can be set back to a nullptr. */
		bool deepDebugActive () const
							{ return (_deepDebugFile != nullptr && _deepDebugFile -> active ()); }
		void setDeepDebugFile (DebugFile* dF)
							{ _deepDebugFile = dF; }
		const DebugFile* deepDebugFile () const
							{ return (_deepDebugFile); }
		DebugFile* deepDebugFile ()
							{ return (_deepDebugFile); }

		protected:
		/** They could be overeloaded for specific subsets: Chips registers,... 
			By default, the instructions are transmitted to the physical storage.
			At this point it must be guaranteed that the byte requested exist in the phisical storage. 
			The number of register (relative) with in the phisical storage is received. \n
			The @see MemoryView can access directly to this method but it also guarantee that the position is within the limits. */
		virtual void setValue (size_t nB, const UByte& d)
							{ _physicalStorage -> set (nB + _initialPhisicalPosition, d); }
		virtual const UByte& readValue (size_t nB) const
							{ return (_physicalStorage -> value (nB + _initialPhisicalPosition)); }

		protected:
		const int _id = -1; 
		/** It is optional. */
		std::string _name;
		PhysicalStorage* _physicalStorage;
		/** The position on the phisical storage, this subset is connected to. */
		size_t _initialPhisicalPosition;
		/** Which address does the initial position defined above represent ? */
		Address _initialAddress;
		/** How many bytes of info does this subset represent? */
		size_t _size;

		// To manage the debug info...
		DebugFile* _deepDebugFile;

		// Implementation
		bool _active;
		bool _activeForReading;
		bool _bufferMemorySetCommands;
		std::vector <MCHEmul::UByte> _defaultData;
	};

	/** To simplify the way a map of elements is managed. */
	using PhysicalStorageSubsets = std::map <int, PhysicalStorageSubset*>;
	using PhysicalStorageSubsetsList = std::vector <PhysicalStorageSubset*>;

	/** Empty physical subset. \n
		In some computers, there might be a part of the memory "not connected"
		that answers always with the same value and it is not affected by "set" command. */
	class EmptyPhysicalStorageSubset : public PhysicalStorageSubset
	{
		public:
		EmptyPhysicalStorageSubset (int id, const UByte& val, 
			PhysicalStorage* pS, size_t pp, const Address& a, size_t s)
			: PhysicalStorageSubset (id, pS, pp, a, s),
			  _fixValue (val)
							{ }

		const UByte& fixValue () const
							{ return (_fixValue); }

		protected:
		virtual void setValue (size_t nB, const UByte& d) override
							{ /** does nothing. */ }
		virtual const UByte& readValue (size_t nB) const override
							{ return (_fixValue); } // Always the same...

		protected:
		UByte _fixValue;
	};

	/** A physical subset that responds always random values. */
	class RandomPhysicalStorageSubset : public PhysicalStorageSubset
	{
		public:
		RandomPhysicalStorageSubset (int id,
			PhysicalStorage* pS, size_t pp, const Address& a, size_t s)
			: PhysicalStorageSubset (id, pS, pp, a, s),
			  _lastValueRead (MCHEmul::UByte::_0)
							{ }

		protected:
		virtual void setValue (size_t nB, const UByte& d) override
							{ /** does nothing. */ }
		virtual const UByte& readValue (size_t nB) const override
							{ return (_lastValueRead = UByte ((unsigned char) (std::rand () % 256))); } 

		private:
		mutable UByte _lastValueRead;
	};

	/** A physicial subset that is a mirror of a previous one, 
		but in another address! */
	class MirrorPhysicalStorageSubset : public PhysicalStorageSubset
	{
		public:
		MirrorPhysicalStorageSubset (int id, 
			PhysicalStorageSubset* pSS, const Address& a)
			: PhysicalStorageSubset (id, 
				pSS -> physicalStorage (), pSS -> initialPhysicalPosition (), a, pSS -> size ())
							{ assert (pSS != nullptr); } // maybe too late, but just in case...
	};

	/** A SetMemoryOrder is to buffer the execution of a "set" command. \n
		When under the class Configuration is configured to buffer the "set" commands,
		any time a "set" command is executed an object of this class is created and stored under the configuration unique object. */
	class SetMemoryCommand final
	{
		public:
		SetMemoryCommand (PhysicalStorageSubset* s, size_t p, const UByte& v)
			: _subset (s), _position (p), _value (v)
						{ assert (_subset != nullptr); }

		/** Execute finally the setValue command, but add info in the debug file
			if it was active. */
		void execute ();

		private:
		PhysicalStorageSubset* _subset;
		size_t _position;
		UByte _value;
	};

	/** To simplify the management of list of commands. */
	using SetMemoryCommands = std::vector <SetMemoryCommand>;

	/** To DUMP the content of a memory view */
	struct MemoryViewDUMP final
	{
		int _id; // of the view...
		std::vector <PhysicalStorageSubsetDUMP> _data;

		InfoStructure getInfoStructure () const;
	};

	/** A memory view represents a set of subsets over phisical storage (one or many). \n
		The subsets can combine either RAM or ROM access and can also overlap each other. */
	class MemoryView : public InfoClass
	{
		public:
		/** The memory view is not the owner of the subsets. */
		MemoryView (int id, const PhysicalStorageSubsets& ss);

		MemoryView (const MemoryView&) = delete;

		MemoryView& operator = (const MemoryView&) = delete;

		virtual ~MemoryView ()
							{ }

		MemoryView (MemoryView&&) = delete;

		MemoryView& operator = (MemoryView&&) = delete;

		int id () const
							{ return (_id); }
		size_t size () const
							{ return (_numPositions); }

		// To manage the subsets...
		const PhysicalStorageSubsets& subsets () const
							{ return (_subsets); }
		bool existsSubset (int id) const
							{ return (_subsets.find (id) != _subsets.end ()); }
		const PhysicalStorageSubset* subset (int id) const
							{ return ((const PhysicalStorageSubset*) (((MemoryView*) (this)) -> subset (id))); }
		PhysicalStorageSubset* subset (int id)
							{ PhysicalStorageSubsets::const_iterator i = _subsets.find (id); 
								return ((i != _subsets.end ()) ? (*i).second : nullptr); }
		/** Returns true when the element was added right. */
		bool addSubset (PhysicalStorageSubset* s);
		/** Returns true when all subsets have been sucessfully added. */
		bool addSubSets (const PhysicalStorageSubsets& ss)
							{ bool r = true; for (const auto& i : ss) r &= addSubset (i.second); return (r); }
		/** Returns true when the element was extracted. \n
			(NOTE) The element is only extracted from the set but not deleted. */
		bool removeSubSet (int id);
		/** Returns true when all have been sucessfully removed. */
		bool removeSubsets (const std::vector <int>& ss)
							{ bool r = true; for (auto i : ss) r &= removeSubSet (i); return (r); }
		
		/** Returns true when the address is in the physical storage.
			The attribute dt is filled up with the distance of the address to the initial position of the physical subset. */
		inline bool isIn (const Address& a, int& dt) const;

		/** Returns the address at in the middle of the physical subset. */
		Address middleMemoryAddress () const
							{ return (_minAddress + (_numPositions / 2)); }

		// Manage the memory...
		// Many methods cannot be inline because they use "setValue/readValue" (PhysicalStorageSubset) that are defined as virtual
		/** If there had been several subsets behind, the read/set operation would always happen 
			on the first readable/writtable subset possible. \n
			This applies to all methods below... */
		const UByte& value (const Address& a) const;
		void set (const Address& a, const UByte& d, bool f = false); 
		UBytes values (const Address& a, size_t nB) const
							{ return (UBytes (bytes (a, nB))); }
		void set (const Address& a, const UBytes& v, bool f = false)
							{ set (a, v.bytes (), f); }
		std::vector <UByte> bytes (const Address& a, size_t nB) const;
		void set (const Address& a, const std::vector <UByte>& v, bool f = false);

		// To put a value/values in the memory...
		// They are like "set" (see above), but not taking into account buffering behaviour if any!
		void put (const Address& a, const UByte& d, bool f = false); 
		void put (const Address& a, const UBytes& v, bool f = false)
							{ put (a, v.bytes (), f); }
		void put (const Address& a, const std::vector <UByte>& v, bool f = false);
		void put (const DataMemoryBlock& mb, bool f = false)
							{ put (mb.startAddress (), mb.bytes (), f); }
		void put (const DataMemoryBlocks& mb, bool f = false)
							{ for (const auto& i : mb) put (i, f); }

		// Fill up the memory...
		// They all use the put method of the class to avoid buffering behaviour if any!
		/** With a value. */
		void fillWith (const MCHEmul::UByte& b, bool f = false)
							{ for (size_t i = 0; i < _numPositions; i++) put (_minAddress + i, b, f); }
		/** Just a piece of the memory. */
		void fillWith (const MCHEmul::Address& addr, const MCHEmul::UByte& b, size_t nB, bool f = false)
							{ for (size_t i = 0; i < nB; i++) put (addr + i, b, f); }

		/** To get a DUMP from all memories actives at some positions. */
		MemoryViewDUMP dump (const Address& f, const Address& t) const;
		/** Dump the structure of the memory view, considering all locations (by default = true)
			and only the active ones (parameter = false). */
		MemoryViewDUMP dumpStructure (bool a = true);

		/** To init the memory view. 
			It might be overloaded. By default the "defaultData" value is assigned. */
		virtual void initialize () 
							{ for (const auto& i : _subsets) i.second -> initialize (); }

		/** Load the file info into the first subset holding the address parameter. */
		bool loadInto (const std::string& fN, const Address& a);

		/** Save "nB" bytes of memory of the first subset matching the address parameter. */
		bool saveFrom (const std::string& fN, size_t nB,const Address& a);

		/**
		  *	The name of the fields are: \n
		  *	STORAGES	= InfoStructure: Every storage belonging to the view (@see PhysicalStorageSubset).
		  */
		virtual InfoStructure getInfoStructure () const override;

		protected:
		// Implementation
		/** Used internally to convert the structure of the memory view into a "plain view"
			much quicker to be accessed from the CPU simulation. \n
			Affects the elements "_minAddress, _maxAddress, _numPositions and _memPositions. */
		void plainMemory ();

		protected:
		int _id;
		PhysicalStorageSubsets _subsets;

		// Implementation
		/** Structure just o speed up the access to a memory view. */
		struct MemoryPosition
		{
			MemoryPosition ()
				: _number (0), _storages ()
							{ }

			size_t _number; // The elements in the array...
			PhysicalStorageSubsetsList _storages; // The array with all memory subsets addressing that position...
		};

		using MemoryPositions = std::vector <MemoryPosition>;

		Address _minAddress, _maxAddress;
		size_t _numPositions;
		MemoryPositions _memPositions;
	};

	// ---
	inline bool MemoryView::isIn (const Address& a, int& dt) const
	{ 
		bool result = false;

		int dtT = a.distanceWith (_minAddress);
		if (dtT >= 0 && (size_t) dtT <= _numPositions)
		{ 
			PhysicalStorageSubset* f = nullptr;
			const PhysicalStorageSubsetsList& pL = _memPositions [dtT]._storages;
			for (MCHEmul::PhysicalStorageSubsetsList::const_iterator i = pL.begin (); i != pL.end () && f == nullptr; i++)
				if ((*i) -> active ()) 
					f = (*i); // The first one active...

			if (result = (f != nullptr)) 
				dt = dtT;
		}

		return (result);
	}

	/** To simplify the way a map of elements is managed. */
	using MemoryViews = std::map <int, MemoryView*>;

	/** To DUMP the content of the memory. */
	struct MemoryDUMP final
	{
		int _id; // of the view...
		std::vector <MemoryViewDUMP> _data;

		InfoStructure getInfoStructure () const;
	};

	/** 
	  * A memory is just an agregation of eveytthing above: phisical storages, subsets and views. \n
	  *	A view has to de declared as the active one. \n
	  *	All accesses will be done throught out that active view. \n
	  *	This gives the user the possible to have different "views" of the memory in different
	  *	moments of the execution of the main cycle from, e.g. different elements in the CPU. */
	class Memory : public MotherboardElement
	{
		public:
		/** To configure general parameters of the memory behaviour. */
		class Configuration final 
		{
			public:
			Configuration ()
				: _bufferMemorySetCommands (false),
				  _memorySetCommands ()
							{ }

			// Managing the set commands...
			/** Should the set commands be buffered? */
			bool bufferMemorySetCommands (PhysicalStorageSubset* pS) const
							{ return (_bufferMemorySetCommands && pS -> bufferMemorySetCommands ()); }
			void setBufferMemorySetCommands (bool a)
							{ if (!(_bufferMemorySetCommands = a)) executeMemorySetCommandsBuffered (); }
			void addMemorySetCommand (const SetMemoryCommand& o)
							{ _memorySetCommands.emplace_back (o); }
			void addMemorySetCommand (SetMemoryCommand&& o)
							{ _memorySetCommands.emplace_back (std::move (o)); }
			/** Execute all set memory commands buffered if that configuration is active.
				Bear in mind that when that configuration is disconnected, this method will also be executed. */
			inline void executeMemorySetCommandsBuffered ();

			private:
			bool _bufferMemorySetCommands;
			SetMemoryCommands _memorySetCommands;
		};

		/** To content all together the elements a memory is made up of. */
		class Content final
		{
			public:
			Content ()
				: _physicalStorages (), _subsets (), _views (),
				  _error (false)
							{ }

			/** Unless this method is executed, 
				the class is always in error (_error = true). \n
				The constructor of Memory executes it. */
			bool verifyCoherence () const;

			const PhysicalStorages& physicalStorages () const
							{ return (_physicalStorages); }
			PhysicalStorages& physicalStorages ()
							{ return (_physicalStorages); }
			bool existsPhysicalStorage (int id) const
							{ return (_physicalStorages.find (id) != _physicalStorages.end ()); }
			const PhysicalStorage* physicalStorage (int id) const
							{ return ((const PhysicalStorage*) (((Content*) (this)) -> physicalStorage (id))); }
			PhysicalStorage* physicalStorage (int id)
							{ PhysicalStorages::const_iterator i = _physicalStorages.find (id); 
								return ((i != _physicalStorages.end ()) ? (*i).second : nullptr); }

			const PhysicalStorageSubsets& subsets ( ) const
							{ return (_subsets); }
			PhysicalStorageSubsets& subsets ( )
							{ return (_subsets); }
			bool existsSubset (int id) const
							{ return (_subsets.find (id) != _subsets.end ()); }
			const PhysicalStorageSubset* subset (int id) const
							{ return ((const PhysicalStorageSubset*) (((Content*) (this)) -> subset (id))); }
			PhysicalStorageSubset* subset (int id)
							{ PhysicalStorageSubsets::const_iterator i = _subsets.find (id); 
								return ((i != _subsets.end ()) ? (*i).second : nullptr); }

			const MemoryViews& views () const
							{ return (_views); }
			MemoryViews& views ()
							{ return (_views); }
			bool existsView (int id) const
							{ return (_views.find (id) != _views.end ()); }
			const MemoryView* view (int id) const
							{ return ((const MemoryView*) (((Content*) (this)) -> view (id))); }
			MemoryView* view (int id)
							{ MemoryViews::const_iterator i = _views.find (id); 
								return ((i != _views.end ()) ? (*i).second : nullptr); }
			MemoryView* firstView () const
							{ return ((*_views.begin ()).second); }

			virtual bool initialize ();

			bool error () const
							{ return (_error); }

			/** Can be accesses directly to set them up. */
			PhysicalStorages _physicalStorages;
			PhysicalStorageSubsets _subsets;
			MemoryViews _views;

			private:
			// Implementation
			mutable bool _error;
		};

		using Contents = std::map <int, Content>;
		using AdditionalSubsets = std::map <int, PhysicalStorageSubsets>;

		Memory (int id, const Content& cnt, const Attributes& attrs = { });

		Memory (const Memory&) = delete;

		Memory& operator = (const Memory&) = delete;

		virtual ~Memory ();

		Memory (Memory&&) = delete;

		Memory& operator = (Memory&&) = delete;

		static Configuration& configuration ()
							{ return (_CONFIGURATION); }

		const PhysicalStorages& physicalStorages () const
							{ return (_content.physicalStorages ()); }
		bool existsPhysicalStorage (int id) const
							{ return (_content.existsPhysicalStorage (id)); } 
		const PhysicalStorage* physicalStorage (int id) const
							{ return (_content.physicalStorage (id)); }
		PhysicalStorage* physicalStorage (int id)
							{ return (_content.physicalStorage (id)); }

		const PhysicalStorageSubsets& subsets () const
							{ return (_content.subsets ()); }
		bool existsSubset (int id) const
							{ return (_content.existsSubset (id)); }
		const PhysicalStorageSubset* subset (int id) const
							{ return (_content.subset (id)); }
		PhysicalStorageSubset* subset (int id)
							{ return (_content.subset (id)); }

		const MemoryViews& views () const
							{ return (_content.views ()); }
		bool existsView (int id) const
							{ return (_content.existsView (id)); }
		const MemoryView* view (int id) const
							{ return (_content.view (id));  }
		MemoryView* view (int id)
							{ return (_content.view (id));  }

		/** Managing the active view of the memory. */
		const MemoryView* activeView () const
							{ return (_activeView); }
		MemoryView* activeView ()
							{ return (_activeView); }
		void setActiveView (int id)
							{ if (existsView (id)) _activeView = view (id); }

		/** This method is quite important for changing dinamically the configuration of the memory.
			Additional subsets can be added and taken off from the active view. 
			Memory view where to add all in is optional. If none is defined, the active one is used. \n
			Returns false when either a set of subsets with the same id already exists (adding),
			or no set of subsets exists with that id. \n
			After that firs verification returns also false if the elements of the set of subsets 
			were not all of them finally added ot removed from the view. \n
			Other situations returs true. */
		bool addAdditionalSubsets (int id, const PhysicalStorageSubsets& ss, MemoryView* v = nullptr);
		bool removeAdditionalSubsets (int id, MemoryView* v = nullptr);

		bool isIn (const Address& a, int & dt) const
							{ return (_activeView -> isIn (a, dt)); }
		size_t size () const
							{ return (_activeView -> size ()); }

		// Manage the stack...
		/** To force the system to look for the stack again. */
		void resetStack ()
							{ _stack = nullptr; }
		/** To get the stack space reserve in the memory. 
			This is something every implementation must define. */
		const Stack* stack () const
							{ return ((const Stack*) (((Memory*) (this)) -> stack ())); }
		Stack* stack () 
							{ return ((_stack == nullptr) ? (_stack = lookForStack ()) : _stack); }

		/** To set the CPU view.
			This is important for the right CPU behivour. */
		void setCPUView ()
							{ _activeView = (_cpuView == nullptr) ? (_cpuView = lookForCPUView ()) : _cpuView; }

		// Manage the memory...
		/** The _activeView might not be nullptr ever, or a crahs is generated. */
		const UByte& value (const Address& a) const
							{ return (_activeView -> value (a)); }
		void set (const Address& a, const UByte& v, bool f = false /** To force even when it is a ROM. */)
							{ _activeView -> set (a, v, f); }
		UBytes values (const Address& a, size_t nB) const
							{ return (_activeView -> values (a, nB)); }
		void set (const Address& a, const UBytes& v, bool f = false)
							{ _activeView -> set (a, v, f); }
		std::vector <UByte> bytes (const Address& a, size_t nB) const
							{ return (_activeView -> bytes (a, nB)); }
		void set (const Address& a, const std::vector <UByte>& v, bool f = false)
							{ _activeView -> set (a, v, f); }

		// To put a value/values in the memory...
		// The methods use put method of the MemoryView (@see MemotyView) to avoid buffering behaviour if any...
		void put (const Address& a, const UByte& d, bool f = false)
							{ _activeView -> put (a, d, f); }
		void put (const Address& a, const UBytes& v, bool f = false)
							{ _activeView -> put (a, v.bytes (), f); }
		void put (const Address& a, const std::vector <UByte>& v, bool f = false)
							{ _activeView -> put (a, v, f); }
		void put (const DataMemoryBlock& mb, bool f = false)
							{ put (mb.startAddress (), mb.bytes (), f); }
		void put (const DataMemoryBlocks& mb, bool f = false)
							{ for (const auto& i : mb) put (i, f); }

		// Fill up the memory...
		/** To fill the all memory with value/values. */
		void fillWith (const MCHEmul::UByte& b, bool f = false)
							{ _activeView -> fillWith (b, f); }
		/** To fill a zone of the memory with a value. */
		void fillWith (const MCHEmul::Address& addr, const MCHEmul::UByte& b, size_t nB, bool f = false)
							{ _activeView -> fillWith (addr, b, nB, f); }

		/** To get a DUMP from all active "memories" between two positions. */
		// Only for the active view...
		MemoryDUMP dumpActive (const Address& f, const Address& t) const
							{ return (MemoryDUMP { _id, { _activeView -> dump (f, t) } }); }
		// For all views...
		MemoryDUMP dump (const Address& f, const Address& t) const;
		/** To get the structure of the memory,
			considering all (a = true) or only the active physical storages (a = false). */
		// Only for the active view...
		MemoryDUMP dumpStructureActive (bool a = true)
							{ return (MemoryDUMP { _id, { _activeView -> dumpStructure (a) } }); }
		// In all views...
		MemoryDUMP dumpStructure (bool a = true);

		/** It can be overloaded later, to set the specific content of specific zones. \n
			By default only subsets have to be initialized, and all of them become active and also active for reading. */
		virtual bool initialize () override
							{ return ((_error == MCHEmul::_NOERROR) ? _content.initialize () : false); }

		/** Load info from a file into a memory using the active view. */
		bool loadInto (const std::string& fN, const Address& a)
							{ return (_activeView -> loadInto (fN, a)); }

		/** Save a set of "nB" bytes from the memory into a file file, using the activeView. */
		bool saveFrom (const std::string& fN, size_t nB, const Address& a)
							{ return (_activeView -> saveFrom (fN, nB, a)); }

		/** To get the last error happend (usually at construction level) */
		unsigned int error () const
							{ return (_error); }
		void resetErrors ()
							{ _error = _NOERROR; }

		/** The info is the same than in the case of the active view (@see MemoryView). */
		virtual InfoStructure getInfoStructure () const override
							{ return (_activeView -> getInfoStructure ()); }

		/** Manages the deep debug file. \n
			Take care it can be set back to a nullptr. */
		bool deepDebugActive () const
							{ return (_deepDebugFile != nullptr && _deepDebugFile -> active ()); }
		void setDeepDebugFile (DebugFile* dF, const std::vector <int>& mId);
		const DebugFile* deepDebugFile () const
							{ return (_deepDebugFile); }
		DebugFile* deepDebugFile ()
							{ return (_deepDebugFile); }

		protected:
		virtual Stack* lookForStack () = 0;
		virtual MemoryView* lookForCPUView () = 0;

		protected:
		Content _content;
		AdditionalSubsets _additionalSubsets;
		static Configuration _CONFIGURATION;

		// To manage the debug info...
		DebugFile* _deepDebugFile;

		// Implementation
		MemoryView* _activeView;
		mutable Stack* _stack;
		mutable MemoryView* _cpuView;
		mutable unsigned int _error;
	};

	// ---
	inline void Memory::Configuration::executeMemorySetCommandsBuffered ()
	{ 
		if (_bufferMemorySetCommands)
		{
			for (auto& i : _memorySetCommands) 
				i.execute (); 
					
			_memorySetCommands.clear ();
		}
	}
}

#endif
  
// End of the file
/*@}*/