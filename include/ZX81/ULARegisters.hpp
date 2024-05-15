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

namespace ZX81
{
	/** Where the important info of the ULA chip is kept. \n
		Notice that this registers are not set in memory. \n
		In the case of Z80 processor this task is done by the Z80Ports. */
	class ULARegisters final : public MCHEmul::InfoClass
	{
		public:
		ULARegisters ();

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
		void setINTack ()
							{ _INTack = true; }
		bool INTack () const
							{ return (_INTack); }

		// Counting internally from 0 to 7 every HSYNC...
		unsigned char LINECNTRL () const // From 0 to 8...
							{ return (_LINECNTRL); }
		void setLINECNTRL (unsigned char lc)
							{ _LINECNTRL = lc; }
		bool LINECTRLBlocked () const
							{ return (_LINECNTRLBlocked); }
		void setLINECTRLNBlocked (bool lb)
							{ _LINECNTRLBlocked = lb; }
		inline unsigned char incLINECTRL (); // From 0 to 7 always...

		// Working with the SHIFT Register...
		const MCHEmul::UByte& SHIFTRegister () const
							{ return (_SHIFTR); }
		const MCHEmul::UByte& originalSHIFTRegister () const
							{ return (_originalSHIFTR); }
		void setSHIFTRegister (const MCHEmul::UByte& sr) // with 0 restart it...
							{ _SHIFTR = _originalSHIFTR = sr; }
		bool shiftOutData ()
							{ return (_SHIFTR.shiftLeftC ()); }

		// The casette signal...
		bool casetteSignal () const
							{ return (_casetteSignal); }
		bool casetteSignalChanged () const // When read, it becomes back to false,...
							{ return (_casetteSignalChanged); }
		bool peekCasetteSignalChanged () const
							{ return (_casetteSignalChanged.peekValue ()); }
		inline void setCasetteSignal (bool cs);

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
		/** Something similar is done withe this 3 bits (0-7) internal counter,
			that is used to determine which line of the character has to be ddrawn
			and increments every HSYNC event. */
		unsigned char _LINECNTRL;
		bool _LINECNTRLBlocked;
		/** The SHIFT Register. */
		MCHEmul::UByte _SHIFTR, _originalSHIFTR;
		/** The casette signal and the signal to indicate whether it has changed. \n
			When read it becomes back to false. */
		bool _casetteSignal;
		MCHEmul::OBool _casetteSignalChanged;
		/** Where the status of the keyboard matrix is kept. */
		std::vector <MCHEmul::UByte> _keyboardStatus;
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
			_LINECNTRL = 0;
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
		}
	}

	// ---
	inline unsigned char ULARegisters::incLINECTRL ()
	{
		if (!_LINECNTRLBlocked && (++_LINECNTRL > 7))
			_LINECNTRL = 0;
		return (_LINECNTRL);
	}

	// ---
	inline void ULARegisters::setCasetteSignal (bool cs)
	{ 
		if (cs != _casetteSignal) 
		{ 
			_casetteSignal = cs;
			
			_casetteSignalChanged = true; 
		} 
	}
}

#endif
  
// End of the file
/*@}*/
