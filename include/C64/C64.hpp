/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: C64.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/04/2021 \n
 *	Description: The global structure defining a c64 machine.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_C64__
#define __C64_C64__

#include <core/Computer.hpp>

namespace C64
{
	class Commodore64 : public MCHEmul::Computer
	{
		public:
		Commodore64 ();

		static const int _DDR_MEMORY		= 0;
		static const int _IOP_MEMORY		= 1;
		static const int _PAGEZERO_MEMORY	= 2;
		static const int _STACK_MEMORY		= 3;
		static const int _RAM0_MEMORY		= 4;
		static const int _BASIC_MEMORY		= 5;
		static const int _RAM1_MEMORY		= 6;
		static const int _IO_MEMORY			= 7;
		static const int _ROM_MEMORY		= 8;

		private:
		// Implementation
		static MCHEmul::Chips standardChips ();
		static MCHEmul::Memory* standardMemory ();
	};
}

#endif
  
// End of the file
/*@}*/
