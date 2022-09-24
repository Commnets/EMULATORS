/** \ingroup CPU */
/*@{*/

/**	
 *	@file	
 *	File: Memory.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 07/04/2021 \n
 *	Description: The memory managed by the machine.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_MEMORY__
#define __MCHEMUL_MEMORY__

#include <CORE/global.hpp>
#include <CORE/Address.hpp>
#include <CORE/UByte.hpp>
#include <CORE/UBytes.hpp>

namespace MCHEmul
{
	class Memory;
	class Stack;

	using Memories = std::map <int, Memory*>;

	/** The memory just keep UBytes, from a specific address onwards. \n
		It can be made of blocks. 
		A reference to the stack needs to be given. It can be either one of the blocks or not. */
	class Memory 
	{
		public:
		Memory (const Address& iA, size_t s, bool r = false, const Memories& blks = { }, Stack* stk = nullptr);

		virtual ~Memory ();

		const Address& initialAddress () const
							{ return (_initialAddress); }
		/** When using this method the whole size is taken (adding the size of the blocks behind). 
			Only internally (private) the pure size of the memory (with no blocks) can be accesed, using the variable _size. */
		size_t size () const;

		bool rom () const
							{ return (_rom); }

		const Memories& blocks () const
							{ return (_blocks); }
		bool existsBlock (int nB) const
							{ return (_blocks.find (nB) != _blocks.end ()); }
		const Memory* block (int nB) const
							{ return (existsBlock (nB) ? (*_blocks.find (nB)).second : nullptr); }
		Memory* block (int nB)
							{ return (existsBlock (nB) ? (*_blocks.find (nB)).second : nullptr); }

		const Stack* stack () const
							{ return (_stack); }
		Stack* stack ()
							{ return (_stack); }

		/** To "copy" the current values as the default ones. They will be put back when it is initialized. */
		void fixDefaultValues ();

		bool isIn (const Address& a) const;

		// These methods rely in setValue and readValue defined as virtual later
		// So, the way a final value can be read or set can be overload...
		const UByte value (const Address& a) const;
		void set (const Address& a, const UByte v, bool f = false /** To force even when it is a rom. */);
		const UBytes values (const Address& a, size_t nB) const;
		void set (const Address& a, const UBytes& v, bool f = false);

		/** It can be overloaded latr, to set the specific content of specific zones. */
		virtual bool initialize ();

		/** First bytes with the address. */
		bool load (const std::string& fN);
		/** Everything into the right memory. */
		bool loadInto (const std::string& fN, const Address& a);
		/** The address where to load things into is the initial address of the memory. */
		bool loadInto (const std::string& fN)
							{ return (loadInto (fN, initialAddress ())); }

		/** The memory can be active o not. */
		bool active () const
							{ return (_active); }
		void setActive (bool e)
							{ _active = e; }

		friend std::ostream& operator << (std::ostream& o, const Memory& m);

		protected:
		/** Sometimes some specific memory addresses can behave different 
			when read or written than the usual way of doing so. */
		virtual void setValue (size_t p, const UByte& u)
							{ _values [p] = u; }
		virtual UByte readValue (size_t p) const
							{ return (_values [p]); }

		protected:
		Address _initialAddress;
		size_t _size;
		UByte* _values;
		bool _rom;
		Memories _blocks;
		Stack* _stack;

		bool _active;

		// Implementation
		UByte* _defaultValues;
	};
}

#endif
  
// End of the file
/*@}*/