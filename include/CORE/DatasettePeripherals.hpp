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

namespace MCHEmul
{
	/** All datasette peripheral should inherit from this class. \n
		Just to guarantte that they can be linked to the Datasette Port. */
	class DatasettePeripheral : public IOPeripheral
	{
		public:
		DatasettePeripheral (int id, const Attributes& attrs);

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
		virtual bool connectData (FileData* dt) override;

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

		ExtendedDataMemoryBlocks _data;

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

		virtual bool simulate (CPU* cpu) override
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
		/** The different status that this peripheral can be in. \n
			At creating this datasette is stopped. */
		enum class Status
		{
			_STOPPED = 0,
			_READING = 1,
			_SAVING = 2
		};

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
			Implementation (const UByte& nV = UByte::_FF)
				: _clockCyclesCounter (0),
				  _valueForNoMoreData (nV)
							{ }

			virtual ~Implementation () = default;

			/** Which is the value when there is no more data to read. */
			const UByte& valueForNoMoreData () const
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
				The method returns always a tuple with 3 values:
				<0>: Whether it is the moment to simulate that a new value has been read from the datasette.
				<1>: If the previous one is true, this value will point the value read
				<2>: Whether it is time to also read a new value from the data file. */
			virtual std::tuple <bool, bool, bool> timeToReadValue (unsigned int cC) = 0;
			/** What to do when a value has been read from the data file. \n
				The method receives the number of cycles of the CPU when the value was read,
				and also the value finally read from the file. */
			virtual void whenValueRead (unsigned int cC, const UByte& v) = 0;
			/** Something, just when reading, the block can change. \n
				In this situations a decision can be needed in the content of the Implementation. \m
				The new block is passed as parameter. \n
				By default, nothing is done. */
			virtual void whenReadingNewBlock (const DataMemoryBlock& dB) { }
			
			// Actions when writting...
			/** To decide whether it is or not time to write a value. \n
				The parameters needed are:
				@param cC	: Cycles of the system when the method is invoked.
				@param vc	: Has changed the value sent to the peripheral?
				@param ccvc	: Cycles when the request of change was invoked (the previous parameter is true).
				@oaram vs   : What was the value sent? \m
				The method has to return a tuple when two fields: \n
				<0> to indicate whether a new value has to be saved into the data file,
				<1> to indicate that value, */
			virtual std::tuple <bool, UByte> timeToWriteValue (unsigned int cC, 
				bool vc, unsigned int ccvc, bool vs) = 0;
			/** What to do when a value has been writtem to the data file. \n
				The method receives the number of cycles of the CPU when the value was written to the data file
				and also the value finally written. */
			virtual void whenValueWritten (unsigned int, const UByte& v) = 0;
			/** This method is invoked when a new block is created. 
				By default nothing is done. */
			virtual void whenCreatingNewBlock (const DataMemoryBlock& dB) { }

			/** The initialization is always with the status. 
				That can or cannot be taken into account by the method. \n
				By default it just reset the counter. */
			virtual void initialize (Status st)
							{ resetClock (); }

			protected:
			/** To control the clock. */
			unsigned int _clockCyclesCounter;
			/** Value returned when there is no more data to read from the file. */
			UByte _valueForNoMoreData;
		};

		/** A implementation to do nothing. */
		class NilImplementation final : public Implementation
		{
			public:
			NilImplementation ()
				: Implementation ()
							{ }

			virtual std::tuple <bool, bool, bool> timeToReadValue (unsigned int cC) override
							{ return { false, false, false }; } // Never is time!
			virtual void whenValueRead (unsigned int cC, const UByte& v) override 
							{ } // Nothing to do...

			virtual std::tuple <bool, UByte> timeToWriteValue (unsigned int cC, 
				bool vc, unsigned int ccvc, bool vs) override
							{ return { false, MCHEmul::UByte::_0 }; } // Never is time!
			virtual void whenValueWritten (unsigned int cC, const UByte& v) override
							{ } // Nothing to do...
		};

		/** The commands accepted by this peripheral. \n
			They refer mainly to keys that can be pressed. \n
			Notice that combinations are possible. */
		static const int _KEYFOWARD = 1;
		static const int _KEYREWIND = 2;
		static const int _KEYSTOP   = 4;
		static const int _KEYPLAY   = 8;
		static const int _KEYRECORD = 16; // Save = RECORD + PLAY
		static const int _KEYEJECT	= 32; // To clean up the data loaded, or to simulate like a new casette is inserted...
		static const int _KEYBEGIN	= 64; // To start the tape from the beginning, so it is like a rewind x times...
		static const int _KEYEND	= 128; // To stop the tape at the end, so it is like a forward x times...

		/** The object is owner of the implementation object too. */
		StandardDatasette (int id, Implementation* i, 
			bool mI, const Attributes& attrs = { });

		~StandardDatasette ();

		const Implementation* implementation () const
							{ return (_implementation); }
		Implementation* implementation ()
							{ return (_implementation); }
		// The implementation can only be changed using a protected method, so usually inside the class!

		/** The status cannot be changed but using the commands. 
			At the b eginning the status is _STOPEED. */
		Status status () const
							{ return (_status); }

		virtual bool initialize () override;

		virtual bool connectData (FileData* dt) override;

		virtual bool executeCommand (int id, const Strings& prms) override;
		virtual Strings commandDescriptions () const override
							{ return (Strings (
								{ "1:FORWARD", "2:REWIND", "4:STOP", "8:PLAY", 
								  "24:PLAY + RECORD", "32:EJECT(and clear data)",
								  "64:BEGIN", "128:END (to start to write)" })); }

		virtual bool simulate (CPU* cpu) override;

		/**
		  *	The name of the fields are: \n
		  * The ones in the parent class. \n
		  *	ATTRS	= InfoStructure: Attributes defining the Peripheral. \n
		  */
		virtual InfoStructure getInfoStructure () const override;

		protected:
		// Invoked from the method executeCommand...
		/** When "save" command is about to be executed at the "end" of the current "tape",
			a new DataBlock must be created, but additional info might need to be added to it, so this method 
			can be overloaded. */
		virtual DataMemoryBlock createNewDataBlock ()
							{ return (DataMemoryBlock ()); }
		// Invoked from the method simulate...
		/** To execute additional things. \n
			It can be extended from the user to do additional activities when making io,
			like sound?....By default it does nothing. (it is invoked per cycle). */
		virtual void additionalSimulateActions (CPU* cpu, unsigned int cC) { }
		/** Read the next value from the data file. \n
			The variable ov is set to true when there is no more data to read, and
			false in any other circunstance. */
		inline UByte readFromData (bool& ov);
		/** Store a value in the data file */
		inline void storeInData (const UByte& dt);

		/** To change the implementation, it cannot be nullptr. */
		inline void setImplementation (Implementation* i);

		protected:
		Implementation* _implementation;
		bool _motorControlledInternally;
		/** The status of the datasette. \n
			It can be either stopped, reading or saving. \n
			Notice that the status is not changed when the motor is off!. */
		mutable Status _status;

		// Immplementation
		// Counting which the info to write or read!
		/** value = max size_t means that there is no data pointed. */
		mutable size_t _dataCounter; 
		mutable size_t _elementCounter;
		/** The CPU Cycles last time the simulation was invoked. */
		mutable unsigned int _lastCPUCycles;
		/** Everytime the status is set back to read (PLAY Key), 
			this variable is set to true. */
		OBool _firstTimeReading;
	};

	// ---
	inline UByte StandardDatasette::readFromData (bool& ov)
	{
		ov = false;

		if (_dataCounter < _data._data.size ())
		{
			if (_elementCounter >= _data._data [_dataCounter].bytes ().size ())
			{
				_elementCounter = 0;
				if (ov = (++_dataCounter >= _data._data.size ()))
					_dataCounter = 0; // Overflow...
			}
		}

		return (!ov // Without error (to avoid problems with the counter)
			? _data._data [_dataCounter].bytes ()[_elementCounter++] 
			: _implementation -> valueForNoMoreData ());
	}

	// ---
	inline void StandardDatasette::storeInData (const UByte& dt)
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
