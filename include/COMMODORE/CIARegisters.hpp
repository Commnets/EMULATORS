/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: CIA2Registers.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 15/05/2021 \n
 *	Description: CIA Registers Emulation
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_CIAREGISTERS__
#define __COMMODORE_CIAREGISTERS__

#include <CORE/incs.hpp>
#include <COMMODORE/CIATimer.hpp>
#include <COMMODORE/CIAClock.hpp>
#include <COMMODORE/CIASerialPort.hpp>

namespace COMMODORE
{
	class CIA;
	class CIATimer;
	class CIAClock;
	class CIASerialPort;

	/** In the CIA Memory, there are a couple of records that behave different
		when they are read that when they are written. \n
		Read this document for better understanding: \n
		http://archive.6502.org/datasheets/mos_6526_cia_recreated.pdf \n
		One of the most important characteristics of the CIA is the port management. \n
		The CIA manages two ports independently: Port A (PA) and Port B (PB). \n
		Initially the ports are not connected to anything so any state could be read, but the documentation 
		of the chip says that tha 1 is read. \n
		One something is connected the state might by determined by the device connected 
		(in the case of the keyboard it is not the case, e.g.). \n
		So when output a value the pins not affected has to maintain the value they have,
		ans when reading a value, the pins not affected will be read as 1. \n
		*/
	class CIARegisters : public MCHEmul::ChipRegisters
	{
		public:
		friend CIA;

		// Some events.
		/** When the ports are actualized. \n
			The parameter is the value (1 or 0) of the actualization. */
		static const unsigned int _PORTAACTUALIZED = 210;
		static const unsigned int _PORTBACTUALIZED = 211;

		CIARegisters (int id, MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		virtual size_t numberRegisters () const override
							{ return (0x10); }

		virtual void initialize () override;

		// managing the ports...
		/** Read the value. */
		MCHEmul::UByte portA () const
							{ return (MCHEmul::UByte (_portA)); }
		/** Put a new value into the port. \n
			It can be done "forcing" the value into the port or taking into account the _dataPortDir value. \n
			Forcing can happen when something with a mandatory value is connected to the port. \n
			e.g if a joystick is connected to the pin, when moving, the right switch is grounded and the mandatory value is 0. \n
			In the other hand a keyboard matrix is not connected to anything, so the new values can not be forced. \n
			When not forced: \n
			The values at 1 of the port are maintained at the pins where the dataPortDir is in read (0) mode.
			The values selected are pushed to the port at the pins where the dataPortDir is in write (1) mode.
			The final value of the port is returned. \n
			http://archive.6502.org/datasheets/mos_6526_cia_recreated.pdf
			By default the value is not forced. */
		MCHEmul::UByte setPortA (const MCHEmul::UByte& v, bool f = false)
							{ notify (MCHEmul::Event (_PORTAACTUALIZED, 
								_portA = f ? v.value () : (_outputRegA | ~_dataPortADir) & v.value ()));
							  return (MCHEmul::UByte (_portA)); }
		/** Same than port A. */
		MCHEmul::UByte portB () const
							{ return (MCHEmul::UByte (_portB)); }
		MCHEmul::UByte setPortB (const MCHEmul::UByte& v, bool f = false)
							{ notify (MCHEmul::Event (_PORTBACTUALIZED, 
								_portB = f ? v.value () : (_outputRegB | ~_dataPortBDir) & v.value ()));
							  return (MCHEmul::UByte (_portB)); }

		// The output registers...
		/** Their values are used internally to operate ports. */
		unsigned char outputRegisterA () const
							{ return (_outputRegA); }
		unsigned char outputRegisterB () const
							{ return (_outputRegB); }

		// All these method are invoked from CIA emulation
		/** To know the value of the data direction registers. */
		unsigned char dataPortADir () const
							{ return (_dataPortADir); }
		unsigned char dataPortBDir () const
							{ return (_dataPortBDir); }

		// Managing the FLAG line and its interruptions...
		/** To know whether the interruptions related with the FLAG line are or not allowed. */
		bool flagLineInterruptEnabled () const
							{ return (_flagLineInterruptEnabled); }
		void setFlagLineInterruptEnabled (bool fL)
							{ _flagLineInterruptEnabled = fL; }
		/** To know when the FLAG line is enabled. 
			Once the value is got then it is pit back t false. */
		bool flagLineInterruptRequested () const
							{ bool r = _flagLineInterruptRequested; _flagLineInterruptRequested = false; return (r); }
		/** Same than the previous method but without modifying the value. */
		bool peekFlagLineInterruptRequested () const
							{ return (_flagLineInterruptRequested); }

		void setFlagLineInterruptRequested (bool a)
							{ _flagLineInterruptRequested = a; }
		/** To know whether a interruption related with the FLAG line has or not to be launched. */
		bool launchFlagLineInterruption () const
							{ return (_flagLineInterruptEnabled && _flagLineInterruptRequested); }

		/** To know whether the interruption has or not to be launched. */
		inline bool launchInterruption () const;

		/** The value to reflect at the bit 6 or 7 of the port B. */
		/** To know first whether the timer A is affecting or not to the bit 6. */
		bool reflectTimerAAtPortDataB () const
							{ return (_reflectTimerAAtPortDataB); }
		/** To knwo the value affecting. */
		bool timerAValueAtPortDataB () const
							{ return (_timerAValueAtPortDataB); }
		/** In case that the timer is affecting, to know whic is the value affecting. */
		void setReflectTimerAAtPortDataB (bool r, bool v = false /** it is not taken into account when r == false. */)
							{ if (_reflectTimerAAtPortDataB = r) 
								setValue (0x01, readValue (0x01)); // Reflects the value if needed a notification can also happen...
							  _timerAValueAtPortDataB = v; }
		// Same for timer B...
		bool reflectTimerBAtPortDataB () const
							{ return (_reflectTimerBAtPortDataB); }
		bool timerBValueAtPortDataB () const
							{ return (_timerBValueAtPortDataB); }
		void setReflectTimerBAtPortDataB (bool r, bool v = false)
							{ if (_reflectTimerBAtPortDataB = r)
								setValue (0x01, readValue (0x01));
							  _timerBValueAtPortDataB = v; }

		protected:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;
		virtual const MCHEmul::UByte& peekValue (size_t p) const override;

		// Implementation
		virtual void initializeInternalValues ();

		// This methods are invoked from CIA chip...
		/** The registers will observe also the timers. */
		void lookAtTimers (CIATimer* tA, CIATimer* tB)
							{ _timerA = tA, _timerB = tB; }
		/** ...and also of the clock. */
		void lookAtClock (CIAClock* c)
							{ _clock = c; }
		/** ..al also to the CIA Serial Port. */
		void lookAtSerialPort (CIASerialPort* sp)
							{ _serialPort = sp; }

		protected:
		/** Ports used by the CIA. */
		mutable unsigned char _portA, _portB;
		/** The output registers that are not neccesary iqual to the ports. */
		mutable unsigned char _outputRegA, _outputRegB;
		/** The direction of the different ports used by the CIA. */
		unsigned char _dataPortADir, _dataPortBDir;
		/** Reference to the timers */
		CIATimer* _timerA;
		CIATimer* _timerB;
		/** Reference to the clock */
		CIAClock* _clock;
		/** Reference to the CIA Serial Port. */
		CIASerialPort *_serialPort;
		/** To indicate whether the FLAG line interruption is or not allowed. */
		bool _flagLineInterruptEnabled;
		/** To indicate that the FLAG line has been or not activated. */
		mutable bool _flagLineInterruptRequested;

		// Implementation
		mutable MCHEmul::UByte _lastValueRead;
		bool _reflectTimerAAtPortDataB, _timerAValueAtPortDataB;
		bool _reflectTimerBAtPortDataB, _timerBValueAtPortDataB;
	};

	// ---
	inline bool CIARegisters::launchInterruption () const
	{
		return (_timerA	-> launchInterruption () ||
				_timerB -> launchInterruption () ||
				_clock  -> launchInterruption () ||
				_serialPort -> interruptRequested () ||
				launchFlagLineInterruption ());
	}
}

#endif
  
// End of the file
/*@}*/
