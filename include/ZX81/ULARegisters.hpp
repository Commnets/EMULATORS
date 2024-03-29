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

		// Control of the raster...
		bool VSYNCPulse () const
							{ return (_VSYNCPulse); }
		void setVSYNCPulse (bool vs)
							{ _VSYNCPulse = vs; }
		unsigned char LINECNTRL () const // From 0 to 8...
							{ return (_LINECNTRL); }
		void setLINECNTRL (unsigned char lc)
							{ _LINECNTRL = lc; }
		inline unsigned char incLINECTRL ();
		unsigned short currentRasterLine () const
							{ return (_currentRasterLine); }
		void setCurrentRasterLine (unsigned short rL)
							{ _currentRasterLine = rL; }

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

		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		void initializeInternalValues ();

		private:
		/** The NMI Generator can be active or not. */
		bool _NMIGenerator;
		/** NTSC. */
		bool _NTSC;
		/** Whether the VSYNC pulse is detected. */
		bool _VSYNCPulse;
		/** Vertical Line Counter. */
		unsigned char _LINECNTRL;
		/** The casette signal and the signal to indicate whether it has changed. \n
			When read it becomes back to false. */
		bool _casetteSignal;
		MCHEmul::OBool _casetteSignalChanged;
		/** Where the status of the keyboard matrix is kept. */
		std::vector <MCHEmul::UByte> _keyboardStatus;

		// Implmentation
		/** The current raster line managed from the ULA. */
		unsigned short _currentRasterLine;
	};

	// ---
	inline unsigned char ULARegisters::incLINECTRL ()
	{
		if (!_VSYNCPulse && (++_LINECNTRL > 7))
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
