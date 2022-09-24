/** \ingroup core */
/*@{*/

/**	
 *	@file	
 *	File: Stack.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/04/2021 \n
 *	Description: Defines the bhviour of the stack.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_STACK__
#define __MCHEMUL_STACK__

#include <ostream>

#include <core/Memory.hpp>

namespace MCHEmul
{
	class Stack : public Memory
	{
		public:
		Stack (const Address& iA, size_t l)
			: Memory (iA, l, { } /** no blocks inside */), 
			  _position (0),
			  _stackOverflow (false)
							{ }

		constexpr size_t position () const
							{ return (_position); }

		void initialize ()
							{ Memory::initialize (); _position = 0; _stackOverflow = false; }

		void push (const UBytes& v);
		UBytes pull (size_t nV);

		/** It is set to true when something happens after push or pull actions. */
		constexpr bool stackOverflow () const
							{ return (_stackOverflow); }

		friend std::ostream& operator << (std::ostream& o, const Stack& s);

		private:
		size_t _position;

		// Implementation
		bool _stackOverflow;
	};
}

#endif
  
// End of the file
/*@}*/
