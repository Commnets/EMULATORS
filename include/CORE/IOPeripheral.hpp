/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: IOPeripheral.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/08/2022 \n
 *	Description: Emulating the peripherals (maybe) connected to a io device.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_IOPERIPHERAL__
#define __MCHEMUL_IOPERIPHERAL__

#include <CORE/global.hpp>
#include <CORE/InfoClass.hpp>
#include <CORE/FileReader.hpp>

namespace MCHEmul
{
	class IODevice;

	/** The IOPeripheral represents an element connected to a IODevice. \n
		For some IODevices the number and type (and manufacturers) of elements that can be connected might be wide and different. \n
		And the simulation of each can even be very different. \n
		This is the reason to represent them in a separate class. */
	class IOPeripheral : public InfoClass
	{
		public:
		friend IODevice;

		IOPeripheral () = delete;

		IOPeripheral (int id, const Attributes& attrs = { })
			: InfoClass ("IOPeripheral"),
			  _id (id), _attributes (attrs), _device (nullptr) /** Set when attached. */
							{ }

		IOPeripheral (const IOPeripheral&) = delete;

		IOPeripheral& operator = (const IOPeripheral&) = delete;

		virtual ~IOPeripheral () 
							{ }

		IOPeripheral (IOPeripheral&&) = delete;

		IOPeripheral& operator = (IOPeripheral&&) = delete;

		int id () const
							{ return (_id); }

		const Attributes& attributes () const
							{ return (_attributes); }
		const std::string& attribute (const std::string& aN) const
							{ Attributes::const_iterator i = _attributes.find (aN); 
							  return ((i == _attributes.end ()) ? AttributedNotDefined : (*i).second); }

		const IODevice* device () const
							{ return (_device); }
		IODevice* device ()
							{ return (_device); }

		/** To initialize the peripheral. 
			The initialization should return true when everything is ok, and false in other case. */
		virtual bool initialize () = 0;

		/** To link, when it is needed, the peripheral with data comming from a file. \n
			Makes sense specially when the peripheral is an input one (like a casette or a unit disk or a cartridge)
			there might be a content in those types of periperals and this instruction is for that. \n
			By default the data can't be connected. */
		virtual bool connectData (FileData* dt)
							{ return (false); }

		/** To emulate the way it works. \n
			Should return true if everything was ok. */
		virtual bool simulate () = 0;

		/**
		  *	The name of the fields are: \n
		  *	ID		= Attribute: Id of the Peripheral. \n
		  *	ATTRS	= InfoStructure: Attributes defining the Peripheral. \n
		  */
		virtual InfoStructure getInfoStructure () const override;

		friend std::ostream& operator << (std::ostream& o, const IOPeripheral& d)
							{ return (o << (*(dynamic_cast <const InfoClass*> (&d)))); }

		protected:
		const int _id; // Adjusted at construction level
		const Attributes _attributes = { }; // Maybe modified at construction level
		/** Accesed from IODevice when a peripherial is added! */
		IODevice* _device;
	};

	/** To simplify the management of a set of peripherals. */
	using IOPeripherals = std::map <int, IOPeripheral*>;
}

#endif
  
// End of the file
/*@}*/
