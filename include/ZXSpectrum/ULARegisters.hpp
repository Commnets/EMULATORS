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

		// The MIC Signal
		bool MICSignal () const
							{ return (_MICSignal); }
		bool MICSignalChanged () const
							{ return (_MICSignalChanged); }
		bool peekMICSignalChanged () const
							{ return (_MICSignalChanged.peekValue ()); }
		inline void setMICSignal (bool cs);

		// The EAR Signal
		bool EARSignal () const
							{ return (_EARSignal); }
		bool EARSignalChanged () const
							{ return (_EARSignalChanged); }
		bool peekEARSignalChanged () const
							{ return (_EARSignalChanged.peekValue ()); }
		inline void setEARSignal (bool cs);

		// The buzzer signal
		bool buzzerSignal () const
							{ return (_buzzerSignal); }
		/** According with the status of the EAR and MIC signals. 
			EAR affect directly, but when it doesn't change, the sound is managed by the MIC. */
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
		  * The ones from the parent class +
		  * BORDER		= Attribute: The color or the border. \n
		  *	MICST		= Attribute: Whether the MIC signal is activated. \n
		  *	MIC			= Attribute: The value of the MIC signal \n
		  *	EARST		= Attribute: Whether the EAR signal is activated. \n
		  *	EAR			= Attribute: The value of the EAR signal.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		void initializeInternalValues ();

		private:
		/** The border color. */
		unsigned char _borderColor;
		/** The MIC signal and the signal to indicate whether it has changed. */
		bool _MICSignal;
		MCHEmul::OBool _MICSignalChanged;
		/** The EAR signal and the signal to indicate whether it has changed. */
		bool _EARSignal;
		MCHEmul::OBool _EARSignalChanged;
		/** The buzzer signal.
			It is not directly the EAR signal, but it is also affected by the MIC one. */
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
	};

	// ---
	inline void ULARegisters::setMICSignal (bool cs)
	{ 
		if (_MICSignal != cs) 
		{ 
			_MICSignal = cs;
			
			_MICSignalChanged = true; 
		}
	}

	// ---
	inline void ULARegisters::setEARSignal (bool cs)
	{ 
		if (_EARSignal != cs) 
		{ 
			_EARSignal = cs;
			
			_EARSignalChanged = true;
		} 
	}

	// ---
	inline void ULARegisters::alignBuzzerSignal ()
	{
		if (_EARSignalChanged) _buzzerSignal = _EARSignal;
		else if (_MICSignalChanged.peekValue ()) _buzzerSignal = _MICSignal;
		// Here it is used peekMICSignalChanged () to avoid changing the value
		// Because the simulate method of ULA uses the chan ge to send and event (usually to the datasette)!
	}
}

#endif
  
// End of the file
/*@}*/
