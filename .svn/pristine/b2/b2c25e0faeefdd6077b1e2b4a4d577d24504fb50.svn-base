/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: UserPort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 22/05/2021 \n
 *	Description: To emulate the behaviour of the User Port.
 *				 Several types of User Peripherals can be connected to this port, 
 *				 but all of them bust inherit from UserIOPeripheral.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_USERPORT__
#define __C64_USERPORT__

#include <CORE/incs.hpp>

namespace C64
{
	/** This class represents the UserIOPort. */
	class UserIOPort final : public MCHEmul::IODevice
	{
		public:
		static const int _ID = 3;

		UserIOPort ();

		/** It verifies before adding it that whether the Peripherial is a UserIOPeripherial,
			as they are the only ones compatible with this type of Port. */
		virtual void addPeripheral (MCHEmul::IOPeripheral* p) override;
	};
}

#endif
  
// End of the file
/*@}*/
