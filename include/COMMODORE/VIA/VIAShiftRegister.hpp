/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: VIAShiftRegister.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 07/01/2024 \n
 *	Description: The "ShiftRegister" of the VIA Chip (@see VIA for better understanding).
 *				 @see https://web.archive.org/web/20160108173129if_/http://archive.6502.org/datasheets/mos_6522_preliminary_nov_1977.pdf
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_VIASHIFTREGISTER__
#define __VIC20_VIASHIFTREGISTER__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	class VIAControlLine;
	class VIATimer;
	class VIA;

	/** The "ShiftRegister" is a way to convert serial data into paralle and the other way around. \n
		There is only one "ShifyRegister" in each VIA Chip. */
	class VIAShiftRegister final : public MCHEmul::InfoClass
	{
		public:
		friend VIA;

		/** Which signal is used to shift the "ShiftRegister" in/out? 
			How does the "ShiftRegister" behave? */
		enum class ShiftMode
		{
			_DISABLE			= 0,		// None. The "ShiftRegister" is free for other purposes.

			// IN
			_INUNDERTIMER		= 1,		// Shift - in (from _CL2 value to bit 0) every time _T reaches0LSB = true.
											// Every shift - in, _CL1 changes (shift pulse).
											// After 8 shift pulses in _CL1 (or 8 shift - ins),
											// the shift operation is disabled and the interrupt flag is set.
			_INATCLOCK			= 2,		// Same than above but system clock pulses determine the shift - in pace instead _T.
			_INUNDERSIGNALCL1	= 3,		// The input signal to shift is here the changes in the _CL1 status.
											// After 8 bits shifted - in, and flagging the interrupt, the counter starts back.
											// Writting or reading the "ShiftRegisters" puts back the counter to 0, 
											// and clear the interrupt flag.

			// OUT
			_OUTFREERUNNING		= 4,		// @see just below.
											// But never stops.
											// Continuous (bit 7 enters back bit 0). Shift counter makes no sense.
			_OUTUNDERTIMER		= 5,		// Shift - out (bit 7 to _CL2) every time _T reaches0 = true.
											// Every shift - out, _CL1 changes (shift pulse).
											// After 8 shift pulses in _CL1 (or 8 shift - outs),
											// the shift operation is disabled, the interrupt flag is set,
											// and _CL2 goes to the status that PCR (VIA Register 0x05) defines (bit 5).
											// To start back the shift - out process, 
											// data has be read/write from/into the "ShiftRegister".
			_OUTATCLOCK			= 6,		// Same than above, 
											// but system clock pulses determine the shift - out pace instead _T.
			_OUTUNDERSIGNALCL1	= 7			// The input signal to shift is here the changes in _CL1 status.
		};

		VIAShiftRegister (int id);

		ShiftMode mode () const
							{ return (_mode); }
		inline void setMode (ShiftMode m);

		inline const MCHEmul::UByte value () const;
		inline void setValue (const MCHEmul::UByte& v);

		// Managing interrupt data...
		bool interruptEnabled () const
							{ return (_interruptEnabled); }
		void setInterruptEnabled (bool e)
							{ _interruptEnabled = e; }
		bool interruptRequested () const
							{ return (_interruptRequested); }
		bool peekInterruptRequested () const
							{ return (_interruptRequested.peekValue ()); }
		bool launchInterrupt () const
							{ return (_interruptRequested.peekValue () && _interruptEnabled); }

		void initialize ();

		/** Returns true with everything ok, and false in any other circunstance. */
		bool simulate (MCHEmul::CPU* cpu);

		/**
		  *	The name of the fields are: \n
		  * The ones comming from the parent class and: \n
		  * MODE			= Attribute: Mode of the "ShiftRegister". \n
		  *	VALUE			= Attribute: Current Value of the "ShiftRegister". \n
		  * NUMBERBITS		= Attribute: Status. From 0 to 7. \n
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		/** To shift in/out the data. \n
			Returns true when 8 bits has been already shifted. */
		inline bool shiftIn (bool i);
		/** The parameter r indicates whether the shifted out bit has or not to be recirculated. \n
			It is NO recirculated by default. */
		inline bool shiftOut (bool& o, bool r = false);

		/** To link the shift register with the VIA chip. */
		void lookAtTimer (VIATimer* t)
							{ _T = t; }
		void lookAtControlLines (VIAControlLine* c1, VIAControlLine* c2)
							{ _CL1 = c1; _CL2 = c2; }

		private:
		int _id;
		ShiftMode _mode;
		MCHEmul::UByte _value;
		bool _interruptEnabled;

		// Other Elements the Shift Register has relation with...
		VIATimer* _T;
		VIAControlLine* _CL1;
		VIAControlLine* _CL2;

		// Implementation
		/** How many bits have been already shifted in/out? */
		mutable unsigned char _numberBits;
		/** True when the "ShiftRegister" is not active. 
			because the mode or because it has finished it job. */
		mutable bool _disable;
		/** True when the "ShiftRegister" has just read or written down.
			Bear in mind that just reading or writting this variable puts its value to false. */
		mutable bool _justReadOrWritten; 
		/** When the interrupt has been requested. */
		mutable MCHEmul::OBool _interruptRequested;
	};

	// ---
	inline void VIAShiftRegister::setMode (VIAShiftRegister::ShiftMode m)
	{
		_mode = m;

		initialize ();
	}

	// ---
	inline const MCHEmul::UByte VIAShiftRegister::value () const
	{
		_numberBits = 0;

		_justReadOrWritten = true;

		_interruptRequested = false;

		return (_value);
	}

	// ---
	inline void VIAShiftRegister::setValue (const MCHEmul::UByte& v)
	{
		_numberBits = 0;

		_justReadOrWritten = true;

		_interruptRequested = false;

		_value = v;
	}

	// ---
	inline bool VIAShiftRegister::shiftIn (bool i)
	{
		_value.shiftLeftC (i);

		bool result = (++_numberBits == 8); // Complete?
		if (result) 
			_numberBits = 0;

		return (result);
	}

	// ---
	inline bool VIAShiftRegister::shiftOut (bool& o, bool r)
	{
		o = _value.shiftLeftC (r /** Recirculating? */ ? _value.bit (7) : false);

		bool result = (++_numberBits == 8); // Complete?
		if (result)
			_numberBits = 0;

		return (result);
	}
}

#endif
  
// End of the file
/*@}*/
