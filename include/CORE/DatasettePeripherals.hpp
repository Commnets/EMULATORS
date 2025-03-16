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
		if (_valueRead != v) 
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
	  * \n
	  *	In any casette, what is actually constant is usually the speed which the motor turns at!. \n
	  *	Computers' in/out "port" (or sometimes the electronic of the device itself) ,
	  * converts the 1s and 0s signals received from the computer into wave forms (1 or more cycles) 
	  * using different methods (sin, squared,...), and
	  *	either moduling the amplitud or the length of the wave or even both. \n
	  * \n
	  *	At a constant speed of the datasette motor,
	  * the quicker the signals from the computer are sent thorugh out the port, 
	  * the more info is kept per second in the datasette. \n
	  *	So the bauds (bits/second) of the datasette depends on the speed of the computer sending signals!. \n
	  * \n
	  * Additionally every memory info to be saved into the datasette could be translated into
	  * different patterns of 1s and 0s before sending them to the port.. \n
	  * These two behaviours "together" are modelled in class "IOEncoderDecoder". \n
	  * \n
	  * In the other hand, the simulation of the datasette can either work synchonized with the rest of the emulator,
	  * (so based on the cpu cycles) or totally independent (in parallel). \n
	  * This behaviour is modelled in the class "IOSimulation" that is also part of the datasette definition.
	  * \n
	  * So a Datasette standard follows a simulation way (IOSimulation) and uses an encoder (IOEncoder).
	  */
	class StandardDatasette : public DatasettePeripheral
	{
		public:
		/** The way the bits are transalated into info to be kept into the 
			final representation of the datasette. */
		class IOEncoderDecoder
		{
			public:
			virtual unsigned char encode (bool s) const = 0;
			virtual bool decode (unsigned char s) const = 0;
		};

		/** When the encoder is useless, because the simulate method is being rebuilt. */
		class IOEmptyEncoder final : public IOEncoderDecoder
		{
			public:
			virtual unsigned char encode (bool s) const override
							{ return (1); }

			virtual bool decode (unsigned char s) const override
							{ return (true); }
		};

		/** Very very simple,
			It will generate at the end a square wave in the datasette. */
		class IOBasicEncoderDecoder final : public IOEncoderDecoder
		{
			public:
			virtual unsigned char encode (bool s) const override
							{ return (s ? 1 : 0); }

			virtual bool decode (unsigned char s) const override
							{ return ((s == 1) ? true : false); }
		};

		/** The way the datasetee emulation simulates storing or 
			retrieving data to/from file can vary. \n
			It e.g. might be either linked to the clock cpu or not!. \n
			The simulation might accept commands. */
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

			// Invoked from "initialize" method in the main class...
			virtual void initialize () = 0;

			// Invoked from the "simulate" method in main class.
			/** When writting or reading 
				this method must is invoked (@see method simulate from main class). */
			virtual bool io (StandardDatasette& dS, CPU* cpu) = 0;
			/** When datasette is paused, 
				this method must is invoked (@see method simulate from main class). */
			virtual void stop () = 0;

			// Invoked from "executeCommand" metod in main class.
			/** When there is commands dedicated to the simulation! */
			virtual bool executeCommand (int id, const MCHEmul::Strings& prms) = 0;
			/** ...and the method to get their description... */
			virtual Strings commandDescriptions () const = 0;

			// Invoked from the "getInfoestructure" method in the main class.
			virtual InfoStructure getInfoStructure () const override;

			protected:
			Attributes _attributes;
		};

		/** When the simulation itself is not needed,
			because the simulation method is overloaded. \n 
			This might happen (e.g) when the standard datasette is needed (becuase the commands e.g), 
			but the simulation method is fully rebuilt 
			(e.g when datasette is implemented as a trap of a kernel routine). */
		class IONilSimulation final : public IOSimulation
		{
			public:
			IONilSimulation ()
				: IOSimulation (Attributes ())
							{ setClassName ("IOEmpty"); }

			virtual void initialize () override
							{ }

			virtual bool io (StandardDatasette&, CPU*) override
							{ return (true); }
			virtual void stop () override
							{ }

			virtual bool executeCommand (int, const MCHEmul::Strings&) override
							{ return (true); }
			virtual Strings commandDescriptions () const override
							{ return (Strings ({ })); }
		};

		/** The basic simulation happens when the activities to write or to read
			are linked to the speed of the CPU. */
		class IOSynchronous final : public IOSimulation
		{
			public:
			/** The number of cycles to wait 
				before writting or reading is received as parameter. */
			IOSynchronous (unsigned int rS, const Attributes& attrs = { })
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
							{ /** Nothing specific to do here. */ }

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

		/** When the activities to read or to write have nothing to do with the speed of the CPU
			and happens completly in parallel. */
		class IOASynchronous final : public IOSimulation
		{
			public:
			/** What is received is the "motor speed" (the time every which the process is executed in microseconds).
				I am afraid this value has to be adjusted litlle by little in any simulation!. */
			IOASynchronous (unsigned int mS, const Attributes& attrs = { })
				: IOSimulation (attrs),
				  _motorSpeed (mS),
				  _process (nullptr),
				  _thread ()
							{ setClassName ("IOCPUParallelSimulation"); }

			~IOASynchronous ()
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

			/** The process really running in parallel. */
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
		static const int _KEYEJECT	= 32; // To clean up the data loaded, or to simulate a new casette is inserted...
		static const int _KEYIOBASE	= 64; // 10 command to the io simulation... (64...73)
		/** The key EJECT has no value. */

		/** The constructor receives the way to simulate the io activity, 
			that is the way to read or write info into the datasette (cannot be null). \n
			mE defines whether the motor is controlled from internal signals (true) or just pressing the buttons (false). */
		StandardDatasette (int id, IOSimulation* s, IOEncoderDecoder* e, 
			bool mI, const Attributes& attrs = { });

		~StandardDatasette ();

		const IOSimulation* ioSimulation () const
							{ return (_ioSimulation); }
		IOSimulation* ioSimulation ()
							{ return (_ioSimulation); }

		const IOEncoderDecoder* ioEncoderDecoder () const
							{ return (_ioEncoderDecoder); }
		IOEncoderDecoder* ioEncoderDecoder ()
							{ return (_ioEncoderDecoder); }

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
		IOEncoderDecoder* _ioEncoderDecoder;
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
		/** value = max size_t means that there is no data pointed. */
		mutable size_t _dataCounter; 
		mutable unsigned short _elementCounter;
	};

	// ---
	inline void StandardDatasette::IOASynchronous::Process::io 
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
		if (_status == Status::_READING) 
			getNextDataBit ();
		else 
			storeNextDataBit ();
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
			setRead (_ioEncoderDecoder -> decode 
				(_data._data [_dataCounter].bytes ()[_elementCounter++].value ()));
	}

	// ---
	inline void StandardDatasette::storeNextDataBit ()
	{
		_data._data [_dataCounter].addByte (_ioEncoderDecoder -> encode (_valueToWrite));

		_elementCounter++;
	}

	/** The simpliest datasette possible is synchronous and with a very simple encoder. */
	class BasicDatasette : public StandardDatasette
	{
		public:
		BasicDatasette (int id, unsigned int rS, bool mI, const Attributes& attrs)
			: StandardDatasette (id, new StandardDatasette::IOSynchronous (rS), 
				new StandardDatasette::IOBasicEncoderDecoder, mI, attrs)
							{ }

		unsigned int runningSpeed () const
							{ return (static_cast <const StandardDatasette::IOSynchronous*> 
								(ioSimulation ()) -> runningSpeed ()); }
		void setRunningSpeed (unsigned int rS)
							{ return (static_cast <StandardDatasette::IOSynchronous*> 
								(ioSimulation ()) -> setRunningSpeed (rS)); }
	};

	/** The other version is in paralell. */
	class BasicDatasetteP : public StandardDatasette
	{
		public:
		BasicDatasetteP (int id, unsigned int mS, bool mI, const Attributes& attrs)
			: StandardDatasette (id, new StandardDatasette::IOASynchronous (mS), 
				new StandardDatasette::IOBasicEncoderDecoder, mI, attrs)
							{ }

		unsigned int motorSpeed () const
							{ return (static_cast <const StandardDatasette::IOASynchronous*> 
								(ioSimulation ()) -> motorSpeed ()); }
		void setMotorSpeed (unsigned int rS)
							{ return (static_cast <StandardDatasette::IOASynchronous*> 
								(ioSimulation ()) -> setMotorSpeed (rS)); }
	};
}

#endif
  
// End of the file
/*@}*/
