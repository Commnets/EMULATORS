/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: CIA1.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 22/05/2021 \n
 *	Description: The CIA1 Chip.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_CIA1__
#define __C64_CIA1__

#include <CORE/incs.hpp>
#include <COMMODORE/incs.hpp>
#include <C64/CIA1Registers.hpp>

namespace C64
{
	class InputOSSystem;

	/** CIA1 is mainly to communicate C64 with external devices.
		Additionaly it is also used to read the keyboard and the joysticks. */
	class CIA1 final : public COMMODORE::CIA
	{
		public:
		friend InputOSSystem;

		static const unsigned int _ID = 200;

		CIA1 ()
			: CIA (_ID, CIA1Registers::_CIA1_SUBSET, F6500::IRQInterrupt::_ID),
			  _CIA1Registers (nullptr)
							{ setClassName ("CIA1"); }

		virtual bool initialize () override;

		private:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		private:
		C64::CIA1Registers* _CIA1Registers;
	};
}

#endif
  
// End of the file
/*@}*/
