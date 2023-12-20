/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: VIA.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 17/12/2023 \n
 *	Description: The VIA Chip.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_VIA__
#define __COMMODORE_VIA__
#include <CORE/incs.hpp>
#include <COMMODORE/VIARegisters.hpp>

namespace COMMODORE
{
	/** The chip is a I/O Chip, with different uses. */
	class VIA : public MCHEmul::Chip
	{
		public:
		static const unsigned int _ID = 107;
		
		/** Receives the id of the chip in the computer, the id of the registers linked
			and the id of the interruption to launch when needed. \n
			In a computer there could be many of these with little differences. */
		VIA (int id, int rId, unsigned int intId);

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
		  *	TODO
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		protected:
		const int _registersId;
		VIARegisters* _VIARegisters;
		unsigned int _interruptId;

		// Implementation
		unsigned int _lastClockCycles;
	};
}

#endif
  
// End of the file
/*@}*/
