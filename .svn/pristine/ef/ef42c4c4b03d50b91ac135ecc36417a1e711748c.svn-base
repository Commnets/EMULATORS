/** \ingroup CPU */
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
	class Stack : public PhisicalStorageSubset
	{
		public:
		/**
		  * Constructor:
		  * @param b	: From the end of the memory to the beggining or the other way around
		  * @param e	: Poining always to the empty place or pointing to the last position kept.
		  */
		Stack (int id, PhisicalStorage* ps, size_t pp, const Address& iA, size_t s, bool b = true, bool e = true)
			: PhisicalStorageSubset (id, ps, pp, iA, s), 
			  _position (0), _fromBack (b), _pointToEmpty (e),
			  _stackOverflow (false),
			  _empty (true)
							{ }

		constexpr size_t position () const
							{ return (_position); }

		virtual void initialize () override;

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
