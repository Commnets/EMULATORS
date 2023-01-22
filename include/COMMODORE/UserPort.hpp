/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: UserPort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 22/05/2021 \n
 *	Description: To emulate the behaviour of the User Port. \n
 *				 Several types of User Peripherals can be connected to this port.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_USERPORT__
#define __COMMODORE_USERPORT__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	/** This class represents the UserIOPort. \n
		The class is not final because when it is attached to a specific computer,
		the links to chip o memory zones could be different. */
	class UserIOPort : public MCHEmul::IODevice
	{
		public:
		static const int _ID = 101;

		UserIOPort ();

		/** It verifies before adding it that whether the peripherial is a right one. */
		virtual bool connectPeripheral (MCHEmul::IOPeripheral* p) override;
	};
}

#endif
  
// End of the file
/*@}*/
