/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: VIAShiftRegister.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
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

		/** Which signal is used to shift the register in/out? 
			How does the Shift REgister behaves? */
		enum class ShiftMode
		{
			_DISABLE			= 0,		// None. The shift register is free for other purposes.

			// IN
			_INUNDERTIMER2		= 1,		// Shift - in (from CB2 to bit 0) every time Timer2 reaches0LSB = true.
											// Every shift - in, CB1 changes (shift pulse).
											// After 8 shift pulses in CB1 (or 8 shift - ins),
											// the shift operation is disabled and the interrupt flag is set.
			_INATCLOCK			= 2,		// Same than above but system clock pulses determine the shift - out pace instead Timer2.
			_INUNDERSIGNALCB1	= 3,		// The input signal to shift is here the changes in the CB1 status.
											// After 8 bits shifted - in, and flaging the interrupt, the counter starts back.
											// Writting or reading the "ShiftRegisters" puts back the counter to 0, 
											// and clear the interrupt flag.

			// OUT
			_OUTFREERUNNING		= 4,		// @see just below.
											// But never stops.
											// Continuous (bit 7 enters back bit 0). Shift counter makes no sense.
			_OUTUNDERTIMER2		= 5,		// Shift - out (bit 7 to CB2) every time Timer2 reaches0 = true.
											// Every shift - out, CB1 changes (shift pulse).
											// After 8 shift pulses in CB1 (or 8 shift - outs),
											// the shift operation is disabled, the interrupt flag is set,
											// and CB2 goes to the status that PCR (VIA register 5) defines (bit 5).
											// To start back the shift - out process, 
											// data has be read/write from/into the "ShiftRegister".
			_OUTATCLOCK			= 6,		// Same than above, 
											// but system clock pulses determine the shift - out pace instead Timer2.
			_OUTUNDERSIGNALCB1	= 7			// The input signal to shift is here the changes in CB1 status.
		};

		VIAShiftRegister (int id /** unique id in the CIA Chip. */, unsigned int iId)
			: MCHEmul::InfoClass ("VIAShiftRegister"),
			  _id (id), 
			  _interruptId (iId),
			  _mode (COMMODORE::VIAShiftRegister::ShiftMode::_DISABLE),
			  _value (MCHEmul::UByte::_0),
			  _timer2 (nullptr), // It has to be assigned later, before similating...
			  _CB1 (nullptr), _CB2 (nullptr), // It has to be assigned later, before simulating...
			  _interruptEnabled (false),
			  _interruptRequested (false),
			  _CB1Pulse (false)
							{ initialize (); } // To assign the implementation variables...

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
							{ _timer2 = t; }
		void lookAtControlLines (VIAControlLine* cb1, VIAControlLine* cb2)
							{ _CB1 = cb1; _CB2 = cb2; }

		private:
		int _id;
		unsigned int _interruptId;
		ShiftMode _mode;
		MCHEmul::UByte _value;
		VIATimer* _timer2;
		VIAControlLine* _CB1;
		VIAControlLine* _CB2;
		/** When the interrupt is enabled. */
		bool _interruptEnabled;

		// Implementation
		/** How many bits have been already shifted in/out? */
		mutable unsigned char _numberBits;
		/** True when the shift register is not active. 
			because the mode or because it has finished it job. */
		mutable bool _disable;
		/** True when the shift register has just read or written down.
			Bear in mind that just reading or writting this variable puts its value to false. */
		mutable bool _justReadOrWritten; 
		unsigned int _lastCPUCycles;
		/** When the interrupt has been requested. */
		mutable MCHEmul::OBool _interruptRequested;
		/** To track a pulse in CB1. */
		mutable MCHEmul::Pulse _CB1Pulse;
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

		_CB1Pulse.set (false);

		return (_value);
	}

	// ---
	inline void VIAShiftRegister::setValue (const MCHEmul::UByte& v)
	{
		_numberBits = 0;

		_justReadOrWritten = true;

		_interruptRequested = false;

		_CB1Pulse.set (false);

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
