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

	/** VIA1 register bank at $9110. \n
		PA2..PA5 receive joystick up/down/left/fire and PA6 cassette sense;
		PA0/PA1/PA7 retain their independently supplied external levels. \n
		PB follows the generic user-port behavior; reads do not replace its inputs. */
	class VIA1Registers final : public COMMODORE::VIARegisters
	{
		public:
		friend VIA1;
		friend VIA1VIA2RegistersReflection;

		static const int _VIA1_SUBSET = 2100;

		VIA1Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		/** Cached joystick state; publishing a new state requires the VIA port links to exist. */
		unsigned char joystickStatus ()
							{ return (_joystickStatus); }
		void setJoystickStatus (unsigned char js);

		/** Cached datasette key/sense state; its setter publishes through the connected PA. */
		bool tapeSensePressed () const
							{ return (_tapeSensePressed); }
		void setTapeSensePressed (bool v);

		private:
		/** Publish live joystick/tape inputs independently of input latching. */
		void refreshPortAInputs ();

		// Reset cached inputs even during construction; apply INITVIA presets only after wiring.
		virtual void initializeInternalValues () override;

		private:
		/** Status of the joystick. */
		unsigned char _joystickStatus;
		/** The status of the keys in the datasette. */
		bool _tapeSensePressed;
	};
}

#endif
  
// End of the file
/*@}*/
