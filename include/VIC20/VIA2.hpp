/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: VIA2.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 17/12/2023 \n
 *	Description: The VIA2 Chip within the VIC20.
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_VIA2__
#define __VIC20_VIA2__

#include <CORE/incs.hpp>
#include <COMMODORE/incs.hpp>
#include <VIC20/VIA2Registers.hpp>

namespace VIC20
{
	/** The chip is to communicate the VIC20 with the environment. */
	class VIA2 final : public COMMODORE::VIA
	{
		public:
		static const unsigned int _ID = 201;

		VIA2 ()
			: VIA (_ID, VIA2Registers::_VIA2_SUBSET, F6500::NMIInterrupt::_ID),
			  _VIA2Registers (nullptr)
							{ setClassName ("VIA2"); }

		virtual bool initialize () override;

		private:
		VIA2Registers* _VIA2Registers;
	};
}

#endif
  
// End of the file
/*@}*/
