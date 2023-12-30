/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: IO.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/06/2021 \n
 *	Description: Emulating the input / output interfaces
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_IO__
#define __MCHEMUL_IO__

#include <CORE/Chip.hpp>
#include <CORE/MBElement.hpp>
#include <CORE/IOPeripheral.hpp>

namespace MCHEmul
{
	/**
	  * The class IODevice represents anything connected to any Chip of the motherboard. \n
	  * The ports (RS232, Serial, IEEE, etc...) are typically elements comnnected to communication chips
	  * but also the screen or the keyboard. \n
	  * Connected to a device there might be different peripherials. \n
	  * For some specific devices (like screen or keyboard) the peripheral connected is "implicit" and
	  * there is no other way to add other different, but it has to be opened to other possibilities! \n
	  * Any device is able to notify events (usually received by chips subscribed) 
	  * or also to receive events from them. \n
	  * The right way to create the relations among them is the method "linkChips" that is invoked in
	  * the constructor of the computer (@see Computer) when all elements involved are received as parameter.
	  */
	class IODevice : public MotherboardElement, public Notifier
	{
		public:
		enum class Type 
		{ 
			_INPUT = 0, 
			_OUTPUT, 
			_INPUTOUTPUT 
		};

		IODevice (Type t, int id, const Attributes& attrs = { });

		IODevice (const IODevice&) = delete;

		IODevice& operator = (const IODevice&) = delete;

		/** The device doesn't own the chips, only work with them. 
			The device doesn't own the peripherals either. */
		virtual ~IODevice () override;

		IODevice (IODevice&&) = delete;

		IODevice& operator = (IODevice&&) = delete;

		Type type () const
							{ return (_type); }

		/** To link to the right chips. \n
			The IO device never owns the chips. \n
			By default all chips are "known". \n
			To really link them use the patter notify-observer (method "observe"). */
		virtual void linkToChips (const Chips& c)
							{ _chips = c; }
		const Chips& chips () const
							{ return (_chips); }

		// To manage the peripherals connected...
		const IOPeripherals& peripherals () const
							{ return (_peripherals); }
		bool existsPeripheral (int id) const
							{ return (_peripherals.find (id) != _peripherals.end ()); }
		/** To connect a peripheral. 
			If either there had been a peripheral with the same id already connected 
			or the peripheral were not valid, nothing would happen. \n
			The method can be overloaded to check (e.g) compability before adding it. \n
			The method initialize form the peripheral is invoked. \n
			Returns true if everything was ok, and false in other case. */
		virtual bool connectPeripheral (IOPeripheral* p);
		/** To disconnect a Peripheral. if the peripheral doesn't exist nothing happens (returns true). \n
			Returns false when an error in the desconnection has happened. \n
			The method finalize from the peropheral is invokes.
			Anyway if the peripheral exist the elements will be disconnected. */
		virtual bool disconnectPeripheral (int id);
		/** To remove all peripherals. \n
			Returns false when at least one peripheral has an error in the deconnection. */
		bool disconnectAllPeripherals ();

		/** To initialize the device. */
		virtual bool initialize () override;

		/** To emulate the way it works. \n
			Return true, if everything was ok. */
		virtual bool simulate (CPU* cpu);

		/** To get the last error happend (after initialize or simulate methods). */
		unsigned int error () const
							{ return (_error); }

		/**
		  *	The name of the fields are: \n
		  *	ID			= Attribute with the Id of the Device. \n
		  *	ATTRS		= InfoStructure with the attributes defining the Device. \n
		  *	PERIPHERALS	= InfoStructure with the info of the peripherals connected.
		  */
		virtual InfoStructure getInfoStructure () const override;

		friend std::ostream& operator << (std::ostream& o, const IODevice& d)
							{ return (o << (*(dynamic_cast <const InfoClass*> (&d)))); }

		/** Manages the deep debug file. \n
			Take care it can be set back to a nullptr. */
		bool deepDebugActive () const
							{ return (_deepDebugFile != nullptr && _deepDebugFile -> active ()); }
		void setDeepDebugFile (DebugFile* dF)
							{ _deepDebugFile = dF; }
		const DebugFile* deepDebugFile () const
							{ return (_deepDebugFile); }
		DebugFile* deepDebugFile ()
							{ return (_deepDebugFile); }

		protected:
		const Type _type; // Modified at constrution level
		Chips _chips; // linked when computer instance is built!
		IOPeripherals _peripherals;

		// To manage the debug info...
		DebugFile* _deepDebugFile;

		// Implementation
		mutable unsigned int _error;
	};

	/** To simplify the management of a list of devices. */
	using IODevices = std::map <int, IODevice*>;

	/** All devices must be managed under a common system. \n
		That system has to be initialized. \n
		The way to initialize the device system is using the singleton structure. 
		Most of the devices are managed using SDL, so this is the framework initialized here. */
	class IODeviceSystem final
	{
		public:
		IODeviceSystem ();

		IODeviceSystem (const IODeviceSystem&) = delete;

		IODeviceSystem& operator = (const IODeviceSystem&) = delete;

		~IODeviceSystem ();

		IODeviceSystem (IODeviceSystem&&) = delete;

		IODeviceSystem& operator = (IODeviceSystem&&) = delete;

		/** To get the pointer to the singleton instance of the class. */
		static std::shared_ptr <IODeviceSystem> system ()
							{ return (_theSystem); }

		// To manage the list of devices...
		const IODevices& devices () const
							{ return (_devices); }
		void add (IODevice* d)
							{ _devices.insert (IODevices::value_type (d -> id (), d)); }
		void remove (int id)
							{ IODevices::const_iterator i = _devices.find (id); 
							  if (i != _devices.end ()) _devices.erase (i); }

		private:
		IODevices _devices;

		/** The static reference to the unique objcet of this instance. */
		static std::shared_ptr <IODeviceSystem> _theSystem;
	};
}

#endif
  
// End of the file
/*@}*/
