/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: ExpansionPeripherals.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 21/01/2023 \n
 *	Description: To emulate the behaviour of the the elements connected to the ExpansionIOPort.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_EXPANSIONPERIPHERALS__
#define __COMMODORE_EXPANSIONPERIPHERALS__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	/** All expansion peripheral should inherit from this class. \n
		Just to guarantte that they can be linked to the Expansion IO Port. */
	class ExpansionPeripheral : public MCHEmul::IOPeripheral
	{
		public:
		ExpansionPeripheral (int id, const MCHEmul::Attributes& attrs)
			: MCHEmul::IOPeripheral (id, attrs),
			  _data (),
			  _dataJustLoaded (false),
			  _pinStatusChanged (false) // At the benginning they might be informed...
							{ }

		/** From 1 to 22. */
		virtual bool PIN_UP (unsigned char nP) const
							{ return (true); } // By default when they are not connected...
		/** From A to Z. */
		virtual bool PIN_DOWN (unsigned char nP) const
							{ return (true); } // By default when they are not connected...

		/** To know whether the data has just loaded. 
			Notice that the only way to update the value of data it to load an "empty" data value. */
		bool dataJustLoaded () const
							{ bool r = _dataJustLoaded; _dataJustLoaded = false; return (r); }
		const MCHEmul::ExtendedDataMemoryBlocks& data () const
							{ return (_data); }

		/** To know whether the status of a PIN has changed. */
		bool pinStatusChanged () const
							{ bool r = _pinStatusChanged; _pinStatusChanged = false; return (r); }

		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		void setData (const MCHEmul::ExtendedDataMemoryBlocks& dt)
							{ _data = dt; }
		void clearData ()
							{ _data = { }; }

		void setDataJustLoaded ()
							{ _dataJustLoaded = true; }

		void setPinStatusChanged ()
							{ _pinStatusChanged = true; }

		protected:
		MCHEmul::ExtendedDataMemoryBlocks _data;

		// Implementation
		/** Becomes true when the data is just loaded. 
			This data becomes back to false when the method dataJustLoaded is read. */
		mutable bool _dataJustLoaded;
		/** It must be true when the status of any pin has changed. 
			In some cases it has to be informed. */
		mutable bool _pinStatusChanged;

	};

	/** Represents nothing connected. */
	class NoExpansionPeripheral : public ExpansionPeripheral
	{
		public:
		static const int _ID = -102;

		NoExpansionPeripheral ()
			: ExpansionPeripheral (_ID, { }) // A nothing identificator...
							{ }

		virtual bool initialize () override
							{ return (true); }
		virtual bool finalize () override
							{ return (true); }

		virtual bool simulate (MCHEmul::CPU* cpu) override
							{ return (true); }
	};
}

#endif
  
// End of the file
/*@}*/
