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
		SerialIOPeripheral (int id, unsigned char dN, const MCHEmul::Attributes& attrs)
			: MCHEmul::IOPeripheral (id, attrs),
			  _deviceNumber (dN)
							{ setClassName ("SerialIOPeripheral"); }

		/** To get the device number. */
		unsigned char deviceNumber () const
							{ return (_deviceNumber); }

		/** 
		  * The info included is the one of the parent plus: \n
		  * DEVICENUMBER	= Attribute: The nummber of device assigned. \n
		  * ACTIVE			= Atribute: YES when active and NO when isn't. 
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		/** The number of the device. 
			There are specific elements with specific numbers. 
			0 is keyboard, 3 is screen, 4-5 printers, 8-30 IEC like disks. */
		unsigned char _deviceNumber;
	};

	/** Represents nothing connected. */
	class NoSerialIOPeripheral : public SerialIOPeripheral
	{
		public:
		static const int _ID = -103;

		NoSerialIOPeripheral ()
			: SerialIOPeripheral (_ID, (unsigned char) 0, { }) // A null (negative) identificator...
							{ }

		virtual bool initialize () override
							{ return (true); } // Nothing...
		virtual bool finalize () override
							{ return (true); } // Nothing...

		virtual bool simulate (MCHEmul::CPU* cpu) override
							{ return (true); } // Does nothing...
	};

	/** All serial io peripherals managed with traps behave in the same way. 
		This class represents all off them. */
	class SerialIOPeripheralSimulation : public SerialIOPeripheral
	{
		public:
		struct Definition final
		{
			public:
			MCHEmul::Address _LATTABLE;		// Memory address of the table with the status of every logical channel...
			MCHEmul::Address _FATTABLE;		// Memory address of the table with the device assigned per logical channel...
			MCHEmul::Address _SATTABLE;		// Memory address of the table with the secondary device number per logical channel...
			MCHEmul::Address _DFLTN;		// Memory address where the Logical Active Channel active for Input is stored
			MCHEmul::Address _DFLTO;		// Memory address where the Logical Active Channel active for Output is stored.

			MCHEmul::Address _sDataAddress;	// The address where the information to be sent to the serial port is kept...
			MCHEmul::Address _stAddress;	// Where to store the result of the operation, before returning from the any trap...
			unsigned char _okResult;		// Code when everything is ok...
			unsigned char _notPresent;		// Code when the device is not present...

			const MCHEmul::Traps _traps;	// The traps...

			/**
			  * The info included in the infraestructure is: \n
			  * LATTABLE	= Attribute: Address of the first element in the table 
			  *				  containing the status of the logical channels. \n
			  * FATTABLE	= Attribute: Idem, but with the device number associated to each logical channel. \n
			  *				  The element is 0, when there is no device assigned to a specific logical channel. \n
			  * STATABLE	= Attribute: Idem than the previous, but regarding the secondary device number, if any. \n
			  * TRAPS		= Attribute: List of the traps defined in the device. String.
			  */
			MCHEmul::InfoStructure getInfoStructure () const;
		};

		/** The name of the traps. */
		static const int _LISTEN1TRAP = 0;
		static const int _LISTEN2TRAP = 1;
		static const int _SENDBYTETRAP = 2;
		static const int _RECEIVEBYTETRAP = 3;
		static const int _READYTRAP = 4;
		static const int _READYTRAP2 = 5; // In commodore 16/iPlus4

		SerialIOPeripheralSimulation (int id, unsigned char dN, const Definition& dt, 
			const MCHEmul::Attributes& attrs);

		/** To get the definition even in a way that might be changed,
			althought it is not advised. */
		const Definition& definition () const
							{ return (_definition); }
		Definition& definition ()
							{ return (_definition); }

		virtual bool simulate (MCHEmul::CPU* cpu) override;

			/**
			  * The info included in the infraestructure is the one of the parent class plus:
			  * SerialPrinterInjectionDefinition	= InfoStructure: The definition of the traps and the way to get them.
			  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		/** This method is invoked from simulate method to determine whether the 
			CPU is just on a point where a trap maust be executed. ºn
			Returns true when the trap is really executed. */
		virtual bool executeTrap (const MCHEmul::Trap& t, MCHEmul::CPU* cpu);

		/** All methods to execute the different traps... \n
			They can be overloaded. \n
			When the trap is really executed, the traps returns true, otherwise false. \n
			The st variable is st also when executed. \n
			Remember that this is rund robin of serial devices, and the trap can be invoked for a specific device that is not on. \n
			These methods can be be overloaded, maybe per type of the KERNEL, because it what they really emulate!. */
		virtual bool executeListenTrap (MCHEmul::CPU* cpu, unsigned char& st);
		virtual bool executeSendByteTrap (MCHEmul::CPU* cpu, unsigned char& st);
		virtual bool executeReceiveByteTrap (MCHEmul::CPU* cpu, unsigned char& st);
		virtual bool executeReadyTrap (MCHEmul::CPU* cpu, unsigned char& st);

		/** What every device does, depends on the type of the device and their specific KERNEL. \n
			So, these methods must be overloaded per tyep of device. \n
			All they return the status code of the execution. */
		virtual unsigned char listen (MCHEmul::CPU* cpu, const MCHEmul::UByte& b)
							{ return (_definition._okResult); }
		virtual unsigned char unlisten (MCHEmul::CPU* cpu, const MCHEmul::UByte& b)
							{ return (_definition._okResult); }
		virtual unsigned char talk (MCHEmul::CPU* cpu, const MCHEmul::UByte& b)
							{ return (_definition._okResult); }
		virtual unsigned char untalk (MCHEmul::CPU* cpu, const MCHEmul::UByte& b)
							{ return (_definition._okResult); }
		virtual unsigned char openChannel (MCHEmul::CPU* cpu, const MCHEmul::UByte& chn)
							{ return (_definition._okResult); }
		virtual unsigned char closeChannel (MCHEmul::CPU* cpu, const MCHEmul::UByte& chn)
							{ return (_definition._okResult); }
		virtual unsigned char sendByte (MCHEmul::CPU* cpu, const MCHEmul::UByte& b)
							{ return (_definition._okResult); }
		/** The byte to be received is in this case a parameter. */
		virtual unsigned char receiveByte (MCHEmul::CPU* cpu, MCHEmul::UByte& b)
							{ b = 0; return (_definition._okResult); }
		virtual unsigned char getReady (MCHEmul::CPU* cpu)
							{ return (_definition._okResult); }

		/** To get information about the device considering the logical channel. \n
			This information must be accessed from the execution of the traps. \n
			When a device o secondary device returns 0 means that there is nothing connected under that logical channel. */
		bool logicalChannelConnected (MCHEmul::CPU* cpu, unsigned char lC) const
							{ return (cpu -> memoryRef () -> value 
								(_definition._LATTABLE + (size_t) lC) != MCHEmul::UByte::_0); }
		unsigned char deviceAssignedToLogicalChannel (MCHEmul::CPU* cpu, unsigned char lC) const
							{ return (cpu -> memoryRef () -> value 
								(_definition._FATTABLE + (size_t) lC).value ()); }
		unsigned char secondaryDeviceAssignedToLogicalChannel (MCHEmul::CPU* cpu, unsigned char lC) const
							{ return (cpu -> memoryRef () -> value 
								(_definition._SATTABLE + (size_t) lC).value ()); }

		// -----
		// Different debug methods to simplify the internal code
		// and to make simplier the modification in case it is needed...
		/** Debug special situations...
			Take care using this instructions _deepDebugFile could be == nullptr... */
		void debugStatus (const std::string& where, MCHEmul::CPU* cpu);
		// -----

		protected:
		Definition _definition;

		// Implementation
		/** The different status this serial port can be in. */
		enum class Status
		{
			_NOTLISTENING = 0,
			_LISTENING = 1,
			_TALKING = 2,
		};

		/** The different possibilities when the port is open. */
		enum class StatusOpen
		{
			_CLOSE = 0,
			_WAITINGFORNAME = 1,
			_OPEN = 2
		};

		/** The internal status of the device. */
		Status _status;
		StatusOpen _statusOpen;

		/** The CPU Cycles last time the simulation was invoked. */
		mutable unsigned int _lastCPUCycles;
	};

	/** To simulate that a device is not present. 
		It is always*/
	class SerialNotPresentIOPeripheralSimulation : public SerialIOPeripheralSimulation
	{
		public:
		/** The max possible. \n
			It must be processed the last one in the simulation. */
		static const int _ID = 1000;

		SerialNotPresentIOPeripheralSimulation (const Definition& dt);

		virtual bool initialize () final override // No more extensions...
							{ return (true); }
		virtual bool finalize () final override // No more extension...
							{ return (true); }

		protected:
		virtual bool executeTrap (const MCHEmul::Trap& t, MCHEmul::CPU* cpu) final override; // No more extensions...
	};
}

#endif
  
// End of the file
/*@}*/
