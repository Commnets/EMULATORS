/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: VIA2Registers.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 17/12/2023 \n
 *	Description: VCI20 VIA 1 Registers Emulation
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_VIA2REGISTERS__
#define __VIC20_VIA2REGISTERS__

#include <COMMODORE/incs.hpp>

namespace VIC20
{
	class VIA2;
	class VIA1VIA2RegistersReflection;

	/** This specific type of port is connected
		to the different elements of OSIO of the VICII: Keyboard and Joystick. \n
		The keyboard matrix is a matrix of 8x8 "cables" not connected to anything in one extreme.
		and to the the two ports of the VIA in the other. \n
		Any of the 64 keys of the keyboard is a switch located 
		in each of the intersection of those 2 groups of cables. \n
					C0	C1	C2	C3	C4	C5	C6	C7	
					|	|	|	|	|	|	|	|     
			  R0----+---+---+---+---+---+---+---+----C
			    	|	|	|	|	|	|	|	|	 o
			  R1----+---+---+---+---+---+---+---+----n
			    	|	|	|	|	|	|	|	|	 n
			  R2----+---+---+---+---+---+---+---+----e
			    	|	|	|	|	|	|	|	|	 c
			  R3----+---+---+---+---+---+---+---+----t
			    	|	|	|	|	|	|	|	|	 e
			  R4----+---+---+---+---+---+---+---+----d
			    	|	|	|	|	|	|	|	|	 
			  R5----+---+---+---+---+---+---+---+----P
			    	|	|	|	|	|	|	|	|	 o
			  R6----+---+---+---+---+---+---+---+----r
			    	|	|	|	|	|	|	|	|	 t
			  R7----+---+---+---+---+---+---+---+----B
					|	|	|	|	|	|	|	|	
					Connected PortA
		The switch only allow the flow of the current in one way (using a diode). \n
		When a key is pressed the status of one line is transmited to the other. \n
		The diode allows to transmite the status of the Column to the Row.
		The default status of the lines, when read is, according with VIA datasheet 1.
		So to read a key the status of a the column is set to 0, and then is read the status of the row, 
		when a 0 appears the key has been pressed. \n
		Additionally the joystick is connected to the port A. \n
	  */
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
		/** and these ones are to manage the matrix of keys pressed...
			...and also the inputs comming from the joystick 1... */
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

		// Implementation
		virtual void initializeInternalValues () override;

		private:
		/** Status of the joystick. */
		unsigned char _joystickStatus;
		/** 0 at the bit representing the part of the keyboard matrix pressed. */
		MCHEmul::UByte _keyboardStatusMatrix [8];
		MCHEmul::UByte _rev_keyboardStatusMatrix [8];
	};
}

#endif
  
// End of the file
/*@}*/
