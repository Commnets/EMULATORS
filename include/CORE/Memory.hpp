/** \ingroup CPU */
/*@{*/

/**	
 *	@file	
 *	File: Memory.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forni?s (EMULATORS library) \n
 *	Creation Date: 07/04/2021 \n
 *	Description: The memory managed by the machine.
 *	Versions: 1.0 Initial.
 *			  2.0 Radical change to speed up accesses and to try to reflect 
 *				  better what the real memory structure is.	
 */

#ifndef __MCHEMUL_MEMORY__
#define __MCHEMUL_MEMORY__

#include <CORE/global.hpp>
#include <CORE/Address.hpp>
#include <CORE/UByte.hpp>
#include <CORE/UBytes.hpp>

namespace MCHEmul
{
	class PhisicalStorageSubset;
	class MemoryView;
	class Stack;

	/** Represents what phisically speaking the memoy is: \n
		Just a place to keep info. \n
		It can be either RAM or ROM depending whether is possible to modify data. \n
		The phisical storage can only be accesed through a subset, controlling the boundaries of the phisical storage. */
	class PhisicalStorage final
	{
		public:
		/** The default value which is used to initialize the memory, can be changed. */
		static UByte _DEFAULTVALUE;

		friend PhisicalStorageSubset;

		enum class Type { _ROM, _RAM };

		PhisicalStorage () = delete;

		PhisicalStorage (int id, Type t, size_t s)
			: _id (id), _type (t), 
			  _data (s, UByte::_0)
							{ }

		PhisicalStorage (const PhisicalStorage&) = default;

		PhisicalStorage& operator = (const PhisicalStorage&) = default;

		int id () const
							{ return (_id); }
		Type type () const
							{ return (_type); }
		size_t size () const
							{ return (_data.size ()); }
		
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
		std::vector <UByte> bytes (size_t pB, size_t nB) const;
		void set (size_t pB, const std::vector <UByte>& v)
							{ for (size_t i = 0; i < v.size (); i++) set (pB + i, v [i]); }

		/** Load the info, as many bytes as possible, from a specific byte of the memory. 
			If nothing is said, the info is load from he vey first bye. */
		bool loadInto (const std::string& fN, size_t p = 0);

		protected:
		const int _id = -1; // Modified at construction level
		const Type _type = Type::_RAM; // Modified also at construction level
		std::vector <MCHEmul::UByte> _data;
	};

	/** To simplify the way a map of elements is managed. */
	using PhisicalStorages = std::map <int, PhisicalStorage*>;

	/** Represents a subset of the physical storage. \n
		Many subsets can be created over the same physical location. */
	class PhisicalStorageSubset
	{
		public:
		friend MemoryView;

		PhisicalStorageSubset () = delete;

		/** It is guarantteed that it must a subset within the boundaries of the phisical storage behind. \n
			Otherwise the view will be have the same size than that. \n
			The reference to the phisical storage can't be null at all. \n 
			The subset is not the owner of the phisical storage. \n
			THe phisical storage can be either active or inactive. */
		PhisicalStorageSubset (int id, PhisicalStorage* pS, size_t pp /** link a phisical */, const Address& a, size_t s);

		PhisicalStorageSubset (const PhisicalStorageSubset&) = default;

		virtual ~PhisicalStorageSubset () 
							{ /** Nothing to do by default. */ }

		PhisicalStorageSubset& operator = (const PhisicalStorageSubset&) = default;

		/** Id for the view. It is diiferent than the id of the phisical storage behind. */
		int id () const
							{ return (_id); }
		const PhisicalStorage* phisicalStorage () const
							{ return (_phisicalStorage); }
		PhisicalStorage* phisicalStorage ()
							{ return (_phisicalStorage); }
		PhisicalStorage::Type type () const
							{ return (_phisicalStorage -> type ()); }
		const Address& initialAddress () const
							{ return (_initialAddress); }
		size_t size () const
							{ return (_size); }
		
		bool active () const
							{ return (_active); }
		void setActive (bool a)
							{ _active = a; }
		bool activeForReading () const
							{ return (_activeForReading); }
		void setActiveForReading (bool aR)
							{ _activeForReading = aR; }

		bool canBeWriten (bool f) const
							{ return (_active && _phisicalStorage -> canBeWriten (f)); }

		/** The address may only be "in" when the subset is active. */
		bool isIn (const Address& a, int& dt) const
							{ return (_active && (a >= _initialAddress && (dt = _initialAddress.distanceWith (a)) <= (int) _size)); }

		/** The internal method "setValue" is invoked when possible. 
			When the address requested is not "in" the subset, nothing happens. */
		void set (const Address& a, const UByte& d, bool f = false)
							{ int dt = 0; if (_phisicalStorage -> canBeWriten (f) && isIn (a, dt)) setValue (dt, d); }
		/** The internal method "readValue" is invoked when possible. 
			When the address requested is not "in" the subset, -0 is returned. */
		const UByte& value (const Address& a) const
							{ int dt = 0; return (_activeForReading && (isIn (a, dt)) 
								? readValue (dt) : PhisicalStorage::_DEFAULTVALUE); }
		UBytes values (const Address& a, size_t nB) const
							{ return (UBytes (bytes (a, nB))); }
		void set (const Address& a, const UBytes& v, bool f = false)
							{ set (a, v.bytes (), f); }
		/** When some of the requested position is out the boundaries, an empty map is returned. */
		std::vector <UByte> bytes (const Address& a, size_t nB) const;
		void set (const Address& a, const std::vector <UByte>& v, bool f = false);

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
								? _phisicalStorage -> loadInto (fN, _initialAddress.distanceWith (a)) : false); }
		/** The address where to load things into is always the initial address of the memory. */
		bool loadInto (const std::string& fN)
							{ return (loadInto (fN, initialAddress ())); }

		/** To keep the values for fither initialization. */
		void fixDefaultValues ()
							{ _defaultData = _phisicalStorage -> values (_initialPhisicalPosition, _size).bytes (); }

		friend std::ostream& operator << (std::ostream& o, const PhisicalStorageSubset& ps);

		protected:
		/** They could be overeloaded for specific subsets: Chips registers,... 
			By default, the instructions are transmitted to the physical storage.
			At this point there is guaranteed that the byte requested exist in the phisical storage. 
			The number of register (relative) with in the phisical storage is received. */
		virtual void setValue (size_t nB, const UByte& d)
							{ _phisicalStorage -> set (nB + _initialPhisicalPosition, d); }
		virtual const UByte& readValue (size_t nB) const
							{ return (_phisicalStorage -> value (nB + _initialPhisicalPosition)); }

		protected:
		const int _id = -1; 
		PhisicalStorage* _phisicalStorage;
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

	/** To simplify the way a map of elements is managed. */
	using PhisicalStorageSubsets = std::map <int, PhisicalStorageSubset*>;
	using PhisicalStorageSubsetsList = std::vector <PhisicalStorageSubset*>;

	/** A memory view represents a set of subsets over phisical storage (one or many). \n
		The subsets can combine either RAM or ROM access and can also overlap each other. */
	class MemoryView
	{
		public:
		MemoryView () = delete;

		/** The memory view is not the owner of the subsets. */
		MemoryView (int id, PhisicalStorageSubsets ss)
			: _id (id), _subsets (ss)
							{ }

		MemoryView (const MemoryView&) = default;

		virtual ~MemoryView ()
							{ }

		MemoryView& operator = (const MemoryView&) = default;

		int id () const
							{ return (_id); }

		// To manage the subsets...
		const PhisicalStorageSubsets& subsets () const
							{ return (_subsets); }
		bool existsSubset (int id) const
							{ return (_subsets.find (id) != _subsets.end ()); }
		const PhisicalStorageSubset* subset (int id) const
							{ return ((const PhisicalStorageSubset*) (((MemoryView*) (this)) -> subset (id))); }
		PhisicalStorageSubset* subset (int id)
							{ PhisicalStorageSubsets::const_iterator i = _subsets.find (id); 
								return ((i != _subsets.end ()) ? (*i).second : nullptr); }
		
		bool isIn (const Address& a, int & dt) const;

		/** If there had been several subsets behind, the write operation would happen 
			on the first writtable subset possible. */
		void set (const Address& a, const UByte& d, bool f = false);
		/** If there had been several subsets behind, the read operation would happen 
			on the first readable subset possible. */
		const UByte& value (const Address& a) const;
		UBytes values (const Address& a, size_t nB) const
							{ return (UBytes (bytes (a, nB))); }
		void set (const Address& a, const UBytes& v, bool f = false)
							{ set (a, v.bytes (), f); }
		std::vector <UByte> bytes (const Address& a, size_t nB) const;
		void set (const Address& a, const std::vector <UByte>& v, bool f = false);

		/** To init the memory view. 
			It might be overloaded. By default the "defaultData" value is assigned. */
		virtual void initialize () 
							{ for (auto i : _subsets) i.second -> initialize (); }

		/** Loaded into the first subset holding the address parameter. */
		bool loadInto (const std::string& fN, const Address& a);

		friend std::ostream& operator << (std::ostream& o, const MemoryView& mv);

		protected:
		int _id;
		PhisicalStorageSubsets _subsets;
	};

	/** To simplify the way a map of elements is managed. */
	using MemoryViews = std::map <int, MemoryView*>;

	/** 
	  * A memory is just an agregation of eveytthing above: phisical storages, subsets and views. \n
	  *	A view has to de declared as the active one. \n
	  *	All accesses will be done throught out that active view. \n
	  *	This gives the user the possible to have different "views" of the memory in different
	  *	moments of the execution of the main cycle from, e.g. different elements in the CPU. \n
	  */
	class Memory
	{
		public:
		/** To content all together the elements a memory is made up of. */
		class Content final
		{
			public:
			Content ()
				: _phisicalStorages (), _subsets (), _views (),
				  _error (true)
							{ }

			Content (const Content&) = default;

			~Content () = default;

			Content& operator = (const Content&) = default;

			/** Unless this method is executed, the class is always in error (_error = true). */
			bool verifyCoherence () const;

			const PhisicalStorages& phisicalStorages () const
							{ return (_phisicalStorages); }
			bool existsPhisicalStorage (int id) const
							{ return (_phisicalStorages.find (id) != _phisicalStorages.end ()); }
			const PhisicalStorage* phisicalStorage (int id) const
							{ return ((const PhisicalStorage*) (((Content*) (this)) -> phisicalStorage (id))); }
			PhisicalStorage* phisicalStorage (int id)
							{ PhisicalStorages::const_iterator i = _phisicalStorages.find (id); 
								return ((i != _phisicalStorages.end ()) ? (*i).second : nullptr); }

			const PhisicalStorageSubsets& Subsets ( ) const
							{ return (_subsets); }
			bool existsSubset (int id) const
							{ return (_subsets.find (id) != _subsets.end ()); }
			const PhisicalStorageSubset* subset (int id) const
							{ return ((const PhisicalStorageSubset*) (((Content*) (this)) -> subset (id))); }
			PhisicalStorageSubset* subset (int id)
							{ PhisicalStorageSubsets::const_iterator i = _subsets.find (id); 
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
			PhisicalStorages _phisicalStorages;
			PhisicalStorageSubsets _subsets;
			MemoryViews _views;

			private:
			// Implementation
			mutable bool _error;
		};

		Memory () = delete;

		Memory (const Content& cnt);

		Memory (const Memory&) = default;

		Memory& operator = (const Memory&) = default;

		~Memory ();

		const PhisicalStorages& phisicalStorages () const
							{ return (_content.phisicalStorages ()); }
		bool existsPhisicalStorage (int id) const
							{ return (_content.existsPhisicalStorage (id)); } 
		const PhisicalStorage* phisicalStorage (int id) const
							{ return (_content.phisicalStorage (id)); }
		PhisicalStorage* phisicalStorage (int id)
							{ return (_content.phisicalStorage (id)); }

		const PhisicalStorageSubsets& Subsets () const
							{ return (_content.Subsets ()); }
		bool existsSubset (int id) const
							{ return (_content.existsSubset (id)); }
		const PhisicalStorageSubset* subset (int id) const
							{ return (_content.subset (id)); }
		PhisicalStorageSubset* subset (int id)
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

		/** It can be overloaded later, to set the specific content of specific zones. \n
			By default only subsets have to be initialized, and all of them become active and also active for reading. */
		virtual bool initialize ()
							{ return ((_lastError == MCHEmul::_NOERROR) ? _content.initialize () : false); }

		bool loadInto (const std::string& fN, const Address& a)
							{ return (_activeView -> loadInto (fN, a)); }

		/** To get the last error happend (usually at construction level) */
		unsigned int lastError () const
							{ return (_lastError); }
		void resetErrors ()
							{ _lastError = _NOERROR; }

		friend std::ostream& operator << (std::ostream& o, const Memory& m);

		protected:
		virtual Stack* lookForStack () = 0;
		virtual MemoryView* lookForCPUView () = 0;

		protected:
		Content _content;

		// Implementation
		MemoryView* _activeView;
		mutable Stack* _stack;
		mutable MemoryView* _cpuView;
		mutable unsigned int _lastError;
	};
}

#endif
  
// End of the file
/*@}*/