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

		// TODO...

		// NTSC?
		bool NTSC () const
							{ return (_NTSC); }
		void setNTSC(bool n)
							{ _NTSC = n; }

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
		  * TODO...
		  * CASETTE			= Attribute: The last signal sent to the casette.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		void initializeInternalValues ();

		private:
		/** NTSC. This variable is set when starting. */
		bool _NTSC;
		/** The casette signal and the signal to indicate whether it has changed. \n
			When read it becomes back to false. */
		bool _casetteSignal;
		MCHEmul::OBool _casetteSignalChanged;
		/** Where the status of the keyboard matrix is kept. */
		std::vector <MCHEmul::UByte> _keyboardStatus;
	};

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
