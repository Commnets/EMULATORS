/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: SerialIOPeripherals.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 15/05/2023 \n
 *	Description: To emulate the behaviour of the elements connected to the SerialIO Port.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_SERIALIOPERIPHERALS__
#define __COMMODORE_SERIALIOPERIPHERALS__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	/** All SerialIO peripherals should inherit from this class. \n
		Just to guarantte that they can be linked to the SerialIO Port. */
	class SerialIOPeripheral : public MCHEmul::IOPeripheral
	{
		public:
		SerialIOPeripheral (int id, const MCHEmul::Attributes& attrs)
			: MCHEmul::IOPeripheral (id, attrs)
							{ }
	};

	/** Represents nothing connected. */
	class NoSerialIOPeripheral : public SerialIOPeripheral
	{
		public:
		static const int _ID = -103;

		NoSerialIOPeripheral ()
			: SerialIOPeripheral (_ID, { }) // A nothing identificator...
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
