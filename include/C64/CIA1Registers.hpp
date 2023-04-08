/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: CIA1Registers.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 15/05/2021 \n
 *	Description: C64 CIA 1 Registers Emulation
 *	Versions: 1.0 Initial
 */

#ifndef __C64_CIA1REGISTERS__
#define __C64_CIA1REGISTERS__

#include <COMMODORE/incs.hpp>

namespace C64
{
	class CIA1;

	/** In the CIA1 Memory, there are a couple of records that behave different
		when they are read that when they are written. \n
		In CIA1 the portA and the portB are connected to the keyboard matrix. \n
		Read this document to know how the keyboard works: \n
		https://c64os.com/post/howthekeyboardworks
		*/
	class CIA1Registers final : public COMMODORE::CIARegisters
	{
		public:
		friend CIA1;

		/** A name when this register is a subset in the main memory. */
		static const int _CIA1_SUBSET = 2000;

		CIA1Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		// This methods are invoked from CIA chip...
		unsigned char joystick1Status () const
							{ return (_joystick1Status); }
		void setJoystick1Status (unsigned char js)
							{ _joystick1Status = js; }
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

		private:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		// Implementation
		virtual void initializeInternalValues () override;

		private:
		/** The joystick 1 status. */
		unsigned char _joystick1Status;
		/** The joystick 2 status. */
		unsigned char _joystick2Status;
		/** The data ports A y B are actually a matrix of info: ( bytes with (bytes each). 
			And it is used to know both the keyboard pressed and also the status of the jiystick 1. */
		MCHEmul::UByte _keyboardStatusMatrix [8];

		// Implementation
		unsigned char _keyboardRowToRead; // bit = 0, means row selected to be read!
	};
}

#endif
  
// End of the file
/*@}*/
