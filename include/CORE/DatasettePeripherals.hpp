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
		/** Data Output. Write TO the casette. */
		void setWrite (bool d)
							{ _valueToWrite = d; _writeChangeValueRequest = true; }
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
		/** The notification that something has changed only happens in the transition from 0 to 1. */
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
	inline void DatasettePeripheral::setRead (bool v)
	{ 
		// It is detected when the signal moves from 0 to 1...
		// Depeding how long the pulse took...
		if (v && !_valueRead) 
			_readChangeValueRequest = true;
		_valueRead = v; 
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
	  *	In any casette what is actually constant is the speed which the motor turns at!. \n
	  *	Computers' in/out ports convert then 1 and 0 into wave forms using one method or another,
	  *	either moduling the amplitud or the length of the wave. \n
	  *	It means that, at a constant speed of the datasette motor, 
	  * the quicker the bits from the CPU are sent to the port, the more info is kept per second in the datasette,
	  *	so the bauds (bits/second) of the datasette depends on the speed of the computer sending bits!.
	  */
	class StandardDatasette : public DatasettePeripheral
	{
		public:
		/** The way the datasetee emulation simulates storing or 
			retrieving data to/from file can vary. \n
			It e.g. might be either linked to the clock cpu or not!. */
		class IOSimulation : public InfoClass
		{
			public:
			IOSimulation (const Attributes& attrs = { })
				: InfoClass ("IOSimulation"),
				  _attributes (attrs)
							{ }

			virtual ~IOSimulation ()
							{ }

			const Attributes& attributes () const
							{ return (_attributes); }

			virtual void initialize () = 0;

			/** When writting/reading this method is invoked (@see method simulate from main class). */
			virtual bool io (StandardDatasette& dS, CPU* cpu) = 0;
			/** When datasette is paused, this method must is invoked (@see method simulate from main class). */
			virtual void stop () = 0;

			// When there is commands dedicated to the simulation!
			virtual bool executeCommand (int id, const MCHEmul::Strings& prms) = 0;
			// ...and the method to get their description...
			virtual Strings commandDescriptions () const = 0;

			virtual InfoStructure getInfoStructure () const override;

			protected:
			Attributes _attributes;
		};

		/** The basic simulation happens when the activities to write or to read
			are linked to the speed of the CPU. */
		class IOCPULinkedSimulation final : public IOSimulation
		{
			public:
			/** The number of cycles to wait before writting or reading is received. */
			IOCPULinkedSimulation (unsigned int rS, const Attributes& attrs = { })
				: IOSimulation (attrs),
				  _runningSpeed (rS),
				  _firstCycleSimulation (false), _lastCPUCycles (0), _clockCycles (0)
							{ setClassName ("IOCPULinkedSimualtion"); }

			/** Get/Change the running speed. */
			unsigned int runningSpeed () const
								{ return (_runningSpeed); }
			void setRunningSpeed (unsigned int rS)
								{ _runningSpeed = rS; initialize (); }

			virtual void initialize () override;

			virtual bool io (StandardDatasette& dS, CPU* cpu) override;
			virtual void stop () override
							{ /** Nothing specific to do. */ }

			virtual bool executeCommand (int id, const MCHEmul::Strings& prms) override;
			virtual Strings commandDescriptions () const override
							{ return (Strings ({ "64 [SPEED]: Change CPU linked speed" })); }

			virtual InfoStructure getInfoStructure () const override;

			private:
			unsigned int _runningSpeed;

			// Implementation
			bool _firstCycleSimulation;
			unsigned int _lastCPUCycles;
			unsigned int _clockCycles;
		};

		/** When the activities to read or to write have nothing to do with the spedd of the CPU
			and happens completly in parallel. */
		class IOParallelSimulation final : public IOSimulation
		{
			public:
			class Process final
			{
				public:
				Process (StandardDatasette& dS, unsigned int mR)
					: _datasette (dS),
					  _motorSpeed (mR),
					  _end (false)
							{ }

				void run ()
							{ io (_datasette, nullptr); }

				void finish ()
							{ _end = true; }

				/** It is an ethernal loop accesing io and waiting 
					what the is defined at comnstruction time. */
				inline void io (StandardDatasette& dS, CPU* /** Not neded. */);

				private:
				bool hasFinished () const
							{ return (_end); }

				private:
				StandardDatasette& _datasette;
				unsigned int _motorSpeed;

				// Implementation
				/** To guaranttee the exclusive access to the next variable. */
				std::atomic <bool> _end;
			};

			/** What is received is the "motor speed" (the time every which the process is executed in microseconds).
				I am afraid this value has to be adjusted litlle by little in any simulation!. */
			IOParallelSimulation (unsigned int mS, const Attributes& attrs = { })
				: IOSimulation (attrs),
				  _motorSpeed (mS),
				  _process (nullptr),
				  _thread ()
							{ setClassName ("IOCPUParallelSimulation"); }

			~IOParallelSimulation ()
							{ stop (); /** Just in case. */ }

			unsigned int motorSpeed () const
							{ return (_motorSpeed); }
			void setMotorSpeed (unsigned int mS)
							{ _motorSpeed = mS; }

			virtual void initialize () override
							{ /** There is nothing to do. */ }

			virtual bool io (StandardDatasette& dS, CPU* cpu) override;
			virtual void stop () override;

			virtual bool executeCommand (int id, const MCHEmul::Strings& prms) override;
			virtual Strings commandDescriptions () const override
							{ return (Strings ({ "64 [SPEED]: Change CPU motor speed" })); }

			virtual InfoStructure getInfoStructure () const override;

			private:
			unsigned int _motorSpeed;

			// Implementation
			Process* _process;
			std::thread _thread;
		};

		/** The commands accepted by this peripheral. \n
			They refer mainly to keys that can be pressed. \n
			Notice that combinations are possible. */
		static const int _KEYFOWARD = 1;
		static const int _KEYREWIND = 2;
		static const int _KEYSTOP   = 4;
		static const int _KEYPLAY   = 8;
		static const int _KEYRECORD = 16; // Save = RECORD + PLAY
		static const int _KEYEJECT	= 32; // To clean up the data loaded, or to simulate a new castte is inserted...
		static const int _KEYIOBASE	= 64; // 10 command to the io simulation... (64...73)
		/** The key EJECT has no value. */

		/** The constructor receives the way to simulate the io activity, 
			that is the way to read or write info into the datasette (cannot be null). \n
			mE defines whether the motor is controlled from internal signals (true) or just pressing the buttons (false). */
		StandardDatasette (int id, IOSimulation* s, bool mI, const Attributes& attrs = { });

		~StandardDatasette ()
							{ delete (_ioSimulation); }

		const IOSimulation* ioSimulation () const
							{ return (_ioSimulation); }
		IOSimulation* ioSimulation ()
							{ return (_ioSimulation); }

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
		/** Invoked from simulate method.
			It can be extended from the user to do additional activities when making io,
			like sound?. */
		virtual void additionalSimulateActions (MCHEmul::CPU* cpu)
							{ }

		/** Invoked from the io simulation. */
		inline void io ();
		/** Invoked from the previous one. \n
			According with the phase the things to store will be one or another. \n
			The can be overloaded to simulate different types of storage, if needed. \n
			But by default they implemented the commodore standard. */
		inline void getNextDataBit ();
		inline void storeNextDataBit ();

		protected:
		IOSimulation* _ioSimulation;
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
		mutable Status _status;
		
		// Counting which the info to write or read!
		mutable size_t _dataCounter; 
		mutable unsigned short _elementCounter;
	};

	// ---
	inline void StandardDatasette::IOParallelSimulation::Process::io 
		(StandardDatasette& dS, CPU*)
	{ 
		while (!hasFinished ()) // Excusive access to _end variable!
		{ 
			dS.io ();
			
			std::this_thread::sleep_for 
				(std::chrono::microseconds (_motorSpeed)); 
		} 
	}

	// ---
	inline void StandardDatasette::io ()
	{
		if (_status == Status::_READING) getNextDataBit ();
		else storeNextDataBit ();
	}

	// ---
	inline void StandardDatasette::getNextDataBit ()
	{
		bool r = false;
		if (_dataCounter < _data._data.size ())
		{
			r = true;
			if (_elementCounter >= _data._data [_dataCounter].bytes ().size ())
			{
				_elementCounter = 0;

				if (++_dataCounter >= _data._data.size ())
					r = false;
			}
		}

		if (r)
			setRead (_data._data [_dataCounter].bytes ()[_elementCounter++] == MCHEmul::UByte::_1);
	}

	// ---
	inline void StandardDatasette::storeNextDataBit ()
	{
		_data._data [_dataCounter].addByte 
			(_valueToWrite ? MCHEmul::UByte::_1 /** cycle 1. */ : MCHEmul::UByte::_0 /** cycle 0. */);

		_elementCounter++;
	}

	/** A very basic datasette. */
	class BasicDatasette : public StandardDatasette
	{
		public:
		BasicDatasette (int id, unsigned int rS, bool mI, const Attributes& attrs)
			: StandardDatasette (id, new StandardDatasette::IOCPULinkedSimulation (rS), mI, attrs)
							{ }

		unsigned int runningSpeed () const
							{ return (static_cast <const StandardDatasette::IOCPULinkedSimulation*> 
								(ioSimulation ()) -> runningSpeed ()); }
		void setRunningSpeed (unsigned int rS)
							{ return (static_cast <StandardDatasette::IOCPULinkedSimulation*> 
								(ioSimulation ()) -> setRunningSpeed (rS)); }
	};

	/** And the same verion but in parallel. */
	class BasicDatasetteP : public StandardDatasette
	{
		public:
		BasicDatasetteP (int id, unsigned int mS, bool mI, const Attributes& attrs)
			: StandardDatasette (id, new StandardDatasette::IOParallelSimulation (mS), mI, attrs)
							{ }

		unsigned int motorSpeed () const
							{ return (static_cast <const StandardDatasette::IOParallelSimulation*> 
								(ioSimulation ()) -> motorSpeed ()); }
		void setMotorSpeed (unsigned int rS)
							{ return (static_cast <StandardDatasette::IOParallelSimulation*> 
								(ioSimulation ()) -> setMotorSpeed (rS)); }
	};
}

#endif
  
// End of the file
/*@}*/
