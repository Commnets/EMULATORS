/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: CPUHook.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 21/06/2024 \n
 *	Description: When CPU executes, sometime needs to simulate things reaching certain position...
 *				 This is what a CPUHook is for!
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_CPUHOOK__
#define __MCHEMUL_CPUHOOK__

#include <string>

namespace MCHEmul
{
	class ProgramCounter;
	class CPU;

	class CPUHook
	{
		public:
		// Just in case an extension needed it!
		virtual ~CPUHook () 
							{ }

		/** Receives a reference to the program counter 
			and returns it back maybe actualized!. \n
			If any other paremeter were required it would have to be managed specifically!. */
		virtual ProgramCounter& hook (CPU* cpu) = 0;

		/** To print out info of the hook if needed (Debugging). */
		virtual std::string& asString () const = 0;

		friend std::ostream& operator << (std::ostream& o, const CPUHook& h)
							{ return (o << h.asString ()); }
	};

	// To simplify a reference to a list of hooks!
	using CPUHooks = std::map <unsigned int, CPUHook*>;
}

#endif
  
// End of the file
/*@}*/

