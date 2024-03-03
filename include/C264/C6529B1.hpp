/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: C6529B1.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 25/02/2024 \n
 *	Description: Specific implementation of the 6529 chip in C264.
 *				 In C264 series, this chip acts as a latch of the input to the keyboard matrix. \n
 *				 The joysticks have nothing to do with this register,
 *				 but to simplify the code that behaviour has also been grouped into this class. \n
 *	Versions: 1.0 Initial
 */

#ifndef __C264_C6529B1__
#define __C264_C6529B1__

#include <COMMODORE/incs.hpp>
#include <C264/C6529B1Registers.hpp>

namespace C264
{
	/** The important thing of this chip is just their registers (@see C6529Registers). */
	class C6529B1 final : public COMMODORE::C6529B
	{
		public:
		static const int _ID = 200;

		// Some events
		static const unsigned int _JOYSTICK1VALUECHANGED = 300;
		static const unsigned int _JOYSTICK2VALUECHANGED = 301;
		
		C6529B1 ()
			: COMMODORE::C6529B (_ID),
			  _C6529B1Registers (nullptr),
			  _joyValueChanged { false, false }
							{ initializeKeyboardAndJoystickStatus (); }
		
		virtual bool initialize () override;

		/** The simulation consists only on notifying the new value of the keyboard matrix, if it changed. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;
		
		private:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		// Implementation
		/** and these ones are to manage the matrix of keys pressed...
			...and also the inputs comming from the joystick 1... */
		bool keyboardStatusMatrix (size_t r, size_t c) const
							{ return (_keyboardStatusMatrix [r].bit (c)); }
		const MCHEmul::UByte& keyboardStatusMatrix (size_t r) const
							{ return (_keyboardStatusMatrix [r]); }
		inline void setKeyboardStatusMatrix (size_t r, size_t c, bool s);
		const MCHEmul::UByte& joystickStatus (size_t nj) const
							{ return (_joystickStatus [nj]); }
		void setJoystickStatus (size_t nj, const MCHEmul::UByte& s)
							{ _joystickStatus [nj] = s; _joyValueChanged [nj] = true; }

		inline void initializeKeyboardAndJoystickStatus ();

		private:
		/** 0 at the bit representing the key of the keyboard matrix pressed. 
			In this case the matrix is always 8 * 8 = 64 positions. */
		MCHEmul::UByte _keyboardStatusMatrix [8];
		MCHEmul::UByte _rev_keyboardStatusMatrix [8];
		/** At the bit representing the direction/fire of the joystick pressed (@see header). */
		MCHEmul::UByte _joystickStatus [2]; // tow joysticks...
		MCHEmul::OBool _joyValueChanged [2];

		// Implementation
		/** The memory is used also as the set of registers of the chip. */
		C6529B1Registers* _C6529B1Registers;
	};

	// ---
	inline void C6529B1::setKeyboardStatusMatrix (size_t r, size_t c, bool s)
	{ 
		_keyboardStatusMatrix [r].setBit (c, s);
		_rev_keyboardStatusMatrix [c].setBit (r, s);
							
		_C6529B1Registers -> setValue (0 /** it doesn't matter, all records reach the same effect. */, 
			_keyboardStatusMatrix [r]);
	}

	// ---
	void C6529B1::initializeKeyboardAndJoystickStatus ()
	{
		// Initialize the status of the keyboard...
		for (size_t i = 0; i < 8; i++)
			_keyboardStatusMatrix [i] = _rev_keyboardStatusMatrix [i] = MCHEmul::UByte::_0;
		// ...and the joystick...
		_joystickStatus [0] = _joystickStatus [1] = MCHEmul::UByte::_FF;

		// To clean up the latches, if any!
		if (_C6529B1Registers != nullptr) 
			_C6529B1Registers -> latchChanged (); 
		_joyValueChanged [0] = _joyValueChanged [1] = false;
	}
}

#endif
  
// End of the file
/*@}*/
