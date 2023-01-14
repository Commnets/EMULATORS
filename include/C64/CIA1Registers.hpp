/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: CIARegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 15/05/2021 \n
 *	Description: C64 CIA 1 Registers Emultation
 *	Versions: 1.0 Initial
 */

#ifndef __C64_CIA1REGISTERS__
#define __C64_CIA1REGISTERS__

#include <CORE/incs.hpp>

namespace C64
{
	class CIA1;
	class CIATimer;
	class CIAClock;

	/** In the CIA1 Memory, there are a couple of records that behave different
		when they are read that when they are written. */
	class CIA1Registers final : public MCHEmul::ChipRegisters
	{
		public:
		friend CIA1;

		/** A name when this register is a subset in the main memory. */
		static const int _CIA1_SUBSET = 1002;

		CIA1Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		virtual size_t numberRegisters () const override
							{ return (0x10 /** = 32. */); }

		virtual void initialize () override;

		private:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		// Implementation
		void initializeInternalValues ();

		// This methods are invoked from CIA chip...
		/** The josystick 1 is very linked with the keyboard. */
		void setJoystick1InputPending (bool jp)
							{ _joystick1InputPending = jp; }
		/** These to manage the joystick 2... */
		unsigned char joystick2Status () const
							{ return (_joystick2Status); }
		void setJoystick2Status (unsigned char js)
							{ _joystick2Status = js; }

		/** and these ones are to manage the matrix of keys pressed...
			...and also the inputs comming from the joystick 1... */
		bool keyboardStatusMatrix (size_t r, size_t c) const
							{ return (_keyboardStatusMatrix [r].bit (c)); }
		const MCHEmul::UByte& keyboardStatusMatrix (size_t r) const
							{ return (_keyboardStatusMatrix [r]); }
		void setKeyboardStatusMatrix (size_t r, size_t c, bool s)
							{ _keyboardStatusMatrix [r].setBit (c, s); }
		void setKeyboardStatusMatrix (size_t r, const MCHEmul::UByte& u)
							{ _keyboardStatusMatrix [r] = u; }

		/** The registers will observe also the timers. */
		void lookAtTimers (CIATimer* tA, CIATimer* tB)
							{ _timerA = tA, _timerB = tB; }
		/** ...and also of the clock. */
		void lookAtClock (CIAClock* c)
							{ _clock = c; }

		private:
		/** Whether there is some input pending from the joystick. */
		bool _joystick1InputPending;
		/** The joystick 2 status. */
		unsigned char _joystick2Status;
		/** The data ports A y B are actually a matrix of info: ( bytes with (bytes each). 
			And it is used to know both the keyboard pressed and also the status of the jiystick 1. */
		MCHEmul::UByte _keyboardStatusMatrix [8];
		// Reference to the timers...
		CIATimer* _timerA;
		CIATimer* _timerB;
		// Reference to the clock...
		CIAClock* _clock;

		// Implementation
		unsigned char _keyboardRowToRead; // bit = 0, means row to be read!
		unsigned char _dataPortADir, _dataPortBDir;

		mutable MCHEmul::UByte _lastValueRead;
	};
}

#endif
  
// End of the file
/*@}*/
