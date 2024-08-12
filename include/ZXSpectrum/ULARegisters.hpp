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
		ULARegisters ();

		// The border color
		unsigned char borderColor ()
							{ return (_borderColor); }
		void setBorderColor (unsigned char bC)
							{ _borderColor = bC; }

		// The MIC Signal
		bool MICActivated () const
							{ return (_MICActivated); }
		inline void setMICActivated (bool mA);
		bool MICSignal () const
							{ return (_MICSignal); }
		bool MICSignalChanged () const
							{ return (_MICSignalChanged); }
		bool peekMICSignalChanged () const
							{ return (_MICSignalChanged.peekValue ()); }
		inline void setMICSignal (bool cs);

		// The EAR Signal
		// Activating the EAR Signal the speaker is activated...
		bool EARActivated () const
							{ return (_EARActivated); }
		inline void setEARActivated (bool mA);
		bool EARSignal () const
							{ return (_EARSignal); }
		bool EARSignalChanged () const
							{ return (_EARSignalChanged); }
		bool peekEARSignalChanged () const
							{ return (_EARSignalChanged.peekValue ()); }
		inline void setEARSignal (bool cs);

		// Info about the situation of the keyboard!
		const MCHEmul::UByte& keyboardStatus (size_t r) const
							{ return (_keyboardStatus [r]); }
		bool keyboardStatus (size_t r, size_t c) const
							{ return (_keyboardStatus [r].bit (c)); }
		void setKeyboardStatus (size_t r, size_t c, bool v)
							{ _keyboardStatus [r].setBit (c, v); }

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
		/** To indicate whether the MIC signal is or not activated. */
		bool _MICActivated;
		/** The MIC signal and the signal to indicate whether it has changed. */
		bool _MICSignal;
		MCHEmul::OBool _MICSignalChanged;
		/** To indicate whether the EAR signal is or not activated. */
		bool _EARActivated;
		/** The EAR signal and the signal to indicate whether it has changed. */
		bool _EARSignal;
		MCHEmul::OBool _EARSignalChanged;
		/** Where the status of the keyboard matrix is kept. */
		std::vector <MCHEmul::UByte> _keyboardStatus;
	};

	// ---
	inline void ULARegisters::setMICActivated (bool mA) // Activating one the other is also activated...
	{ 
		if (_MICActivated = mA) 
			setEARActivated (true); 
		
		_MICSignal = false; 
		_MICSignalChanged = false; 
	}

	// ---
	inline void ULARegisters::setMICSignal (bool cs)
	{ 
		if (cs != _MICSignal) 
		{ 
			_MICSignal = cs;
			
			_MICSignalChanged = true; 
		}
	}

	// ---
	inline void ULARegisters::setEARActivated (bool mA)
	{ 
		if (_EARActivated = mA) 
			setMICActivated (true); // Activating one the other is also activated...
		
		_EARSignal = false; 
		_EARSignalChanged = false; 
	}

	// ---
	inline void ULARegisters::setEARSignal (bool cs)
	{ 
		if (cs != _EARSignal) 
		{ 
			_EARSignal = cs;
			
			_EARSignalChanged = true; 
		} 
	}
}

#endif
  
// End of the file
/*@}*/
