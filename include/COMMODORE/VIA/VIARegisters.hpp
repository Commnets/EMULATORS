/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VIARegisters.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 16/12/2023 \n
 *	Description: VIA Registers Emulation
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_VIAREGISTERS__
#define __COMMODORE_VIAREGISTERS__

#include <CORE/incs.hpp>
#include <COMMODORE/VIA/VIAControlLine.hpp>
#include <COMMODORE/VIA/VIATimer.hpp>
#include <COMMODORE/VIA/VIAShiftRegister.hpp>

namespace COMMODORE
{
	class VIA;
	class VIAControlLine;
	class VIATimer;
	class VIAShiftRegister;

	/** In the VIA Memory, there are a couple of records that behave different
		when they are read that when they are written. \n
		Read this document for better understanding: \n
		https://web.archive.org/web/20160108173129if_/http://archive.6502.org/datasheets/mos_6522_preliminary_nov_1977.pdf \n
		One of the most important characteristics of the VIA is the port management. \n
		The VIA manages two ports independently: Port A (PA) and Port B (PB). \n
		*/
	class VIARegisters : public MCHEmul::ChipRegisters
	{
		public:
		friend VIA;

		// Some events.
		/** When the ports are actualized. \n
			The parameter is the value (1 or 0) of the actualization. */
		static const unsigned int _PORTAACTUALIZED = 210;
		static const unsigned int _PORTBACTUALIZED = 211;

		VIARegisters (int id, MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		virtual size_t numberRegisters () const override
							{ return (0x10); }

		virtual void initialize () override;

		// Managing the ports...
		/** Read the value. */
		MCHEmul::UByte portB () const
							{ return (MCHEmul::UByte (_portB)); }
		inline MCHEmul::UByte setPortB (const MCHEmul::UByte& v, bool f = false);
		bool lastPB6 () const
							{ return (_lastPB6); }
		bool hasPB6Changed () const
							{ return (_PB6Changed); }
		bool latchAEnabled () const
							{ return (_latchAEnabled); }

		/** Same than port B. */
		MCHEmul::UByte portA () const
							{ return (MCHEmul::UByte (_portA)); }
		MCHEmul::UByte setPortA (const MCHEmul::UByte& v, bool f = false);
		bool latchBEnabled () const
							{ return (_latchBEnabled); }

		// The output registers...
		/** Their values are used internally to operate ports. */
		unsigned char outputRegisterB () const
							{ return (_outputRegB); }
		unsigned char outputRegisterA () const
							{ return (_outputRegA); }

		// All these method are invoked from VIA emulation
		/** To know the value of the data direction registers. */
		unsigned char dataPortBDir () const
							{ return (_dataPortBDir); }
		unsigned char dataPortADir () const
							{ return (_dataPortADir); }

		/** To know whether the interrupt has or not to be launched,
			becasue any of the elements integrating the VIA has generated one allowed. */
		inline bool launchInterrupt () const;
		/** Just to have a code with the reason of the interrupt when happens. */
		inline unsigned int reasonIntCode () const;

		protected:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;
		virtual const MCHEmul::UByte& peekValue (size_t p) const override;

		// Implementation
		virtual void initializeInternalValues ();

		// This methods are invoked from CIA chip...
		/** Link to the Control lines. */
		void lookAtControlLines (VIAControlLine* ca1, VIAControlLine* ca2, 
			VIAControlLine* cb1, VIAControlLine* cb2)
							{ _CA1 = ca1; _CA2 = ca2; _CB1 = cb1; _CB2 = cb2; }
		/** The registers will observe also the timers. */
		void lookAtTimers (VIATimer* tA, VIATimer* tB)
							{ _timerA = tA, _timerB = tB; }
		/** The registers will observe also the shift register. */
		void lookAtShiftRegister (VIAShiftRegister* s)
							{ _shiftRegister = s; }

		protected:
		/** Ports used by the VIA. 
			These reflects the values that are in the PINS of the chip:	PB7-0, PA7-0 */
		mutable unsigned char _portB, _portA;
		/** Whether the info of the ports has or not to be latched. */
		bool _latchAEnabled, _latchBEnabled;
		/** The output registers that are neccesary to control the ports. */
		unsigned char _outputRegB, _outputRegA;
		/** The input laches registers. */
		unsigned char _inputRegB, _inputRegA;
		/** The direction of the different ports used by the VIA. */
		unsigned char _dataPortBDir, _dataPortADir;

		// Elements related with this one...
		/** The control lines: _CA1, _CA2, CB1, _CB2 */
		VIAControlLine* _CA1;
		VIAControlLine* _CA2;
		VIAControlLine* _CB1;
		VIAControlLine* _CB2;
		/** The timers. */
		VIATimer* _timerA;
		VIATimer* _timerB;
		/** The sihift register. */
		VIAShiftRegister* _shiftRegister;

		// Implementation
		/** Timer 2 might count when a negative going pulse is detected in this bit. */
		bool _lastPB6; 
		bool _PB6Changed;
		/** Keeps always the last value read. */
		mutable MCHEmul::UByte _lastValueRead;
	};

	// ---
	inline MCHEmul::UByte VIARegisters::setPortA (const MCHEmul::UByte& v, bool f)
	{ 
		notify (MCHEmul::Event (_PORTAACTUALIZED, 
			_portA = f ? v.value () : (_outputRegA | ~_dataPortADir) & v.value ()));
							  
		return (MCHEmul::UByte (_portA)); 
	}

	// ---
	inline MCHEmul::UByte VIARegisters::setPortB (const MCHEmul::UByte& v, bool f)
	{ 
		if ((_PB6Changed = (_lastPB6 != v.bit (6))))
			_lastPB6 = v.bit (6);

		notify (MCHEmul::Event (_PORTBACTUALIZED, 
			_portB = f ? v.value () : (_outputRegB | ~_dataPortBDir) & v.value ()));

		return (MCHEmul::UByte (_portB)); 
	}

	// ---
	inline bool VIARegisters::launchInterrupt () const
	{
		return (_timerA	-> launchInterrupt () ||
				_timerB -> launchInterrupt () ||
			    _CB1 ->	launchInterrupt () ||
				_CB2 ->	launchInterrupt () ||
				_shiftRegister -> launchInterrupt () ||
				_CA1 ->	launchInterrupt () ||
				_CA2 ->	launchInterrupt ());
	}

	// ---
	inline unsigned int VIARegisters::reasonIntCode () const
	{
		return ((_timerA -> launchInterrupt () ? 1 : 0) +
				(_timerB -> launchInterrupt () ? 2 : 0) +
				(_CB1 -> launchInterrupt () ? 4 : 0) +
				(_CB2 -> launchInterrupt () ? 16 : 0) +
				(_shiftRegister -> launchInterrupt () ? 32 : 0) +
				(_CA1 -> launchInterrupt () ? 64 : 0) + 
				(_CA2 -> launchInterrupt () ? 128 : 0));
	}
}

#endif
  
// End of the file
/*@}*/
