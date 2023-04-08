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

namespace COMMODORE
{
	class CIA;
	class CIATimer;
	class CIAClock;

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

		/** Some events. */
		static const unsigned int _PORTAACTUALIZED = 300;
		static const unsigned int _PORTBACTUALIZED = 301;

		CIARegisters (int id, MCHEmul::PhysicalStorage* ps, size_t pp, const MCHEmul::Address& a, size_t s);

		virtual size_t numberRegisters () const override
							{ return (0x10); }

		virtual void initialize () override;

		// All these method are invoked from CIA emulation
		/** Managing the ports. */
		const MCHEmul::UByte& portA () const
							{ return (_portA); }
		void setPortA (const MCHEmul::UByte& v)
							{ _portA = v.value (); }
		const MCHEmul::UByte& portB () const
							{ return (_portB); }
		void setPortB (const MCHEmul::UByte& v)
							{ _portB = v.value (); }

		/** To know when the flag line is enabled. 
			Once the value is got then it is pit back t false. */
		bool flagLineInterruptRequested () const
							{ bool r = _flagLineInterruptRequested; _flagLineInterruptRequested = false; return (r); }
		void setFlagLineInterruptRequested (bool a)
							{ _flagLineInterruptRequested = a; }

		/** A potential value to be reflected at the bit 6 of the Port B:
			0: don't do anything. \n
			1: true. \n 
			2: false. */
		unsigned int reflectTimerAAtPortDataB () const
							{ return (_reflectTimerAAtPortDataB); }
		void setReflectTimerAAtPortDataB (unsigned int w)
							{ _reflectTimerAAtPortDataB = w; }
		unsigned int reflectTimerBAtPortDataB () const
							{ return (_reflectTimerBAtPortDataB); }
		void setReflectTimerBAtPortDataB (unsigned int w)
							{ _reflectTimerBAtPortDataB = w; }

		protected:
		virtual void setValue (size_t p, const MCHEmul::UByte& v) override;
		virtual const MCHEmul::UByte& readValue (size_t p) const override;

		// Implementation
		virtual void initializeInternalValues ();

		// This methods are invoked from CIA chip...
		/** The registers will observe also the timers. */
		void lookAtTimers (CIATimer* tA, CIATimer* tB)
							{ _timerA = tA, _timerB = tB; }
		/** ...and also of the clock. */
		void lookAtClock (CIAClock* c)
							{ _clock = c; }

		protected:
		/** Ports used by the CIA. */
		unsigned char _portA, _portB;
		/** The direction of the different ports used by the CIA. */
		unsigned char _dataPortADir, _dataPortBDir;
		/** Reference to the timers */
		CIATimer* _timerA;
		CIATimer* _timerB;
		/** Reference to the clock */
		CIAClock* _clock;
		/** To indicate that the flag line has been or not activated. */
		mutable bool _flagLineInterruptRequested;

		// Implementation
		mutable MCHEmul::UByte _lastValueRead;
		unsigned int _reflectTimerAAtPortDataB;
		unsigned int _reflectTimerBAtPortDataB;
	};
}

#endif
  
// End of the file
/*@}*/
