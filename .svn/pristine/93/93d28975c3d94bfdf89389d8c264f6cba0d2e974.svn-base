/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: UserPeripherials.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/08/2022 \n
 *	Description: To emulate the behaviour of the most classical peripherials connected top the User Port.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_USERPERIPHERIALS__
#define __C64_USERPERIPHERIALS__

#include <CORE/incs.hpp>

namespace C64
{
	/** Represents any peripheral connected to the User Port. */
	class UserIOPeripheral : public MCHEmul::IOPeripheral
	{
		public:
		UserIOPeripheral (int id)
			: MCHEmul::IOPeripheral (id, { })
							{ }
	};

	/** Represents a no device, just for simulation purposes. 
		This is the one created when no other is defined for the commodore 64. */
	class UserIONoPeripheral final : public UserIOPeripheral
	{
		public:
		static const int _ID = -1;

		UserIONoPeripheral ()
			: UserIOPeripheral (_ID)
							{ }

		virtual bool initialize () override;

		virtual bool simulate () override;
	};
}

#endif
  
// End of the file
/*@}*/
