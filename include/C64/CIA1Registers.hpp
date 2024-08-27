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
					C0	C1	C2	C3	C4	C5	C6	C7				\n
					|	|	|	|	|	|	|	|				\n
			  R0----+---+---+---+---+---+---+---+----C			\n
			    	|	|	|	|	|	|	|	|	 o			\n
			  R1----+---+---+---+---+---+---+---+----n			\n
			    	|	|	|	|	|	|	|	|	 n			\n
			  R2----+---+---+---+---+---+---+---+----e			\n
			    	|	|	|	|	|	|	|	|	 c			\n
			  R3----+---+---+---+---+---+---+---+----t			\n
			    	|	|	|	|	|	|	|	|	 e			\n
			  R4----+---+---+---+---+---+---+---+----d			\n
			    	|	|	|	|	|	|	|	|	 			\n
			  R5----+---+---+---+---+---+---+---+----P			\n
			    	|	|	|	|	|	|	|	|	 o			\n
			  R6----+---+---+---+---+---+---+---+----r			\n
			    	|	|	|	|	|	|	|	|	 t			\n
			  R7----+---+---+---+---+---+---+---+----B			\n
					|	|	|	|	|	|	|	|				\n
					Connected PortA								\n
		\n
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
		\n
		The CIA1 can manage either joysticks or paddles, connected wither to the game port 1. \n
		By default it is configured to manage joysticks and "understand" the events comming from them. \n
		In a single game port (let's say game port 1, tracked in the register $01 = $dc01) there might be connected two paddles. \n
		So a total of 4 paddles can be simultaneosly overseen from the CIA1. \n
		But CIA1 only oversees the situation of the buttons of those paddles. \n
		The position is tracked from the SID register, throught two pins (@see SID anyway): POTX (pin 21) and POTY (pin 22).
		However 2 pins for 2 paddles. sp how to track the position of the 4? It is not possible simultaneously!
		To select the game port to read the register $00 (= $dc00) of the CIA1 is used. \n
		The bits 6 and 7 are used to determine that (00 = nothing to read, 10 = read paddle position on port 1, 01 = on port 2). \n
		Paddles are simulated with joysticks as well. \n
		So there must be possible to tell the CIA which type of device to understand.
		*/
	class CIA1Registers final : public COMMODORE::CIARegisters
	{
		public:
		friend CIA1;

		/** A name when this register is a subset in the main memory. */
		static const int _CIA1_SUBSET = 2000;

		CIA1Registers (MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		/** The CIA has a link with the SID,
			as in register 0 is possible to tell SID the paddle position to read. 
			The method is used when C64 is initilized. */
		void linkToSID (COMMODORE::SID* sid);

		// This methods are invoked from CIA chip...
		/** The boundaries are not controlled at all, 
			so be cerefull when using these methods. */
		unsigned char joystickStatusAtPort (size_t p) const
							{ return (_joystickStatus [p]); }
		void setJoystickStatusAtPort (size_t p, unsigned char js)
							{ _joystickStatus [p] = js; }

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

		/** To know whether there is or not a paddle connected,
			When there is, the port will be managed in that sense, when there is not as joystick. 
			The parameter is the number of the port (either 0 or 1).
			No boundaries checks are, so take care when using any of the following methods. */
		bool isPaddleConnectedAtPort (size_t p) const
							{ return (_paddleConnected [p]); } // No boundary checks are done...
		bool arePaddlesConnected () const
							{ return (_paddleConnected [0] || _paddleConnected [1]); }
		void connectPaddleAtPort (size_t p)
							{ _paddleConnected [p] = true; }
		void connectAllPaddles () // One in each port...
							{ _paddleConnected [0] = _paddleConnected [1] = true; }
		inline void disconnectPaddle (size_t p);
		inline void disconnectAllPaddles ();
		/** In CIA1 only the status of the fire buttons are checked.
			Status of the fire buttons of the paddles.
			No bounday check is done, so be careful when using them. 
			No checked either whether the paddle is or not active. */
		void setPaddleFireButtonStatus (size_t p, size_t bt, bool st)
							{ _paddleFireButtonStatus [p][bt] = st; }
		bool paddleFireButtonStatus (size_t p, size_t bt) const
							{ return (_paddleFireButtonStatus [p][bt]); }

		private:
		/** To manage to activate which paddle position (if any) the SID would have to read. @see SID. */
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		/** To read the status of the keyboard, joysticks and paddles. */
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		// Implementation
		virtual void initializeInternalValues () override;

		private:
		/** The SID linked. */
		COMMODORE::SID* _sid;
		/** The joystick status /** two ports. */
		unsigned char _joystickStatus [2];
		/** The data ports A y B are actually a matrix of info: ( bytes with (bytes each). 
			And it is used to know both the keyboard pressed and also the status of the jiystick 1. */
		MCHEmul::UByte _keyboardStatusMatrix [8];
		/** The opposite. */
		MCHEmul::UByte _rev_keyboardStatusMatrix [8];
		/** The paddel connected if any in every port.
			When boths are 0, no paddle connected, which is the value by default. */
		bool _paddleConnected [2];
		/** The status of the fire buttons. */
		bool _paddleFireButtonStatus [2][2];
	};

	// ---
	inline void CIA1Registers::disconnectPaddle (size_t p)
	{ 
		_paddleConnected [p] = false; 
		_paddleFireButtonStatus [p][0] = _paddleFireButtonStatus [p][1] = false; 
	}

	// ---
	inline void CIA1Registers::disconnectAllPaddles ()
	{
		_paddleConnected [0] = _paddleConnected [1] = false;
		_paddleFireButtonStatus [0][0] = _paddleFireButtonStatus [0][1] = 
		_paddleFireButtonStatus [1][0] = _paddleFireButtonStatus [1][1] = false; // Quicker than a loop...
	}
}

#endif
  
// End of the file
/*@}*/
