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

		virtual bool initialize () override;

		private:
		// Implementation
		static MCHEmul::Chips standardChips (VisualSystem vS);
		static MCHEmul::IODevices standardDevices (VisualSystem vS);

		private:
		VisualSystem _visualSystem;
	};
}

#endif
  
// End of the file
/*@}*/
