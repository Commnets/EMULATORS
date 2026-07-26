/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: AY38910.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 26/07/2026 \n
 *	Description: The AY-3-8910 chip with its MSX-specific general-purpose I/O wiring.
 *	Versions: 1.0 Initial
 */

#ifndef __MSX_AY38910__
#define __MSX_AY38910__

#include <CORE/incs.hpp>
#include <GENERALINSTRUMENTS/incs.hpp>
#include <array>

namespace MSX
{
	/** The MSX uses the AY-3-8910 I/O ports for the two general-purpose connectors,
		the keyboard layout signal, the cassette input and the Kana LED. */
	class AY38910 final : public GENERALINSTRUMENTS::AY38910
	{
		public:
		AY38910 (
			GENERALINSTRUMENTS::AY38910Registers* reg,
			GENERALINSTRUMENTS::AY38910LibWrapper* w);

		virtual void setRegister (
			unsigned char rId, const MCHEmul::UByte& v) override;

		virtual bool initialize () override;

		protected:
		virtual void processEvent (
			const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		private:
		void updateJoystickMovement (
			const MCHEmul::InputOSSystem::JoystickMovementEvent& jm);
		void updateJoystickButton (
			const MCHEmul::InputOSSystem::JoystickButtonEvent& jb);
		void refreshPortAInput ();

		private:
		/** Active-low direction and trigger state for the two MSX general-purpose ports. */
		std::array <MCHEmul::UByte, 2> _joystickStatus;
		/** The two non-joystick signals connected to bits 6 and 7 of PSG port A. */
		bool _JISKeyboard;
		bool _cassetteInput;
	};
}

#endif
  
// End of the file
/*@}*/
