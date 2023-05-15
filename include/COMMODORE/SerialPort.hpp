/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: SerialPort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
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
	/** This class represents the SerialIOPort. \n
		The class is not final because when it is attached to a specific computer,
		the links to chip o memory zones could be different. */
	class SerialIOPort : public MCHEmul::IODevice
	{
		public:
		static const int _ID = 103;

		SerialIOPort ();

		/** It verifies before adding it that whether the peripherial is a right one. */
		virtual bool connectPeripheral (MCHEmul::IOPeripheral* p) override;
	};
}

#endif
  
// End of the file
/*@}*/
