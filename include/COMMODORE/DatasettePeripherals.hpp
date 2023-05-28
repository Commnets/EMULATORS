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
		DatasettePeripheral (int id, const MCHEmul::Attributes& attrs);

		// To know when some element has requested to change it status.
		//	The status might or not changed finally. This variable will inform you just about the "try"
		// Once the variable has been read it goes back to false...
		bool motorChangeStatusRequested () const
							{ bool r = _motorChangeStatusRequest; _motorChangeStatusRequest = false; return (r); }
		bool readChangeValueRequested () const
							{ bool r = _readChangeValueRequest; _readChangeValueRequest = false; return (r); }
		bool writeChangeValueRequested () const
							{ bool r = _writeChangeValueRequest; _writeChangeValueRequest = false; return (r); }
		bool keysChangedStatusRequested () const
							{ bool r = _keysChangedStatusRequest; _keysChangedStatusRequest = false; return (r); }

		// Managing the datasette...
		// The important PINS in the datasette are:
		/** PINC3	: MOTOR CONTROL = To move the motor. */
		void setMotorOff (bool d)
							{ _motorOff = d; _motorChangeStatusRequest = true; }
		/** PIND4	: READ = Data Input. Read FROM the casette. */
		bool read () const
							{ return (_valueRead); }
		/** PINE5	: WRITE = Data Output. Write TO the casette. */
		void setWrite (bool d)
							{ _valueToWrite = d; _writeChangeValueRequest = true; }
		/** PINF6	: SENSE = Detect when one the main keys is pressed (PLAY, RECORD, F.FWD, REW). */
		bool noKeyPressed () const
							{ return (_noKeyPressed); }

		virtual bool initialize () override;
		virtual bool finalize () override
							{ return (true); }

		virtual bool connectData (MCHEmul::FileData* dt) override;

		/** The file type returned is always RAW (@see RawFileData at FileIO.hpp in COMMODORE. */
		virtual MCHEmul::FileData* retrieveData () const override;

		/** To know whether data is or not loaded into the casette, ready to be read. */
		bool hasDataLoaded () const
							{ return (!_data._data.empty ()); }
		const MCHEmul::ExtendedDataMemoryBlocks& data () const
							{ return (_data); }

		/**
		  *	The name of the fields are: \n
		  * The ones in the parent class. \n
		  *	ATTRS	= InfoStructure: Attributes defining the Peripheral. \n
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		/** To be used from the classes inherint this one. 
			The methods are the opposite ones to the public ones. */
		bool motorOff () const
							{ return (_motorOff); }
		void setRead (bool v)
							{ _valueRead = v; _readChangeValueRequest = true; }
		bool valueToWrite () const
							{ return (_valueToWrite); }
		void setNoKeyPressed (bool n)
							{ _noKeyPressed = n; _keysChangedStatusRequest = true; }
		
		/** To clear the data. */
		void clearData ()
							{ _data = { }; }

		protected:
		bool _valueRead, _valueToWrite;
		bool _motorOff;
		bool _noKeyPressed;

		MCHEmul::ExtendedDataMemoryBlocks _data;

		// Implementation.
		mutable bool _motorChangeStatusRequest;
		mutable bool _readChangeValueRequest;
		mutable bool _writeChangeValueRequest;
		mutable bool _keysChangedStatusRequest;
	};

	/** Represents nothing connected. */
	class NoDatasettePeripheral : public DatasettePeripheral
	{
		public:
		static const int _ID = -100;

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
