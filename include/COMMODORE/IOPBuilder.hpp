/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: IOPBuilder.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 12/08/2022 \n
 *	Description: To build the basic peripherals connected to a COMMODORE computer.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_IOPBUILDER__
#define __COMMODORE_IOPBUILDER__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	/** It can be even extende to add new peripherals. */
	class IOPeripheralBuilder : public MCHEmul::IOPeripheralBuilder
	{
		public:
		IOPeripheralBuilder ()
			: MCHEmul::IOPeripheralBuilder ()
				{ }

		protected:
		/** This method must be overloaded to include any new type of peripherals. */
		virtual MCHEmul::IOPeripheral* createPeripheral 
			(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const override;

		/** Building ap rinter can have several definition parameters: \n
			Printer output file, device number, emulation, paper simulated used,... \n
			That's common for every commodore emulation, so a specific method have been created to get the values. \n
			The parameters have to have the following form: \n
			D:[VALUE] To define the device number. \n
			F:[VALUE] To define the name of the output file. \n
			P:[VALUE] To define the type of simulation (PSMPS801) */
		std::tuple <std::string, unsigned char, 
			MCHEmul::MatrixPrinterEmulation*> getDataPrinterFrom 
				(const MCHEmul::Attributes& prms, 
					const std::tuple <std::string, unsigned char, MCHEmul::MatrixPrinterEmulation*>& eD) const;

		/** To get the parameters related with the emultator driver. \n
			Those parameters are always in parenthesis and separated by commas and between parenthesis,
			and in the following order: \n
			[PAPER TYPE],[PAPER WIDTH],[PAPER HEIGHT],[PAPER COLOR]. \n
			The methos returns the configuration of the printer and the paper to be used. */
		std::tuple <MCHEmul::MatrixPrinterEmulation::Configuration, MCHEmul::MatrixPrinterEmulation::Paper>
			getDataPrinterEmulationFrom (const std::string& pDt) const;
	};
}

#endif
  
// End of the file
/*@}*/
