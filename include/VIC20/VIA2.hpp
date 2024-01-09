/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: VIA2.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 17/12/2023 \n
 *	Description: The VIA2 Chip. This chip read the keyboard and the main joystick!
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_VIA2__
#define __VIC20_VIA2__

#include <CORE/incs.hpp>
#include <COMMODORE/incs.hpp>
#include <VIC20/VIA2Registers.hpp>

namespace VIC20
{
	class InputOSSystem;

	/** VIA2 is mainly to communicate VIC20 with external devices.
		Additionaly it is also used to read the keyboard and the joysticks. */
	class VIA2 final : public COMMODORE::VIA
	{
		public:
		friend InputOSSystem;

		static const unsigned int _ID = 211;

		VIA2 ()
			: VIA (_ID, VIA2Registers::_VIA2_SUBSET, F6500::IRQInterrupt::_ID),
			  _VIA2Registers (nullptr)
							{ setClassName ("VIA2"); }

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		private:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		private:
		VIC20::VIA2Registers* _VIA2Registers;
	};
}

#endif
  
// End of the file
/*@}*/
