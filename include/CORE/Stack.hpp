/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: Stack.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 11/04/2021 \n
 *	Description: Defines the behviour of the stack, that it is a special type of memory.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_STACK__
#define __MCHEMUL_STACK__

#include <CORE/Memory.hpp>

namespace MCHEmul
{
	class Stack final : public PhysicalStorageSubset
	{
		public:
		/**
		  * Constructor:
		  * The ones needed by the parent class.
		  * @param b	: From the end of the memory to the beggining or the other way around
		  * @param e	: Poining always to the empty place or pointing to the last position kept.
		  */
		Stack (int id, PhysicalStorage* ps, size_t pp, const Address& iA, size_t s, bool b = true, bool e = true)
			: PhysicalStorageSubset (id, ps, pp, iA, s), 
			  _position (0), _fromBack (b), _pointToEmpty (e),
			  _overflow (false),
			  _notUsed (true)
							{ setClassName ("Stack"); }

		int position () const
							{ return (_position); }
		/** Take care using this method. No checks are done on regards to the speed. 
			And it is not the same a back-stack than a foward one. \n
			The r parameter is very important:
			In some processor the stack can be located in any place in the memory 
			and there are instructions to set where the stack pointer is pointing to. 
			In that computers the value p received will be absolute and the r must be FALSE. 
			This is because _position is always managed internally as a relative value. */
		void setPosition (int p, bool r = true)
							{ _position = (r) ? p : p - initialAddress ().value (); }
		/** To move to the beginning. */
		void reset ();

		virtual void initialize () override;

		/** To manage the stack. */
		void push (const UBytes& v); // When overflow no more is entered...
		UBytes pull (size_t nV); // When overflow am empty object is returned...

		/** It is true when the stack is empty. */
		bool empty () const
							{ return (_notUsed ? true 
								: (_fromBack ? (_position == (int) (size () - 1)) : (_position == 0))); }
		/** It is set to true when something happens after push or pull actions. */
		bool overflow () const
							{ return (_overflow); }
		/** To simplify the way the stack status is enquired. */
		bool operator ! () const
							{ return (_overflow); }

		/** To know whether it has ben already used or not. */
		bool notUsed () const
							{ return (_notUsed); }
		/** This is a very rare method. Use carefully. \n
			The default behaviour of a non-pointing-to-empty stack when it hasn't been used ever,
			but this behaviour is not the good one in some implementations, like Z80. */
		void setNotUsed (bool nU)
							{ _notUsed = nU; }

		/**
		  *	The name of the fields are: \n
		  * MEMORY		= InfoStructure: Information about the memory itself. \n
		  *	BACK		= Attribute: YES when moves from back to the top and NO in the other circunstance. \n
		  *	LAST		= Attribute: YES when the empty space is pointing to the last data. \n
		  *	OVERFLOW	= Attribute: YES when no more elements are admitted. \n
		  *	EMPTY		= Attribute: YES when there is no elements inside. \n
		  *	POSITION	= Attribute: Current position in the stack.
		  */
		virtual InfoStructure getInfoStructure () const override;

		private:
		int _position;
		const bool _fromBack = true; // Adapted at construction time
		const bool _pointToEmpty = true; // Adapted at construction time

		// Implementation
		bool _overflow;
		bool _notUsed;
	};
}

#endif
  
// End of the file
/*@}*/
