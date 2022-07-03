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

#include <CORE/incs.hpp>
#include <C64/Screen.hpp>

namespace C64
{
	class Commodore64 final : public MCHEmul::Computer
	{
		public:
		enum class VisualSystem { _NTSC, _PAL };

		Commodore64 (VisualSystem vS = VisualSystem::_PAL /** Europe initially */);

		static const int _DDR_MEMORY		= 0;
		static const int _IOP_MEMORY		= 1;
		static const int _PAGEZERO_MEMORY	= 2;
		static const int _STACK_MEMORY		= 3;
		static const int _RAM0_MEMORY		= 4;
		static const int _BASIC_MEMORY		= 5;
		static const int _RAM1_MEMORY		= 6;
		static const int _VICREGS_MEMORY	= 7;
		static const int _SIDREGS_MEMORY	= 8;
		static const int _COLOR_MEMORY		= 9;
		static const int _CIA1_MEMORY		= 10;
		static const int _CIA2_MEMORY		= 11;
		static const int _IO1_MEMORY		= 12;
		static const int _IO2_MEMORY		= 13;
		static const int _CHAROM_MEMORY		= 14;
		static const int _KERNELROM_MEMORY	= 15;

		// Implementation
		static MCHEmul::Chips standardChips (VisualSystem vS);
		static MCHEmul::Memory* standardMemory ();
		static MCHEmul::IODevices standardDevices (VisualSystem vS);

		private:
		VisualSystem _visualSystem;
	};
}

#endif
  
// End of the file
/*@}*/
