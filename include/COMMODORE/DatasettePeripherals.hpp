/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: DatasettePeripherals.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 21/01/2023 \n
 *	Description: To emulate the behaviour of the the elements connected to the DatassettePort.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_DATASETTEPERIPHERALS__
#define __COMMODORE_DATASETTEPERIPHERALS__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	/** All datasette peripheral shopuld inherit from this class. \n
		Just to guarantte that they can be linked to the Datasette Port. */
	class DatasettePeripheral : public MCHEmul::IOPeripheral
	{
		public:
		DatasettePeripheral (int id, const MCHEmul::Attributes& attrs)
			: MCHEmul::IOPeripheral (id, attrs)
							{ }

		// Managing the datasette...
		virtual bool pinD4 () const = 0;
		virtual void pintE5 (bool d) = 0;
		virtual bool pinF6 () const = 0;
	};

	/** Represents nothing connected. */
	class NoDatasettePeripheral : public DatasettePeripheral
	{
		public:
		static const int _ID = -101;

		NoDatasettePeripheral ()
			: DatasettePeripheral (_ID, { }) // A nothing identificator...
							{ }

		virtual bool pinD4 () const override
							{ return (false); }
		virtual void pintE5 (bool d) override
							{ }
		virtual bool pinF6 () const override
							{ return (false); }

		virtual bool initialize () override
							{ return (true); }

		virtual bool simulate (MCHEmul::CPU* cpu) override
							{ return (true); }
	};
}

#endif
  
// End of the file
/*@}*/
