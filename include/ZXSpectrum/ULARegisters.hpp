/** \ingroup ZXSPECTRUM */
/*@{*/

/**	
 *	@file	
 *	File: ULARegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/08/2024 \n
 *	Description: ULA Registers Emulation.
 *				 They don't exist actually, but they are used to store the critical info of the ULA
 *				 and to make it also accesible for the ports related (FE,...)
 *	Versions: 1.0 Initial
 */

#ifndef __ZXSPECTRUM_ULAREGISTERS__
#define __ZXSPECTRUM_ULAREGISTERS__

#include <CORE/incs.hpp>

namespace ZXSPECTRUM
{
	/** Where the important info of the ULA chip is kept. \n
		Notice that this registers are not set in memory. \n
		In the case of Z80 processor this task is done by the Z80Ports. */
	class ULARegisters final : public MCHEmul::InfoClass
	{
		public:
		/** To read the different element sof the Joystick. */
		enum class JoystickElement : size_t
		{
			_RIGHT = 0, 
			_LEFT = 1,
			_DOWN = 2, 
			_UP = 3, 
			_FIRE = 4
		};

		ULARegisters ();

		// The border color
		unsigned char borderColor ()
							{ return (_borderColor); }
		void setBorderColor (unsigned char bC)
							{ _borderColor = bC; }

		// The MIC output signal
		bool MICOutputSignal () const
							{ return (_MICOutputSignal); }
		bool MICOutputSignalChanged () const
							{ return (_MICOutputSignalChanged); }
		bool peekMICOutputSignalChanged () const
							{ return (_MICOutputSignalChanged.peekValue ()); }
		inline void setMICOutputSignal (bool cs);

		// The EAR output signal
		bool EAROutputSignal () const
							{ return (_EAROutputSignal); }
		bool EAROutputSignalChanged () const
							{ return (_EAROutputSignalChanged); }
		bool peekEAROutputSignalChanged () const
							{ return (_EAROutputSignalChanged.peekValue ()); }
		inline void setEAROutputSignal (bool cs);

		// The EAR input signal from the cassette
		bool EARInputSignal () const
							{ return (_EARInputSignal); }
		bool EARInputSignalChanged () const
							{ return (_EARInputSignalChanged); }
		bool peekEARInputSignalChanged () const
							{ return (_EARInputSignalChanged.peekValue ()); }
		inline void setEARInputSignal (bool cs);

		/** Value returned in D6 when reading port FE in the emulated Issue 3. */
		bool EARReadSignal () const
							{ return (_EAROutputSignal != _EARInputSignal); }

		// The buzzer signal
		bool buzzerSignal () const
							{ return (_buzzerSignal); }
		/** Aligns the simplified digital buzzer with the signal that has just changed. */
		inline void alignBuzzerSignal ();

		// Info about the situation of the keyboard!
		const MCHEmul::UByte& keyboardStatus (size_t r) const
							{ return (_keyboardStatus [r]); }
		bool keyboardStatus (size_t r, size_t c) const
							{ return (_keyboardStatus [r].bit (c)); }
		void setKeyboardStatus (size_t r, size_t c, bool v)
							{ _keyboardStatus [r].setBit (c, v); }

		// Information about the situation of the joystick!
		bool joystickStatus (JoystickElement jE) const
							{ return (_joystickStatus [(size_t) jE]); }
		void setJoystickStatus (JoystickElement jE, bool v)
							{ _joystickStatus [(size_t) jE] = v; }

		void initialize ();

		/**
		  *	The name of the fields are: \n
		  * The attributes and infostructures of the parent class, plus: \n
		  * BORDER		= Attribute: The color of the border. \n
		  *	MICOUT		= Attribute: The value of the MIC output signal. \n
		  *	EAROUT		= Attribute: The value of the EAR output signal. \n
		  *	EARIN		= Attribute: The value of the EAR input signal from the cassette. \n
		  *	EARREAD		= Attribute: The value returned in D6 when reading port FE. \n
		  *	BUZZER		= Attribute: The simplified digital buzzer level.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		/** This method is invoked from PortManager to point out 
			that the port is being accessed. */
		void setULABeingAccessedFromPortManager ()
							{ _ULABeingAccessedFromPortManager = true; }
		/** And a method to know wether it was accessed or not. 
			Once the attributed is checked, becomes false back. */
		bool ULABeingAccessedFromPortManager () const
							{ return (_ULABeingAccessedFromPortManager); }

		private:
		void initializeInternalValues ();

		private:
		/** The border color. */
		unsigned char _borderColor;
		/** The MIC output signal and the signal to indicate whether it has changed. */
		bool _MICOutputSignal;
		MCHEmul::OBool _MICOutputSignalChanged;
		/** The EAR output signal and the signal to indicate whether it has changed. */
		bool _EAROutputSignal;
		MCHEmul::OBool _EAROutputSignalChanged;
		/** The cassette EAR input signal and the signal to indicate whether it has changed. */
		bool _EARInputSignal;
		MCHEmul::OBool _EARInputSignalChanged;
		/** The buzzer signal.
			It is a simplified digital representation of the analogue output network. */
		bool _buzzerSignal;
		/** Where the status of the keyboard matrix is kept. */
		std::vector <MCHEmul::UByte> _keyboardStatus;
		/** Where the status of the joystick is kept. 5 positions: \n
			-> 0 = right position selected. \n
			-> 1 = left position selected. \n
			-> 2 = down position selected. \n
			-> 3 = up position selected. \n
			-> 4 = fire button pressed. */
		std::vector <bool> _joystickStatus;
		/** When the ULA is being accessed from outside. */
		MCHEmul::OBool _ULABeingAccessedFromPortManager;
	};

	// ---
	inline void ULARegisters::setMICOutputSignal (bool cs)
	{ 
		if (_MICOutputSignal != cs)
		{ 
			_MICOutputSignal = cs;
			
			_MICOutputSignalChanged = true;
		}
	}

	// ---
	inline void ULARegisters::setEAROutputSignal (bool cs)
	{ 
		if (_EAROutputSignal != cs)
		{ 
			_EAROutputSignal = cs;
			
			_EAROutputSignalChanged = true;
		} 
	}

	// ---
	inline void ULARegisters::setEARInputSignal (bool cs)
	{
		if (_EARInputSignal != cs)
		{
			_EARInputSignal = cs;

			_EARInputSignalChanged = true;
		}
	}

	// ---
	inline void ULARegisters::alignBuzzerSignal ()
	{
		if (_EARInputSignalChanged) _buzzerSignal = _EARInputSignal;
		else if (_EAROutputSignalChanged) _buzzerSignal = _EAROutputSignal;
		else if (_MICOutputSignalChanged.peekValue ()) _buzzerSignal = _MICOutputSignal;
		// MIC is only peeked because ULA::simulate consumes the change to notify the cassette.
	}
}

#endif
  
// End of the file
/*@}*/
