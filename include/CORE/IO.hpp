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
#include <CORE/IOPeripheral.hpp>

namespace MCHEmul
{
	/**
	  * The class IODevice represents anything connected to any Chip of the motherboard. \n
	  * The ports (RS232, Serial, IEEE, etc...) are typically elements comnnected to communication chips
	  * but also the screen or the keyboard. \n
	  * Connected to a device there might be different peripherials. \n
	  * For some specific devices (like screen or keyboard) the peripheral connected is "implicit" and
	  * there is no other way to add other different, but it has to be opened to other possibilities!
	  */
	class IODevice : public InfoClass
	{
		public:
		enum class Type { _INPUT = 0, _OUTPUT, _INPUTOUTPUT };

		IODevice () = delete;

		IODevice (Type t, int id, const Attributes& attrs = { });

		IODevice (const IODevice&) = delete;

		IODevice& operator = (const IODevice&) = delete;

		/** The device doesn't own the chips, only work with them. 
			The device doesn't own the peripherals either. */
		virtual ~IODevice () 
							{ }

		Type type () const
							{ return (_type); }

		int id () const
							{ return (_id); }

		const Attributes& attributes () const
							{ return (_attributes); }
		const std::string& attribute (const std::string& aN) const
							{ Attributes::const_iterator i = _attributes.find (aN); 
							  return ((i == _attributes.end ()) ? AttributedNotDefined : (*i).second); }

		/** To link to the right chips. 
			The IO device never owns the chips.
			By default all chips are linked. */
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
			The method can be overloaded to check (e.g) compability before adding it. */
		virtual void addPeripheral (IOPeripheral* p);
		/** To remove a Peripheral. if the peripheral doesn't exist nothing happens. */
		void removePeripheral (int id);

		/** To initialize the device. */
		virtual bool initialize ();

		/** To emulate the way it works. \n
			Return true, if everything was ok. */
		virtual bool simulate ();

		/** To get the last error happend (after initialize or simulate methods). */
		unsigned int lastError () const
							{ return (_lastError); }

		/**
		  *	The name of the fields are: \n
		  *	ID			= Attribute with the Id of the Device. \n
		  *	ATTRS		= InfoStructure with the attributes defining the Device. \n
		  *	PERIPHERALS	= InfoStructure with the info of the peripherals connected.
		  */
		virtual InfoStructure getInfoStructure () const override;

		friend std::ostream& operator << (std::ostream& o, const IODevice& d)
							{ return (o << (*(dynamic_cast <const InfoClass*> (&d)))); }

		protected:
		const Type _type; // Modified at constrution level
		const int _id; // Idem
		Chips _chips; // linked when computer instance is built!
		const Attributes _attributes = { }; // Maybe modified at construction level
		IOPeripherals _peripherals;

		// Implementation
		mutable unsigned int _lastError;
	};

	/** To simplify the management of a map of devices. */
	using IODevices = std::map <int, IODevice*>;
}

#endif
  
// End of the file
/*@}*/
