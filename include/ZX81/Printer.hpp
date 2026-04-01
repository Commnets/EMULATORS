/** \ingroup ZX81 */
/*@{*/

/**	
 *	@file	
 *	File: Printer.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 30/03/2026 \n
 *	Description: To emulate the behaviour of the Thermal printer connected 
 *				 to a ZX81 Computer (through the Edge Connector). \n
 *	Versions: 1.0 Initial
 */

#ifndef __ZX81_PRINTER__
#define __ZX81_PRINTER__

#include <SINCLAIR/incs.hpp>
#include <ZX81/EdgeConnectorPeripherals.hpp>

namespace ZX81
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

		/** The traps in the ZX81 are different than in a ZXSpectrum e.g. */
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
		/** Address where the ZX81 keeps the LSB of the memory buffer where to 
			keep the next value received. No take into account the bit 7. */
		static MCHEmul::Address _PR_CC;
		/** Address of the memory buffer. */
		static MCHEmul::Address _PRBUFF;
		/** Character to represent the End of Line (= 0x76). */
		static const MCHEmul::UByte _EOL;
		/** Initial LSB of the memory buffer (= 0x3c). */
		static const MCHEmul::UByte _IBP;
		/** Last LSB of the memory buffer (= 0x5c). */
		static const MCHEmul::UByte _LBP;
		/** Length of the memory buffer (= _LBP - IBP = 0x20). */
		static const size_t _BLEN;

		/** A reference to the emulation. */
		MCHEmul::MatrixPrinterEmulation* _emulation;
		/** The trap where the info to the printer is caught. */
		const MCHEmul::Trap _printTrap;
	};
}

#endif
  
// End of the file
/*@}*/
