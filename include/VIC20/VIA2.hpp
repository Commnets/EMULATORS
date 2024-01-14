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
	class VIA2 final : public COMMODORE::VIA
	{
		public:
		friend InputOSSystem;

		static const unsigned int _ID = 211;

		VIA2 ();

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

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		private:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		private:
		/** Status of the joystick. */
		unsigned char _joystickStatus;
		/** 0 at the bit representing the part of the keyboard matrix pressed. */
		MCHEmul::UByte _keyboardStatusMatrix [8];
		MCHEmul::UByte _rev_keyboardStatusMatrix [8];

		VIC20::VIA2Registers* _VIA2Registers;
	};
}

#endif
  
// End of the file
/*@}*/
