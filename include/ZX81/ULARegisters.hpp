/** \ingroup ZX81 */
/*@{*/

/**	
 *	@file	
 *	File: ULARegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 25/03/2024 \n
 *	Description: ULA Registers Emulation.
 *				 They don't exist actually, but they are used to store the critical info of the ULA
 *				 and to make it also accesible for the ports related (FE,...)
 *	Versions: 1.0 Initial
 */

#ifndef __ZX81_ULAREGISTERS__
#define __ZX81_ULAREGISTERS__

#include <CORE/incs.hpp>
#include <ZX81/Type.hpp>

namespace ZX81
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

		ULARegisters (Type t);

		// The NMIGenerator...
		bool NMIGenerator () const
							{ return (_NMIGenerator); }
		void setNMIGenerator (bool a)
							{ _NMIGenerator = a; }

		// NTSC?
		bool NTSC () const
							{ return (_NTSC); }
		void setNTSC(bool n)
							{ _NTSC = n; }

		// To control how the video signal is issued...
		bool inVSync () const
							{ return (_inVSync); }
		void setInVSync (bool i)
							{ _inVSync = i; }
		inline void setVSync (bool s);
		bool syncOutputWhite () const
							{ return (_syncOutputWhite); }
		void setSyncOutputWhite (bool w)
							{ _syncOutputWhite = w; }

		// When INT interrupt has been acknowledged...
		void setINTack (unsigned int cC)
							{ _INTack = true; _INTackClock = cC; }
		/** Returns true if the INT has been acknowledged at the given clock cycle. \n/ */
		inline bool INTack (unsigned int cC) const;
		unsigned int INTackClock () const
							{ return (_INTackClock); }

		// Counting internally from 0 to 7 every HSYNC with an external origin (INT)...
		unsigned char LINECNTRL () const // From 0 to 8...
							{ return (_LINECNTRL); }
		void setLINECNTRL (unsigned char lc)
							{ _LINECNTRL = lc; }
		bool LINECTRLBlocked () const
							{ return (_LINECNTRLBlocked); }
		void setLINECTRLNBlocked (bool lb)
							{ _LINECNTRLBlocked = lb; }
		inline unsigned char incLINECTRL (); // From 0 to 7 always...

		// Working with the CHAR Register...
		bool reverseVideo () const
							{ return (_reverseVideo); }
		void setReverseVideo (bool rV)
							{ _reverseVideo = rV; }

		// Working with the SHIFT Register...
		const MCHEmul::UByte& SHIFTRegister () const
							{ return (_SHIFTR); }
		const MCHEmul::UByte& originalSHIFTRegister () const
							{ return (_originalSHIFTR); }
		/** Only if the previous SHIFT Register value has all bits shifted out. */
		inline bool loadSHIFTRegister (const MCHEmul::UByte& sr);
		/** Shifts the SHIFT Register left. \n
			Returns true if the bit has been shifted out, false otherwise. \n
			The d variable has the value shifed, if any!.
			When 8 bits are shifted out, it returns false. */
		inline bool shiftOutData (bool& d);

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
		  * NTSC			= Attribute: NTSC or PAL?.
		  * NMIGEN			= Attribute: Whether the NMI generator is or not active. \n
		  * SYNCWHITE		= Attribute: The ULA is generating a white noise? \n
		  * LINECNTRL		= Attribute: In which line (every 8) the raster line is in? \n
		  * CASETTE			= Attribute: The last signal sent to the casette.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		void initializeInternalValues ();

		private:
		/** The type of model representing in this ULA. */
		Type _type;
		/** The NMI Generator can be active or not. */
		bool _NMIGenerator;
		/** NTSC. This variable is set when starting. */
		bool _NTSC;
		/** Whether the ULA is or nor doing a VSync. */
		bool _inVSync;
		/** When the ULA is in the first zone of the memory a whote nouse is generated,
			once it enter in the slow / fast zone, the video can be generated. */
		bool _syncOutputWhite;
		/** The INT ack. Once it is checked becomes false. */
		MCHEmul::OBool _INTack;
		/** At the same time it is checked, the value of the clock is kept. 
			This value makes no sense when the previous one is false, just when it is true. */
		unsigned int _INTackClock;
		/** Something similar is done withe this 3 bits (0-7) internal counter,
			that is used to determine which line of the character has to be ddrawn
			and increments every HSYNC event. */
		unsigned char _LINECNTRL;
		bool _LINECNTRLBlocked;
		/** Video inverse or not?. */
		bool _reverseVideo;
		/** The SHIFT Register. */
		MCHEmul::UByte _SHIFTR, _originalSHIFTR;
		/** The number of bit shifted. When it becomes 8 no more shifts are done. */
		unsigned char _shiftedBit;
		/** The MIC signal and the signal to indicate whether it has changed. */
		bool _MICSignal;
		MCHEmul::OBool _MICSignalChanged;
		/** The EAR signal and the signal to indicate whether it has changed. */
		bool _EARSignal;
		MCHEmul::OBool _EARSignalChanged;
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
	inline void ULARegisters::setVSync (bool s)
	{
		// When the systems enters in the VSync...
		if (s)
		{
			_inVSync = true; 
			// no output signal...
			_syncOutputWhite = false;
			// and no counting LINES (from 0 to 7)...
			// In the Type::_ZX80, the counting starts from 0...
			_LINECNTRL = (_type == Type::_ZX80) ? 0 : 7; 
			_LINECNTRLBlocked = true;
		}
		// When it is not...
		else
		{
			_inVSync = false;
			// output signal allowed...
			_syncOutputWhite = true;
			// ans starts to count LINECTRL...
			_LINECNTRLBlocked = false;

			_shiftedBit = 8; // To load a new value...
		}
	}

	// ---
	bool ULARegisters::INTack (unsigned int cC) const
	{ 
		bool tmp = 
			(cC == _INTackClock) && _INTack.peekValue () /** Doesn't change the value. */;
							 
		return (tmp
			? (bool) _INTack /** to reset the value. */ : false);
	}

	// ---
	inline unsigned char ULARegisters::incLINECTRL ()
	{
		if (!_LINECNTRLBlocked && (++_LINECNTRL > 7))
			_LINECNTRL = 0;
		return (_LINECNTRL);
	}

	// ---
	inline bool ULARegisters::loadSHIFTRegister (const MCHEmul::UByte& sr)
	{ 
		bool result = false;

		if (result = (_shiftedBit >= 8)) // Only at the end...
		{
			_SHIFTR = _originalSHIFTR = sr;

			_shiftedBit = 0; // Reset the shifted bit counter.
		}

		return (result);
	}

	// ---
	inline bool ULARegisters::shiftOutData (bool& d)
	{ 
		bool result = false;

		if (_shiftedBit < 8)
		{
			// Shift the bit out...
			result = true;
				
			d = _SHIFTR.shiftLeftC ();

			_shiftedBit++;

		}

		return (result);
	}

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
}

#endif
  
// End of the file
/*@}*/
