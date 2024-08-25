/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: CIA1Registers.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
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
		The keyboard matrix is a matrix of 8x8 "cables" not connected to anything in one extreme.
		and to the the ports of the CIA in the other. \n
		Any of the 64 keys of the keyboard is a switch located in each of the intersection of those 2 groups of cables. \n
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
		The default status of the lines, when read is, according with CIA datasheet 1.
		So to read a key the status of a the column is set to 0, and then is read the status of the row, 
		when a 0 appears the key has been pressed. \n
		Additionally the joystick 2 is connected to the port A and the joystick 1 to the port B. \n
		As the port B is where the info is read, the status created by it can be confused with keys,
		which is not the case for joystick 2.
		https://c64os.com/post/howthekeyboardworks
		The CIA1 also controls the paddles that are read, and in that case the way the 
		info is read from the data ports of the CIA 1 is different, as only the fire button of the paddle would affect.
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
		bool rev_keyboardStatusMatrix (size_t r, size_t c) const
							{ return (_rev_keyboardStatusMatrix [c].bit (r)); }
		const MCHEmul::UByte& rev_keyboardStatusMatrix (size_t c) const
							{ return (_rev_keyboardStatusMatrix [c]); }
		void setKeyboardStatusMatrix (size_t r, size_t c, bool s)
							{ _keyboardStatusMatrix [r].setBit (c, s);
							  _rev_keyboardStatusMatrix [c].setBit (r, s); }

		/** The way the paddles are managed.
			There can be 2 paddles connecter per game port, 
			that produces 2 positions (continuous, from 0 - 255) in 2 SID registers. \n
			In CIA1 there is selected the paddles to read at SID level. */
		bool isPaddleConnected () const
							{ return (_paddleConnected != 0); }
		int paddleConnected () const
							{ return (_paddleConnected); }
		/** = 0 means none, and common joysticks instead. 
			  The value can be either 0, 1 or 2. */
		void setPaddleConnected (int p)
							{ if (p == 0 || p == 1 || p == 2) _paddleConnected = p; }
		/** Status of the fire buttons of the paddles.
			No bounday check is done, so be careful when using them. */
		void setPaddleFireButtonStatus (size_t p, size_t bt, bool st)
							{ _paddleFireButtonStatus [p][bt] = st; }
		bool paddleFireButtonStatus (size_t p, size_t bt) const
							{ return (_paddleFireButtonStatus [p][bt]); }

		private:
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
		/** The opposite. */
		MCHEmul::UByte _rev_keyboardStatusMatrix [8];
		/** The paddel connected if any. 
			When 0 no paddle connected and joystick instead. */
		int _paddleConnected;
		/** The status of the fire buttons. */
		bool _paddleFireButtonStatus [2][2];
	};
}

#endif
  
// End of the file
/*@}*/
