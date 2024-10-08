/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: VIA1Registers.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 17/12/2023 \n
 *	Description: VIC20 VIA 1 Registers Emulation.
 *	Versions: 1.0 Initial.
 */

#ifndef __VIC20_VIA1REGISTERS__
#define __VIC20_VIA1REGISTERS__

#include <COMMODORE/incs.hpp>

namespace VIC20
{
	class VIA1;
	class VIA1VIA2RegistersReflection;

	/** Just to identify this space of the memory in a different section. 
		VIA2Registers is aimed to take care of the information about keyboard. */
	class VIA1Registers final : public COMMODORE::VIARegisters
	{
		public:
		friend VIA1;
		friend VIA1VIA2RegistersReflection;

		static const int _VIA1_SUBSET = 2100;

		VIA1Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		/** To manage the status of the joystick. */
		unsigned char joystickStatus ()
							{ return (_joystickStatus); }
		void setJoystickStatus (unsigned char js)
							{ _joystickStatus = js; }

		private:
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		// Implementation
		virtual void initializeInternalValues () override;

		private:
		/** Status of the joystick. */
		unsigned char _joystickStatus;
	};
}

#endif
  
// End of the file
/*@}*/
