/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: Memory.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 07/04/2021 \n
 *	Description: The memory managed by the machine.
 *	Versions: 1.0 Initial.
 *			  2.0 Radical change to speed up accesses and to try to reflect 
 *				  better what the real memory structure is.	
 */

#ifndef __MCHEMUL_MEMORY__
#define __MCHEMUL_MEMORY__

#include <CORE/global.hpp>
#include <CORE/InfoClass.hpp>
#include <CORE/Address.hpp>
#include <CORE/UByte.hpp>
#include <CORE/UBytes.hpp>
#include <CORE/DtMemoryBlock.hpp>

namespace MCHEmul
{
	class PhysicalStorageSubset;
	class MemoryView;
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

		friend PhysicalStorageSubset;

		enum class Type { _ROM, _RAM };

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

		protected:
		// This methods can only be invoked from PhisicalMemorySubset class
		// IMPORTANT NOTE: 
		// They go direclty to the memory and can be dangerous for the stability of the system!

		/** Managing the content of the memory. \n
			Take into account that whether it is a RAM or a ROM is not controlled here. \n
			It is something that has to be guarantteed by the subsets built on top of this. */
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
			If nothing is said, the info is load from the very first byte. */
		bool loadInto (const std::string& fN, size_t p = 0);

		/** Save the a number of bytes "nB" of the memory into a file, string from a position p. \n
			If nothing is said the position choosen is the initial one. */
		bool saveFrom (const std::string& fN, size_t nB, size_t p = 0);

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

	/** Represents a subset of the physical storage. \n
		Many subsets can be created over the same physical location. */
	class PhysicalStorageSubset : public InfoClass
	{
		public:
		friend MemoryView;

		/** It is guarantteed that it must a subset within the boundaries of the phisical storage behind. \n
			Otherwise the view will be have the same size than that. \n
			The reference to the phisical storage can't be null at all. \n 
			The subset is not the owner of the phisical storage. \n
			THe phisical storage can be either active or inactive. */
		PhysicalStorageSubset (int id, PhysicalStorage* pS, size_t pp /** link a phisical */, const Address& a, size_t s);

		PhysicalStorageSubset (const PhysicalStorageSubset&) = delete;

		PhysicalStorageSubset& operator = (const PhysicalStorageSubset&) = delete;

		virtual ~PhysicalStorageSubset () 
							{ /** Nothing to do by default. */ }

		PhysicalStorageSubset (PhysicalStorageSubset&&) = delete;

		PhysicalStorageSubset& operator = (PhysicalStorageSubset&&) = delete;

		/** Id for the view. It is diiferent than the id of the phisical storage behind. */
		int id () const
							{ return (_id); }
		const PhysicalStorage* physicalStorage () const
							{ return (_physicalStorage); }
		PhysicalStorage* physicalStorage ()
							{ return (_physicalStorage); }
		PhysicalStorage::Type type () const
							{ return (_physicalStorage -> type ()); }
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

		/** The address may only be "in" when the subset is active. */
		bool isIn (const Address& a, int& dt) const
							{ return (_active && (a >= _initialAddress && (dt = _initialAddress.distanceWith (a)) < (int) _size)); }

		/** The internal method "setValue" is invoked when possible. 
			When the address requested is not "in" the subset, nothing happens. */
		void set (const Address& a, const UByte& d, bool f = false)
							{ int dt = 0; if (_physicalStorage -> canBeWriten (f) && isIn (a, dt)) setValue (dt, d); }
		/** The internal method "readValue" is invoked when possible. 
			When the address requested is not "in" the subset, -0 is returned. */
		const UByte& value (const Address& a) const
							{ int dt = 0; return (_activeForReading && (isIn (a, dt)) 
								? readValue (dt) : PhysicalStorage::_DEFAULTVALUE); }
		UBytes values (const Address& a, size_t nB) const
							{ return (UBytes (bytes (a, nB))); }
		void set (const Address& a, const UBytes& v, bool f = false)
							{ set (a, v.bytes (), f); }
		/** When some of the requested position is out the boundaries, an empty map is returned. */
		inline std::vector <UByte> bytes (const Address& a, size_t nB) const;
		std::vector <UByte> bytes () const
							{ return (bytes (initialAddress (), size ())); }
		inline void set (const Address& a, const std::vector <UByte>& v, bool f = false);

		/** To init the memory. It might be overloaded. By default the "defaultData" value is assigned. 
			It doesn't matter whether the subset is or not active. */
		virtual void initialize ()
							{ for (size_t i = 0; i < _size; i++) setValue (i, _defaultData [i]); }

		/** First "sA" bytes of the file defining the address where to load the bytes. 
			It is also necessary to define whether that address is or not big endian. */
		bool load (const std::string& fN, size_t sA, bool bE = true);
		/** Everything from the memory position received. */
		bool loadInto (const std::string& fN, const Address& a)
							{ return ((a >= _initialAddress) 
								? _physicalStorage -> loadInto (fN, _initialAddress.distanceWith (a)) : false); }
		/** The address where to load things into is always the initial address of the memory. */
		bool loadInto (const std::string& fN)
							{ return (loadInto (fN, initialAddress ())); }

		/** Save the content of full PhysycalStorageSubset into a file. 
			The first "sA" bytes will indicate the address of the first position in big endian of little endian format. */
		bool save (const std::string& fN, size_t sA, bool bE = true);
		bool saveFrom (const std::string& fN, size_t nB, const Address& a)
							{ return ((a >= _initialAddress && (a + nB) < lastAddress ()) 
								? _physicalStorage -> saveFrom (fN, _initialAddress.distanceWith (a), nB) : false); }

		/** To keep the values for fither initialization. */
		void fixDefaultValues ()
							{ _defaultData = _physicalStorage -> values (_initialPhisicalPosition, _size).bytes (); }

		/**
		  *	The name of the fields are: \n
		  *	ID			= Attribute: Id of the memory. \n
		  *	ADDRESS		= Attribute: Address where the storage starts. \n
		  *	SIZE		= Attribute: Size in bytes. \n
		  * ACTIVE		= Attribute: YES when access operations are possible and NO in other case. \n
		  * READ		= Attribute: YES when read operations are possible and NO in other case. \n
		  * BYTES		= Attribute: Info of the memory.
		  */
		virtual InfoStructure getInfoStructure () const override;

		protected:
		/** They could be overeloaded for specific subsets: Chips registers,... 
			By default, the instructions are transmitted to the physical storage.
			At this point there is guaranteed that the byte requested exist in the phisical storage. 
			The number of register (relative) with in the phisical storage is received. */
		virtual void setValue (size_t nB, const UByte& d)
							{ _physicalStorage -> set (nB + _initialPhisicalPosition, d); }
		virtual const UByte& readValue (size_t nB) const
							{ return (_physicalStorage -> value (nB + _initialPhisicalPosition)); }

		protected:
		const int _id = -1; 
		PhysicalStorage* _physicalStorage;
		/** The position on the phisical storage, this subset is connected to. */
		size_t _initialPhisicalPosition;
		/** Which address does the initial position defined above represent ? */
		Address _initialAddress;
		/** How many bytes of info does this subset represent? */
		size_t _size;

		// Implementation
		bool _active;
		bool _activeForReading;
		std::vector <MCHEmul::UByte> _defaultData;
	};

	// ---
	inline std::vector <UByte> PhysicalStorageSubset::bytes (const Address& a, size_t nB) const
	{
		std::vector <UByte> result;

		int dt;
		if (_active && _activeForReading &&
			nB <= _size && (a >= _initialAddress && (dt = _initialAddress.distanceWith (a)) <= (int) (_size - nB))) 
			for (size_t i = 0; i < nB; i++)
				result.emplace_back (readValue (dt + i)); 

		return (result);
	}

	// ---
	inline void PhysicalStorageSubset::set (const Address& a, const std::vector <UByte>& v, bool f)
	{
		std::vector <MCHEmul::UByte> result;

		int dt; 
		if (_active && _physicalStorage -> canBeWriten (f) &&
			v.size () < _size && (a >= _initialAddress && (dt = _initialAddress.distanceWith (a)) <= (int) (_size - v.size ())))
			for (size_t i = 0; i < v.size (); i++)
				setValue (dt + i, v [i]);
	}

	/** To simplify the way a map of elements is managed. */
	using PhysicalStorageSubsets = std::map <int, PhysicalStorageSubset*>;
	using PhysicalStorageSubsetsList = std::vector <PhysicalStorageSubset*>;

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
		
		inline bool isIn (const Address& a, int & dt) const;

		Address middleMemoryAddress () const
							{ return (_minAddress + (_numPositions / 2)); }

		/** If there had been several subsets behind, the write operation would happen 
			on the first writtable subset possible. */
		inline void set (const Address& a, const UByte& d, bool f = false);
		/** If there had been several subsets behind, the read operation would happen 
			on the first readable subset possible. */
		inline const UByte& value (const Address& a) const;
		UBytes values (const Address& a, size_t nB) const
							{ return (UBytes (bytes (a, nB))); }
		inline std::vector <UByte> bytes (const Address& a, size_t nB) const;
		void set (const Address& a, const UBytes& v, bool f = false)
							{ set (a, v.bytes (), f); }
		inline void set (const Address& a, const std::vector <UByte>& v, bool f = false);

		/** To init the memory view. 
			It might be overloaded. By default the "defaultData" value is assigned. */
		virtual void initialize () 
							{ for (const auto& i : _subsets) i.second -> initialize (); }

		/** Load into the first subset holding the address parameter. */
		bool loadInto (const std::string& fN, const Address& a);

		/** Save "nB" of memory of the first subset matching the address parameter. */
		bool saveFrom (const std::string& fN, size_t nB,const Address& a);

		/**
		  *	The name of the fields are: \n
		  *	STORAGES	= InfoStructure: Every storage belonging to the view (@see PhysicalStorageSubset).
		  */
		virtual InfoStructure getInfoStructure () const override;

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

	// ---
	inline const UByte& MemoryView::value (const Address& a) const
	{
		UByte& result = PhysicalStorage::_DEFAULTVALUE;

		int dtT = _minAddress.distanceWith (a);
		if (dtT >= 0 && (size_t) dtT <= _numPositions)
		{
			PhysicalStorageSubset* fS = nullptr;
			const PhysicalStorageSubsetsList& pL = _memPositions [dtT]._storages;
			for (size_t i = 0; i < pL.size () && fS == nullptr; i++)
				if (pL [i] -> active () && pL [i] -> activeForReading ()) 
					fS = pL [i];

			if (fS != nullptr)
				result = fS -> readValue (a - fS -> initialAddress ());
		}

		return (result);
	}

	// ---
	inline std::vector <UByte> MemoryView::bytes (const Address& a, size_t nB) const
	{
		std::vector <UByte> result;

		// If more bytes are required than max available nothing is returned...
		int dtT = _minAddress.distanceWith (a);
		if (dtT >= 0 && (size_t) dtT <= (_numPositions - nB))
		{
			for (size_t i = 0; i < nB; i++)
			{ 
				PhysicalStorageSubset* fS = nullptr;
				const PhysicalStorageSubsetsList& pL = _memPositions [dtT + i]._storages;
				for (size_t j = 0; j < pL.size () && fS == nullptr; j++)
					if (pL [j] -> active () && pL [j] -> activeForReading ()) 
						fS = pL [j];

				result.emplace_back ((fS != nullptr) 
					? fS -> readValue (a - fS -> initialAddress () + i) : PhysicalStorage::_DEFAULTVALUE);
			}
		}
			
		return (result);
	}

	// ---
	inline void MemoryView::set (const Address& a, const UByte& d, bool f)
	{
		int dtT = _minAddress.distanceWith (a);
		if (dtT >= 0 && (size_t) dtT <= _numPositions)
		{
			PhysicalStorageSubset* fS = nullptr;
			const PhysicalStorageSubsetsList& pL = _memPositions [dtT]._storages;
			for (size_t i = 0; i < pL.size () && fS == nullptr; i++)
				if (pL [i] -> active () && pL [i] -> canBeWriten (f)) fS = pL [i];

			if (fS != nullptr)
				fS -> setValue (a - fS -> initialAddress (), d);
		}
	}

	// ---
	inline void MemoryView::set (const Address& a, const std::vector <UByte>& v, bool f)
	{ 
		// If there are more bytes to set than max available nothing is done...
		int dtT = _minAddress.distanceWith (a);
		if (dtT >= 0 && (size_t) dtT <= (_numPositions - v.size ()))
		{
			for (size_t i = 0; i < v.size (); i++)
			{
				PhysicalStorageSubset* fS = nullptr;
				const PhysicalStorageSubsetsList& pL = _memPositions [dtT + i]._storages;
				for (size_t j = 0; j < pL.size () && fS == nullptr; j++)
					if (pL [j] -> active () && pL [j] -> canBeWriten (f)) fS = pL [j];

				if (fS != nullptr)
					fS -> setValue (a - fS -> initialAddress () + i, v [i]);
			}
		}
	}

	/** To simplify the way a map of elements is managed. */
	using MemoryViews = std::map <int, MemoryView*>;

	/** 
	  * A memory is just an agregation of eveytthing above: phisical storages, subsets and views. \n
	  *	A view has to de declared as the active one. \n
	  *	All accesses will be done throught out that active view. \n
	  *	This gives the user the possible to have different "views" of the memory in different
	  *	moments of the execution of the main cycle from, e.g. different elements in the CPU. \n
	  */
	class Memory : public InfoClass
	{
		public:
		/** To content all together the elements a memory is made up of. */
		class Content final
		{
			public:
			Content ()
				: _physicalStorages (), _subsets (), _views (),
				  _error (true)
							{ }

			/** Unless this method is executed, the class is always in error (_error = true). */
			bool verifyCoherence () const;

			const PhysicalStorages& physicalStorages () const
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
			bool existsSubset (int id) const
							{ return (_subsets.find (id) != _subsets.end ()); }
			const PhysicalStorageSubset* subset (int id) const
							{ return ((const PhysicalStorageSubset*) (((Content*) (this)) -> subset (id))); }
			PhysicalStorageSubset* subset (int id)
							{ PhysicalStorageSubsets::const_iterator i = _subsets.find (id); 
								return ((i != _subsets.end ()) ? (*i).second : nullptr); }

			const MemoryViews& views () const
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

			bool initialize ();

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

		Memory (const Content& cnt);

		Memory (const Memory&) = delete;

		Memory& operator = (const Memory&) = delete;

		virtual ~Memory ();

		Memory (Memory&&) = delete;

		Memory& operator = (Memory&&) = delete;

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

		// Managing the active view of the memory...
		const MemoryView* activeView () const
							{ return (_activeView); }
		MemoryView* activeView ()
							{ return (_activeView); }
		void setActiveView (int id)
							{ if (existsView (id)) _activeView = view (id); }

		bool isIn (const Address& a, int & dt) const
							{ return (_activeView -> isIn (a, dt)); }

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

		const UByte& value (const Address& a) const
							{ return (_activeView -> value (a)); }
		void set (const Address& a, const UByte& v, bool f = false /** To force even when it is a rom. */)
							{ _activeView -> set (a, v, f); }
		UBytes values (const Address& a, size_t nB) const
							{ return (_activeView -> values (a, nB)); }
		void set (const Address& a, const UBytes& v, bool f = false)
							{ _activeView -> set (a, v, f); }
		std::vector <UByte> bytes (const Address& a, size_t nB) const
							{ return (_activeView -> bytes (a, nB)); }
		void set (const Address& a, const std::vector <UByte>& v, bool f = false)
							{ _activeView -> set (a, v, f); }
		/** To set a vector of codeblocks. */
		void set (const DataMemoryBlocks& mb, bool f = false)
							{ for (const auto& i : mb) set (i.startAddress (), i.bytes (), f); }

		/** It can be overloaded later, to set the specific content of specific zones. \n
			By default only subsets have to be initialized, and all of them become active and also active for reading. */
		virtual bool initialize ()
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

		protected:
		virtual Stack* lookForStack () = 0;
		virtual MemoryView* lookForCPUView () = 0;

		protected:
		Content _content;

		// Implementation
		MemoryView* _activeView;
		mutable Stack* _stack;
		mutable MemoryView* _cpuView;
		mutable unsigned int _error;
	};
}

#endif
  
// End of the file
/*@}*/