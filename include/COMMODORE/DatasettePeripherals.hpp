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
			: MCHEmul::IOPeripheral (id, attrs),
			  _valueRead (true),
			  _valueToWrite (true),
			  _motorOff (true),
			  _noKeyPressed (true),
			  _newValueReceived (false)
							{ }

		// Managing the datasette...
		// The important PINS in the datasette are:
		/** PINC3	: MOTOR CONTROL = To move the motor. */
		virtual void setMotorOff (bool d)
							{ _motorOff = d; }
		/** PIND4	: READ = Data Input. Read FROM the casette. */
		bool read () const
							{ return (_valueRead); }
		/** PINE5	: WRITE = Data Output. Write TO the casette. */
		virtual void setWrite (bool d)
							{ _newValueReceived = true; _valueToWrite = d; }
		/** PINF6	: SENSE = Detect when one the main keys is pressed (PLAY, RECORD, F.FWD, REW). */
		virtual bool noKeyPressed () const
							{ return (_noKeyPressed); }

		virtual bool initialize () override;

		protected:
		volatile bool _valueRead, _valueToWrite;
		volatile bool _motorOff;
		volatile bool _noKeyPressed;

		// Implementation
		bool _newValueReceived;
	};

	/** Represents nothing connected. */
	class NoDatasettePeripheral : public DatasettePeripheral
	{
		public:
		static const int _ID = -101;

		NoDatasettePeripheral ()
			: DatasettePeripheral (_ID, { }) // A nothing identificator...
							{ }

		virtual bool simulate (MCHEmul::CPU* cpu) override
							{ return (true); } // Always right but nothing is done with the variations of the internal data...
	};
}

#endif
  
// End of the file
/*@}*/
