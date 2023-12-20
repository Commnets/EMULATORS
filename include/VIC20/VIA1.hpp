/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: VIA1.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 17/12/2023 \n
 *	Description: The VIA1 Chip.
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_VIA1__
#define __VIC20_VIA1__

#include <CORE/incs.hpp>
#include <COMMODORE/incs.hpp>
#include <VIC20/VIA1Registers.hpp>

namespace VIC20
{
	class InputOSSystem;

	/** VIA1 is mainly to communicate VIC20 with external devices.
		Additionaly it is also used to read the keyboard and the joysticks. */
	class VIA1 final : public COMMODORE::VIA
	{
		public:
		friend InputOSSystem;

		static const unsigned int _ID = 200;

		VIA1 ()
			: VIA (_ID, VIA1Registers::_VIA1_SUBSET, F6500::IRQInterrupt::_ID),
			  _VIA1Registers (nullptr)
							{ setClassName ("VIA1"); }

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		private:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		private:
		VIC20::VIA1Registers* _VIA1Registers;
	};
}

#endif
  
// End of the file
/*@}*/
