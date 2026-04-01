/** \ingroup SINCLAIR */
/*@{*/

/**	
 *	@file	
 *	File: IOPBuilder.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 24/03/2024 \n
 *	Description: To build the basic peripherals connected to a SINCLAIR computer.
 *	Versions: 1.0 Initial
 */

#ifndef __SINCLAIR_IOPBUILDER__
#define __SINCLAIR_IOPBUILDER__

#include <CORE/incs.hpp>

namespace SINCLAIR
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

		/** Building a printer can have several definition parameters: \n
			Printer output file, device number, emulation, paper simulated used,... \n
			That's common for every SINCLAIR emulation, so a specific method have been created to get the values. \n
			The parameters have to have the following form: \n
			F:[VALUE] To define the name of the output file. \n
			P:[VALUE] To define the type of simulation (THERMAL|-PS).
					  In case it doesn't exist a default one is created just to avoid crashes. */
		std::tuple <std::string, 
			MCHEmul::MatrixPrinterEmulation*> getDataPrinterFrom 
				(const MCHEmul::Attributes& prms, 
					const std::tuple <std::string, MCHEmul::MatrixPrinterEmulation*>& eD) const;
	};
}

#endif
  
// End of the file
/*@}*/
