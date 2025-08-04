/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: VIA1.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 17/12/2023 \n
 *	Description: The VIA1 Chip within the VIC20.
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_VIA1__
#define __VIC20_VIA1__

#include <CORE/incs.hpp>
#include <COMMODORE/incs.hpp>
#include <VIC20/VIA1Registers.hpp>

namespace VIC20
{
	/** VIA1 in the VIC20,
		takes care of the Serial Port, User Port, 
		Game Port and Datasette Port (key pressed and motor) */
	class VIA1 final : public COMMODORE::VIA
	{
		public:
		static const unsigned int _ID = 210;

		VIA1 ()
			: VIA (_ID, VIA1Registers::_VIA1_SUBSET, F6500::NMIInterrupt::_ID),
			  _VIA1Registers (nullptr)
							{ setClassName ("VIA1"); }

		virtual bool initialize () override;

		private:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		private:
		VIA1Registers* _VIA1Registers;
	};
}

#endif
  
// End of the file
/*@}*/
