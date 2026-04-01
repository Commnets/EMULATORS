/** \ingroup ZXSPECTRUM */
/*@{*/

/**	
 *	@file	
 *	File: Printer.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 30/03/2026 \n
 *	Description: To emulate the behaviour of the Thermal Printer connected 
 *				 to a ZXSpectrum Computer (through the Edge Connector). \n
 *	Versions: 1.0 Initial
 */

#ifndef __ZXSPECTRUM_PRINTER__
#define __ZXSPECTRUM_PRINTER__

#include <SINCLAIR/incs.hpp>
#include <ZXSpectrum/EdgeConnectorPeripherals.hpp>

namespace ZXSPECTRUM
{
	/** The Thermal Printer simulation. */
	class ThermalPrinterSimulation final : public EdgeConnectorPeripheral
	{
		public:
		static const int _ID = 102;

		ThermalPrinterSimulation (MCHEmul::MatrixPrinterEmulation* mPE = 
			new MCHEmul::BasicMatrixPrinterEmulation (32, "Printer.txt"));

		/** Open & close the output file. */
		virtual bool initialize () override
							{ return (_emulation -> initialize ()); }
		virtual bool finalize () override
							{ return (_emulation -> finalize ()); }

		/** To execute a command.
			The commands are:
			1: FLUSH, to send the content still kept in the buffer to the printer file. */
		virtual bool executeCommand (int cId, const MCHEmul::Strings& prms) override;
		/** To get the commands to manage the device, if any. */
		virtual MCHEmul::Strings commandDescriptions () const
							{ return (MCHEmul::Strings (
								{ "1:FLUSH",
								  "2:NEW PAGE" })); }

		/** The traps in the ZXSpectrum are different than in a ZX81 e.g. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugSimulation (MCHEmul::CPU* cpu);
		// -----

		private:
		/** A reference to the emulation. */
		MCHEmul::MatrixPrinterEmulation* _emulation;
		/** The trap where the info to the printer is caught. */
		const MCHEmul::Trap _printTrap;
	};
}

#endif
  
// End of the file
/*@}*/
