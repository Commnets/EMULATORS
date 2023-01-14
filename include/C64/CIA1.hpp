/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: CIA.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 22/05/2021 \n
 *	Description: The CIA Chips.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_CIA1__
#define __C64_CIA1__

#include <CORE/incs.hpp>
#include <C64/CIA1Registers.hpp>
#include <C64/CIATimer.hpp>
#include <C64/CIAClock.hpp>
#include <F6500/IRQInterrupt.hpp>

namespace C64
{
	class InputOSSystem;

	/** CIA1 is mainly to communicate C64 with external devices.
		Additionaly it is also used to read the keyboard and the joysticks. */
	class CIA1 : public MCHEmul::Chip
	{
		public:
		friend InputOSSystem;

		static const unsigned int _ID = 102;

		CIA1 ()
			: MCHEmul::Chip (_ID,
				{ { "Name", "CIA1" },
				  { "Code", "6526/6526A/8521" },
				  { "Manufacturer", "Commodore Business Machines CBM" },
				  { "Year", "1980" } }),
			  _CIA1Registers (nullptr),
			  _timerA (0, F6500::IRQInterrupt::_ID), _timerB (1, F6500::IRQInterrupt::_ID), 
			  _clock (0, F6500::IRQInterrupt::_ID)
							{ setClassName ("CIA1"); }

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * The ones comming from the parent class. \n
		  * Registers	= InfoStructure: Value of the registers. \n
		  *	TimerA		= InfoStructure: Info about the timer A. \n
		  *	TimerB		= InfoStructure: Info about the timer B. \n
		  *	Clock		= InfoStructure: Info about the clock. \n
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		// These methods are invoked from InputOSSystem...	
		// To manage the joystick
		void setJoystick1InputPending (bool jP)
							{ _CIA1Registers -> setJoystick1InputPending (jP); }
		unsigned char joystick2Status () const
							{ return (_CIA1Registers -> joystick2Status ()); }
		void setJoystick2Status (unsigned char js)
							{ _CIA1Registers -> setJoystick2Status (js); }

		// To manage status of the data ports
		bool keyboardStatusMatrix (size_t r, size_t c) const
							{ return (_CIA1Registers -> keyboardStatusMatrix (r, c)); }
		const MCHEmul::UByte& keyboardStatusMatrix (size_t r) const
							{ return (_CIA1Registers -> keyboardStatusMatrix (r)); }
		void setKeyboardStatusMatrix (size_t r, size_t c, bool s)
							{ _CIA1Registers -> setKeyboardStatusMatrix (r, c, s); }
		void setKeyboardStatusMatrix (size_t r, const MCHEmul::UByte& u)
							{ _CIA1Registers -> setKeyboardStatusMatrix (r, u); }

		private:
		C64::CIA1Registers* _CIA1Registers;
		/** The very important timers. */
		CIATimer _timerA, _timerB;
		CIAClock _clock;
	};
}

#endif
  
// End of the file
/*@}*/
