/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: SerialPort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 15/05/2023 \n
 *	Description: To emulate the behaviour of the Serial Port. \n
 *				 Several types of User Peripherals can be connected to this port.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_SERIALPORT__
#define __COMMODORE_SERIALPORT__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	class SerialIOPeripheral;

	/** This class represents the SerialIOPort. \n
		The class is not final because when it is attached to a specific computer,
		the links to chip o memory zones could be different. \n
		In the commodore serial port there might be many peripherals connected. */
	class SerialIOPort : public MCHEmul::IODevice
	{
		public:
		static const int _ID = 103;

		/** The constructor might receive a default peripheral.
			NOTE: That peripheral would have to be executed at the end of the simulation.
			So the ID of ot should be the max possible. */
		SerialIOPort (SerialIOPeripheral* nS = nullptr);

		/** It verifies before adding it that whether the peripherial is a right one. */
		virtual bool connectPeripheral (MCHEmul::IOPeripheral* p) override;
	};
}

#endif
  
// End of the file
/*@}*/
