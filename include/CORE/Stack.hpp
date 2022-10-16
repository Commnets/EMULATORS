/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: Stack.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
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
			  _stackOverflow (false),
			  _empty (true)
							{ setClassName ("Stack"); }

		size_t position () const
							{ return (_position); }
		/** Take care using this method. No checks are done on regards to the speed. 
			And it is not the same a back-stack than a foward one. */
		void setPosition (int p)
							{ _position = p; }

		virtual void initialize () override;

		void push (const UBytes& v);
		UBytes pull (size_t nV);

		/** It is set to true when something happens after push or pull actions. */
		bool stackOverflow () const
							{ return (_stackOverflow); }

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
		bool _stackOverflow;
		bool _empty;
	};
}

#endif
  
// End of the file
/*@}*/
