/** \ingroup COMMODORE */
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
	class VIAControlLineType1;
	class VIAControlLineType2;
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

			// Internally generated CB1 clocks shift on falling edges.
			// Timer modes use a private divider loaded from the low T2 latch,
			// independently of the T2 counter and its timeout/zero flags.
			_INUNDERTIMER		= 1,		// Eight input bits from CB2; T2-latch divider.
			_INATCLOCK			= 2,		// Eight input bits from CB2; PHI2 divider.
			_INUNDERSIGNALCL1	= 3,		// Input on external CB1 positive edges; repeated bytes.

			_OUTFREERUNNING		= 4,		// T2-latch divider; recirculating output, modulo-eight count, no IFR2.
			_OUTUNDERTIMER		= 5,		// Eight output bits to CB2; T2-latch divider.
			_OUTATCLOCK			= 6,		// Eight output bits to CB2; PHI2 divider.
			_OUTUNDERSIGNALCL1	= 7			// Output on external CB1 positive edges; repeated bytes.
			// Finite internal transfers complete the final rising CB1 half-cycle.
			// Except in mode 4, the eighth shift sets IFR2.
			// Finite internal output idles at PCR bit 5 after the final rising edge.
			// CB1 interrupt polarity remains independently selected by PCR.
		};

		VIAShiftRegister (int id);

		ShiftMode mode () const
							{ return (_mode); }
		/** Reapplying the current mode preserves the transfer; a changed mode resets it. */
		void setMode (ShiftMode m);

		// Ordinary reads/writes clear IFR2 and restart a transfer; peekValue() only observes.
		const MCHEmul::UByte value () const;
		const MCHEmul::UByte& peekValue () const
							{ return (_value); }
		void setValue (const MCHEmul::UByte& v);

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
		void startTransfer () const;
		void updateCB2Function () const;

		/** To shift in/out the data. \n
			Returns true when 8 bits has been already shifted. */
		inline bool shiftIn (bool i);
		/** Shifts bit 7 to CB2 and recirculates it into bit 0. */
		inline bool shiftOut (bool& o);

		/** To link the shift register with the VIA chip. */
		void lookAtTimer (VIATimer* t)
							{ _T = t; }
		void lookAtControlLines (VIAControlLineType1* c1, VIAControlLineType2* c2)
							{ _CL1 = c1; _CL2 = c2; }

		private:
		int _id;
		ShiftMode _mode;
		MCHEmul::UByte _value;
		bool _interruptEnabled;

		// Other Elements the Shift Register has relation with...
		VIATimer* _T;
		VIAControlLineType1* _CL1;
		VIAControlLineType2* _CL2;

		// Implementation
		/** Number of bits shifted modulo eight, including free-running mode. */
		mutable unsigned char _numberBits;
		/** Whether the current transfer is producing/accepting shift clocks. */
		mutable bool _shifting;
		/** A finite internally-clocked transfer needs a final rising edge on CB1. */
		mutable bool _finishPending;
		/** Independent divider loaded from the low byte of the Timer 2 latch. */
		mutable unsigned short _timerCounter;
		/** Current phase of an internally generated CB1 clock. */
		mutable bool _clockPhase;
		/** When the interrupt has been requested. */
		mutable MCHEmul::OBool _interruptRequested;
	};

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
	inline bool VIAShiftRegister::shiftOut (bool& o)
	{
		o = _value.shiftLeftC (_value.bit (7));

		bool result = (++_numberBits == 8); // Complete?
		if (result)
			_numberBits = 0;

		return (result);
	}
}

#endif
  
// End of the file
/*@}*/
