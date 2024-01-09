/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VIA.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 17/12/2023 \n
 *	Description: The VIA Chip.
 *				 @see https://web.archive.org/web/20160108173129if_/http://archive.6502.org/datasheets/mos_6522_preliminary_nov_1977.pdf
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_VIA__
#define __COMMODORE_VIA__

#include <CORE/incs.hpp>
#include <COMMODORE/VIA/VIARegisters.hpp>
#include <COMMODORE/VIA/VIAControlLine.hpp>
#include <COMMODORE/VIA/VIATimer.hpp>
#include <COMMODORE/VIA/VIAShiftRegister.hpp>

namespace COMMODORE
{
	class VIA : public MCHEmul::Chip
	{
		public:
		static const unsigned int _ID = 107;

		/** Receives the id of the chip in the computer, the id of the registers linked
			and the id of the interruption to launch when needed. \n
			In a computer there could be many of these with little differences. */
		VIA (int id, int rId, unsigned int intId);

		/** To allow the external conexion. */
		const MCHEmul::Wire& CA1 () const
							{ return (_CA1.wire ()); }
		MCHEmul::Wire& CA1 ()
							{ return (_CA1.wire ()); }
		const MCHEmul::Wire& CA2 () const
							{ return (_CA2.wire ()); }
		MCHEmul::Wire& CA2 ()
							{ return (_CA2.wire ()); }
		const MCHEmul::Wire& CB1 () const
							{ return (_CB1.wire ()); }
		MCHEmul::Wire& CB1 ()
							{ return (_CB1.wire ()); }
		const MCHEmul::Wire& CB2 () const
							{ return (_CB2.wire ()); }
		MCHEmul::Wire& CB2 ()
							{ return (_CB2.wire ()); }

		// Managing the ports...
		const MCHEmul::UByte& portA () const
							{ return (_VIARegisters -> portA ()); }
		MCHEmul::UByte setPortA (const MCHEmul::UByte& v, bool f = false)
							{ return (_VIARegisters -> setPortA (v, f)); }
		const MCHEmul::UByte& portB () const
							{ return (_VIARegisters -> portB ()); }
		MCHEmul::UByte setPortB (const MCHEmul::UByte& v, bool f = false)
							{ return (_VIARegisters -> setPortB (v, f)); }

		virtual bool initialize () override;

		// Just to add debug info when active...
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * The ones comming from the parent class. \n
		  * Registers	= InfoStructure: Value of the registers. \n
		  *	VIATimerA	= InfoStructure: Timer A info. \n
		  *	VIATimerB	= InfoStructure: Timer B info. \n
		  *	VIAShiftReg	= InfoStructure: Shift Register info. \n
		  *	PortA		= Attribute: Port A value. \n
		  *	PortB		= Attribite: Port B value. \n
		  * CA1			= InfoStructure: Control line A1 info.
		  * CA2			= InfoStructure: Control line A2 info.
		  * CB1			= InfoStructure: Control line B1 info.
		  * CB2			= InfoStructure: Control line B2 info.
		  *	VIADDRA		= Attribute: Port A direction. \n
		  *	VIADDRB		= Attribute: Port B direction.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		const int _registersId;
		VIARegisters* _VIARegisters;
		unsigned int _interruptId;
		// The different elements of the VIA Chip...
		VIATimer _timer1, _timer2;
		VIAControlLine _CA1, _CA2, _CB1, _CB2;
		VIAShiftRegister _shiftRegister;

		// Implementation
		unsigned int _lastClockCycles;
	};
}

#endif
  
// End of the file
/*@}*/
