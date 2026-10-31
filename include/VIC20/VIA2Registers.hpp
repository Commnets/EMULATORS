/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: VIA2Registers.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 17/12/2023 \n
 *	Description: VIC20 VIA 2 Registers Emulation
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_VIA2REGISTERS__
#define __VIC20_VIA2REGISTERS__

#include <COMMODORE/incs.hpp>

namespace VIC20
{
	class VIA2;
	class VIA1VIA2RegistersReflection;

	/** VIA2 connects keyboard rows to PA and columns to PB. \n
		The joystick right switch shares PB7. \n
		The digital keyboard model propagates low levels through pressed switches
		in both directions while configured outputs retain drive priority. \n
		A zero in a keyboard matrix bit represents a pressed switch. */
	class VIA2Registers final : public COMMODORE::VIARegisters
	{
		public:
		friend VIA2;
		friend VIA1VIA2RegistersReflection;

		/** A name when this register is a subset in the main memory. */
		static const int _VIA2_SUBSET = 2110;

		VIA2Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		/** To manage the status of the joystick. */
		unsigned char joystickStatus ()
							{ return (_joystickStatus); }
		void setJoystickStatus (unsigned char js)
							{ _joystickStatus = js; }
		/** Cached switch matrix and its transposed view; zero means a pressed key. */
		bool keyboardStatusMatrix (size_t r, size_t c) const
							{ return (_keyboardStatusMatrix [r].bit (c)); }
		const MCHEmul::UByte& keyboardStatusMatrix (size_t r) const
							{ return (_keyboardStatusMatrix [r]); }
		bool rev_keyboardStatusMatrix (size_t r, size_t c) const
							{ return (_rev_keyboardStatusMatrix [r].bit (c)); }
		const MCHEmul::UByte& rev_keyboardStatusMatrix (size_t r) const
							{ return (_rev_keyboardStatusMatrix [r]); }
		void setKeyboardStatusMatrix (size_t r, size_t c, bool s)
							{ _keyboardStatusMatrix [r].setBit (c, s);
							  _rev_keyboardStatusMatrix [c].setBit (r, s); }

		private:
		virtual const MCHEmul::UByte& readValue (size_t p) const override;
		/** Avoids acknowledgement/handshake but still resolves and publishes keyboard pin levels. */
		virtual const MCHEmul::UByte& peekValue (size_t p) const override;

		/** Settles the sixteen-pin digital network in at most sixteen computations, without CPU cycles. \n
			Called for port reads/peeks and initialization; it is not an independently clocked scan. \n
			Outputs retain priority; analog contention is outside this model. */
		void refreshKeyboardInputs () const;

		// Reset cached inputs even during construction; apply keyboard presets only after wiring.
		virtual void initializeInternalValues () override;

		private:
		/** Status of the joystick. */
		unsigned char _joystickStatus;
		/** Column-to-row switch states; zero means pressed. */
		MCHEmul::UByte _keyboardStatusMatrix [8];
		/** Row-to-column view of the same switches. */
		MCHEmul::UByte _rev_keyboardStatusMatrix [8];
	};
}

#endif
  
// End of the file
/*@}*/
