/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: UserIOPeripherals.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 21/01/2023 \n
 *	Description: To emulate the behaviour of the elements connected to the UserIO Port.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_USERIOPERIPHERALS__
#define __COMMODORE_USERIOPERIPHERALS__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	/** All UserIO peripherals should inherit from this class. \n
		Just to guarantte that they can be linked to the UserIO Port. */
	class UserIOPeripheral : public MCHEmul::IOPeripheral
	{
		public:
		UserIOPeripheral (int id, const MCHEmul::Attributes& attrs)
			: MCHEmul::IOPeripheral (id, attrs)
							{ }
	};

	/** Represents nothing connected. */
	class NoUserIOPeripheral : public UserIOPeripheral
	{
		public:
		static const int _ID = -101;

		NoUserIOPeripheral ()
			: UserIOPeripheral (_ID, { }) // A nothing identificator...
							{ }

		virtual bool initialize () override
							{ return (true); }
		virtual bool finalize () override
							{ return (true); }

		virtual bool simulate (MCHEmul::CPU* cpu) override
							{ return (true); }
	};
}

#endif
  
// End of the file
/*@}*/
