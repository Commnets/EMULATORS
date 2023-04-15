/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: CIA.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 14/01/2023 \n
 *	Description: The CIA Chip.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_CIA__
#define __COMMODORE_CIA__

#include <CORE/incs.hpp>
#include <COMMODORE/CIARegisters.hpp>
#include <COMMODORE/CIATimer.hpp>
#include <COMMODORE/CIAClock.hpp>
#include <COMMODORE/CIASerialPort.hpp>

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
		void setPortA (const MCHEmul::UByte& v)
							{ _CIARegisters -> setPortA (v); }
		const MCHEmul::UByte& portB () const
							{ return (_CIARegisters -> portB ()); }
		void setPortB (const MCHEmul::UByte& v)
							{ _CIARegisters -> setPortB (v); }

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * The ones comming from the parent class. \n
		  * Registers	= InfoStructure: Value of the registers. \n
		  *	TimerA		= InfoStructure: Info about the timer A. \n
		  *	TimerB		= InfoStructure: Info about the timer B. \n
		  *	Clock		= InfoStructure: Info about the clock. \n
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		protected:
		CIARegisters* _CIARegisters;
		const int _registersId;
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
