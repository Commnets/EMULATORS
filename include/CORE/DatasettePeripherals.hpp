/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: DatasettePeripherals.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 21/01/2023 \n
 *	Description: To emulate the behaviour of the the elements connected to the DatassettePort.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_DATASETTEPERIPHERALS__
#define __MCHEMUL_DATASETTEPERIPHERALS__

#include <CORE/IOPeripheral.hpp>
#include <CORE/OBool.hpp>
#include <CORE/FileIO.hpp>
#include <CORE/DtMemoryBlock.hpp>
#include <CORE/InfoStructure.hpp>
#include <thread>
#include <mutex>

namespace MCHEmul
{
	/** All datasette peripheral should inherit from this class. \n
		Just to guarantte that they can be linked to the Datasette Port. */
	class DatasettePeripheral : public MCHEmul::IOPeripheral
	{
		public:
		DatasettePeripheral (int id, const MCHEmul::Attributes& attrs);

		/** To know when some element in the status of the datasette has requested to change it status. \n
			The status might or not actually change. \n
			This variable will inform you just about the "try". \n
			Once the variable has been read it goes back to false... 
			Use peek variations of the methods, 
			if to put if back to false is not the desired behaviour. */
		bool motorChangeStatusRequested () const
							{ return (_motorChangeStatusRequest); }
		bool peekMotorChangeStatusRequested () const
							{ return (_motorChangeStatusRequest.peekValue ()); }
		bool readChangeValueRequested () const
							{ return (_readChangeValueRequest); }
		bool peekReadChangeValueRequested () const
							{ return (_readChangeValueRequest.peekValue ()); }
		bool writeChangeValueRequested () const
							{ return (_writeChangeValueRequest); }
		bool peekWriteChangeValueRequested () const
							{ return (_writeChangeValueRequest.peekValue ()); }
		unsigned int clockCyclesWhenWriteAction () const
							{ return (_clockCyclesWhenWriteAction); }
		bool keysChangedStatusRequested () const
							{ return (_keysChangedStatusRequest); }
		bool peekKeysChangedStatusRequested () const
							{ return (_keysChangedStatusRequest.peekValue ()); }

		// Managing the datasette...
		/** To move the motor. */
		void setMotorOff (bool d)
							{ _motorOff = d; _motorChangeStatusRequest = true; }
		/** Data Input. Read FROM the casette. */
		bool read () const
							{ return (_valueRead); }
		/** Data Output. Write TO the casette. \n
			The method received the value and the clock cycles when that happened. */
		inline void setWrite (bool d, unsigned int cC);
		/** Detect when one the main keys is pressed (PLAY, RECORD, F.FWD, REW). */
		bool noKeyPressed () const
							{ return (_noKeyPressed); }

		virtual bool initialize () override;
		virtual bool finalize () override
							{ return (true); }

		/** It can be overloaded later. \n
			The default accepted type of data is @see @RawFileData. */
		virtual bool connectData (MCHEmul::FileData* dt) override;

		/** The specific format could be overloaded. \n
			By default it is again the most simple one (@see @RawFileData) */
		virtual FileData* retrieveData () const override;

		/** To know whether data is or not loaded into the casette, ready to be read. */
		bool hasDataLoaded () const
							{ return (!_data._data.empty ()); }
		const ExtendedDataMemoryBlocks& data () const
							{ return (_data); }

		/**
		  *	The name of the fields are: \n
		  * The ones in the parent class. \n
		  *	ATTRS	= InfoStructure: Attributes defining the Peripheral. \n
		  */
		virtual InfoStructure getInfoStructure () const override;

		protected:
		/** To be used from the classes inherint this one. 
			The methods are the opposite ones to the public ones. */
		bool motorOff () const
							{ return (_motorOff); }
		/** The method receives the value read, 
			and the clock cycle when that happened */
		inline void setRead (bool v);
		bool valueToWrite () const
							{ return (_valueToWrite); }
		void setNoKeyPressed (bool n)
							{ _noKeyPressed = n; _keysChangedStatusRequest = true; }
		
		/** To clear the data. */
		void clearData ()
							{ _data = { }; _data._name = "COMMTYNETS"; }

		protected:
		bool _valueRead, _valueToWrite;
		unsigned int _clockCyclesWhenWriteAction;
		bool _motorOff;
		bool _noKeyPressed;

		MCHEmul::ExtendedDataMemoryBlocks _data;

		// Implementation...
		mutable OBool _motorChangeStatusRequest;
		mutable OBool _readChangeValueRequest;
		mutable OBool _writeChangeValueRequest;
		mutable OBool _keysChangedStatusRequest;
	};

	// ---
	inline void DatasettePeripheral::setWrite (bool d, unsigned int cC)
	{ 
		_valueToWrite = d; 

		_clockCyclesWhenWriteAction = cC;
		
		_writeChangeValueRequest = true; 
	}

	// ---
	inline void DatasettePeripheral::setRead (bool v)
	{ 
		_valueRead = v;

		_readChangeValueRequest = true;
	}

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
	
	/** 
	  *	Represents a standard datasette. \n
	  * A datasette (any) is a serial device, so it sends and receives data bit by bit. \n
	  * When the data is ready to be sent the method "setWrite" " has to be invoked. \n
	  * Usually it happens when a event is received from a IO Chip 
	  * connected to the IODevice (the datasette is connected ot that one). \n
	  * In a similar way, when the data is received, the method "setRead" has to be invoked. \n
	  * Usually it happens every certain number of cycles either constant or variable depending on the implementation. \n
	  * \n
	  * The datasette is connected to a datasette port,
	  * that is the final IODevice connected to the computer (ant to the computer simulation too!). \n
	  * The simulation of the datasettePort (@see DatasettePort class) checks whether a data has been read, 
	  * and if so, it notifies the event. \n
	  * \n
	  * In a StandardDatasette there are a couple of commands:
	  * _KEYFOWARD, _KEYREWIND, _KEYSTOP, _KEYPLAY, _KEYRECORD, _KEYEJECT. \n
	  * _KEYFORWARD & _KEYREWIND are used to move the tape forward or backward.
	  * _dataCounter is move up or daow and _elementCounter is reset to 0. \n
	  * _dataCounter can point to and empty datablock (after the last one). This is used to sabe new data. \n
	  * _KEYSTOP stops the tape, so no more data is read or written.
	  * If the previous status of the datasette was SAVING, 
	  * the _dataCounter is moved one position forward (_elementCounter is reset to 0). \n
	  * _KEYEJECT clears the data loaded, simulating a new and clean casette is in.
	  */
	class StandardDatasette : public DatasettePeripheral
	{
		public:
		/** This class represents the way in detail the bits are read or written. 
			The default implementation just manage an internal counter that is incremented
			through out the method clock (), but defines three methods to be implemented:
			-> timeToReadValue: To decide whether it is or not time to read a value. \n
			-> whenValueRead: What to do when a value has been read from the data file. \n
			-> valueToSaveForBit: To decide what value has to be saved for a bit. \n 
			All these methods are invoked in the simulation method. */
		class Implementation
		{
			public:
			Implementation (const MCHEmul::UByte& nV = MCHEmul::UByte::_FF)
				: _clockCyclesCounter (0),
				  _valueForNoMoreData (nV)
							{ }

			virtual ~Implementation () = default;

			/** Which is the value when there is no more data to read. */
			const MCHEmul::UByte& valueForNoMoreData () const
							{ return (_valueForNoMoreData); }

			// Controlling the clock...
			virtual void clock ()
							{ ++_clockCyclesCounter; }
			unsigned int clockValue () const
							{ return (_clockCyclesCounter); }
			virtual void resetClock ()
							{ _clockCyclesCounter = 0; }

			/// Actions when reading...
			/** To decide whether it is or not time to read a value. \n
				If it is the time, the value is returned in the parameter. 
				The method receives also the number of cycles of the CPU when this method is invoked. */
			virtual bool timeToReadValue (unsigned int cC, bool &v) const = 0;
			/** What to do when a value has been read from the data file. \n
				The method receives the number of cycles of the CPU when the value was read,
				and also the value finally read from the file. */
			virtual void whenValueRead (unsigned int cC, const MCHEmul::UByte& v) = 0;
			
			// Actions when writting...
			/** To decide whether it is or not time to write a value. \n
				Just return true when it is time, and false if it is not. \n
				The method receives the number of cycles of the CPU when the method is invoked
				and the value to be written (suggested by the logic of the program). */
			virtual bool timeToWriteValue (unsigned int cC, bool v) const = 0;
			/** What to do when a value has been writtem to the data file. \n
				The method receives the number of cycles of the CPU when the value was written to the data file
				and also the value finally written. */
			virtual void whenValueWritten (unsigned int, const MCHEmul::UByte& v) = 0;

			// Actions when writting...
			/** To decide what value has to be saved for a bit. */
			virtual MCHEmul::UByte valueToSaveForBit (bool v) const = 0;

			virtual void initialize ()
							{ resetClock (); }

			protected:
			/** To control the clock. */
			unsigned int _clockCyclesCounter;
			/** Value returned when there is no more data to read from the file. */
			MCHEmul::UByte _valueForNoMoreData;
		};

		/** A implementation to do nothing. */
		class NilImplementation final : public Implementation
		{
			public:
			NilImplementation ()
				: Implementation ()
							{ }

			virtual bool timeToReadValue (unsigned int cC, bool &v) const override
							{ v = false; return (false); } // Never is time!
			virtual void whenValueRead (unsigned int cC, const MCHEmul::UByte& v) override 
							{ } // Nothing to do...

			virtual bool timeToWriteValue (unsigned int cC, bool v) const override
							{ return (false); } // Never is time!
			virtual void whenValueWritten (unsigned int cC, const MCHEmul::UByte& v) override
							{ } // Nothing to do...

			virtual MCHEmul::UByte valueToSaveForBit (bool v) const override
							{ return (0); }
		};

		/** The commands accepted by this peripheral. \n
			They refer mainly to keys that can be pressed. \n
			Notice that combinations are possible. */
		static const int _KEYFOWARD = 1;
		static const int _KEYREWIND = 2;
		static const int _KEYSTOP   = 4;
		static const int _KEYPLAY   = 8;
		static const int _KEYRECORD = 16; // Save = RECORD + PLAY
		static const int _KEYEJECT	= 32; // To clean up the data loaded, or to simulate a new casette is inserted...
		static const int _KEYIOBASE	= 64; // 10 command to the io simulation... (64...73)
		/** The key EJECT has no value. */

		/** The object is owner of the implementation object too. */
		StandardDatasette (int id, Implementation* i, 
			bool mI, const Attributes& attrs = { });

		~StandardDatasette ();

		const Implementation* implementation () const
							{ return (_implementation); }
		Implementation* implementation ()
							{ return (_implementation); }
		// The implementation can only be changed using a protected method, so usually inside the class!

		virtual bool initialize () override;

		virtual bool executeCommand (int id, const MCHEmul::Strings& prms) override;
		virtual Strings commandDescriptions () const override
							{ return (Strings (
								{ "1:FORWARD", "2:REWIND", "4:STOP", "8:PLAY", "24:PLAY + RECORD", "32:EJECT(and clear data)" })); }

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		virtual bool connectData (MCHEmul::FileData* dt) override;

		/**
		  *	The name of the fields are: \n
		  * The ones in the parent class. \n
		  *	ATTRS	= InfoStructure: Attributes defining the Peripheral. \n
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		// Invoked from the method simulate...
		/** To execute additional things. \n
			It can be extended from the user to do additional activities when making io,
			like sound?....By default it does nothing. (it is invoked per cycle). */
		virtual void additionalSimulateActions (MCHEmul::CPU* cpu, unsigned int cC) { }
		/** Read the next value from the data file. \n
			The variable e is set to true when there is no more data to read, and
			false in any other circunstance. */
		inline MCHEmul::UByte readFromData (bool& e);
		/** Store a value in the data file */
		inline void storeInData (const MCHEmul::UByte& dt);

		/** To change the implementation, it cannot be nullptr. */
		inline void setImplementation (Implementation* i);

		protected:
		Implementation* _implementation;
		bool _motorControlledInternally;

		/** The different status that this peripheral can be in. \n
			At creating this datasette is stopped. */
		enum class Status
		{
			_STOPPED = 0,
			_READING = 1,
			_SAVING = 2
		};

		// Immplementation
		/** The status of the datasette. \n
			It can be either stopped, reading or saving. \n
			Notice that the status is not changed when the motor is off!. */
		mutable Status _status;
		// Counting which the info to write or read!
		/** value = max size_t means that there is no data pointed. */
		mutable size_t _dataCounter; 
		mutable size_t _elementCounter;
		/** The CPU Cycles last time the simulation was invoked. */
		mutable unsigned int _lastCPUCycles;
		/** Everytime the status is set back to read (PLAY Key), 
			this variable is set to true. */
		MCHEmul::OBool _firstTimeReading;
	};

	// ---
	inline MCHEmul::UByte StandardDatasette::readFromData (bool& e)
	{
		e = false;
		if (_dataCounter < _data._data.size ())
		{
			e = true;
			if (_elementCounter >= _data._data [_dataCounter].bytes ().size ())
			{
				_elementCounter = 0;

				if (++_dataCounter >= _data._data.size ())
				{
					_dataCounter = 0;

					e = false; // Overflow...
				}
			}
		}

		return (e // Without error (to avoid problems with the counter)
			? _data._data [_dataCounter].bytes ()[_elementCounter++] 
			: _implementation -> valueForNoMoreData ());
	}

	// ---
	inline void StandardDatasette::storeInData (const MCHEmul::UByte& dt)
	{
		_data._data [_dataCounter].addByte (dt);

		_elementCounter++;
	}

	// ---
	inline void StandardDatasette::setImplementation (StandardDatasette::Implementation* i)
	{
		assert (i != nullptr);

		delete (_implementation);

		_implementation = i;
	}
}

#endif
  
// End of the file
/*@}*/
