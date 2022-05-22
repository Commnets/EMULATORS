/** \ingroup core */
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

#include <core/Memory.hpp>

namespace MCHEmul
{
	class Stack : public Memory
	{
		public:
		/**
		  * Constructor:
		  * @param b	: From the end of the memory to the beggining or the other way around
		  * @param e	: Poining always to the empty place or pointing to the last position kept.
		  */
		Stack (const Address& iA, size_t l, bool b = true, bool e = true)
			: Memory (iA, l, { } /** no blocks inside */), 
			  _position (0), _fromBack (b), _pointToEmpty (e),
			  _stackOverflow (false),
			  _empty (true)
							{ assert (size () > 0) /* It has to have size */; }

		constexpr size_t position () const
							{ return (_position); }

		virtual bool initialize () override;

		void push (const UBytes& v);
		UBytes pull (size_t nV);

		/** It is set to true when something happens after push or pull actions. */
		constexpr bool stackOverflow () const
							{ return (_stackOverflow); }

		friend std::ostream& operator << (std::ostream& o, const Stack& s);

		private:
		int _position;
		const bool _fromBack = true; // Adapted at construction time
		const bool _pointToEmpty = true; // Adàted at construction time

		// Implementation
		bool _stackOverflow;
		bool _empty;
	};
}

#endif
  
// End of the file
/*@}*/
