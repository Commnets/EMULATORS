/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: CIA.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 14/01/2023 \n
 *	Description: The CIA Chip.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_CIA__
#define __COMMODORE_CIA__

#include <CORE/incs.hpp>
#include <COMMODORE/CIA/CIARegisters.hpp>
#include <COMMODORE/CIA/CIATimer.hpp>
#include <COMMODORE/CIA/CIAClock.hpp>
#include <COMMODORE/CIA/CIASerialPort.hpp>

namespace COMMODORE
{
	/** The chip is a I/O Chip, with different uses. */
	class CIA : public MCHEmul::Chip
	{
		public:
		static const unsigned int _ID = 102;

		// Some events.
		/** The CNT and SP signals are sused by many elements within the CIA. \n
			This is the reason for them to be defined here. \n/
			In C64 e.g. the CNT signal is received from the UserPort mainly, but it is output from the SerialPort. */
		static const unsigned int _CNTSIGNAL	= 220;
		/** In the C64 the SP signal is received from the UserPort and it is also an output from the SerialPort. */
		static const unsigned int _SPSIGNAL		= 221;
		
		/** Receives the id of the chip in the computer, the id of the registers linked
			and the if og the interruption to launch when needed. \n
			In a computer there could be many of these with little differences. */
		CIA (int id, int rId, unsigned int intId);

		// Managing the ports...
		const MCHEmul::UByte& portA () const
							{ return (_CIARegisters -> portA ()); }
		MCHEmul::UByte setPortA (const MCHEmul::UByte& v, bool f = false)
							{ return (_CIARegisters -> setPortA (v, f)); }
		const MCHEmul::UByte& portB () const
							{ return (_CIARegisters -> portB ()); }
		MCHEmul::UByte setPortB (const MCHEmul::UByte& v, bool f = false)
							{ return (_CIARegisters -> setPortB (v, f)); }

		virtual bool initialize () override;

		// Just to add debug info when active...
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * The ones comming from the parent class. \n
		  * Registers	= InfoStructure: Value of the registers. \n
		  *	TimerA		= InfoStructure: Info about the timer A. \n
		  *	TimerB		= InfoStructure: Info about the timer B. \n
		  *	Clock		= InfoStructure: Info about the clock. \n
		  * SerialPort	= InfoStructure: Info about the serial port. \n
		  * PortA		= Attribute:	 Info about the value at the port A. \n
		  * PortB		= Attribute:	 Info about the value at the port B. \n
		  * CIDDRA		= Attribute:	 The direction of the port A. \n
		  * CIDDRB		= Attribute:	 The direction of the port B. \n
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugCIACycle (MCHEmul::CPU* cpu, unsigned int i);
		// -----

		protected:
		const int _registersId;
		CIARegisters* _CIARegisters;
		unsigned int _interruptId;
		CIATimer _timerA, _timerB;
		CIAClock _clock;
		CIASerialPort _serialPort;

		// Implementation
		unsigned int _lastClockCycles;
		bool _pulseTimerASentToPortB, _pulseTimerBSentToPortB;
	};
}

#endif
  
// End of the file
/*@}*/
