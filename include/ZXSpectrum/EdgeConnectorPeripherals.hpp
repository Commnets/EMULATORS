/** \ingroup ZXSPECTRUM */
/*@{*/

/**	
 *	@file	
 *	File: EdgeConnectorPeripherals.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/08/2024 \n
 *	Description: To emulate the behaviour of the the elements connected to the Edge Connector.
 *	Versions: 1.0 Initial
 */

#ifndef __ZXSPECTRUM_EDGECONNECTORPERIPHERALS__
#define __ZXSPECTRUM_EDGECONNECTORPERIPHERALS__

#include <CORE/incs.hpp>

namespace ZXSPECTRUM
{
	/** All edge connectort peripherals should inherit from this class. \n
		Just to guarantte that they can be linked to the Edge Connector Port. */
	class EdgeConnectorPeripheral : public MCHEmul::IOPeripheral
	{
		public:
		EdgeConnectorPeripheral (int id, const MCHEmul::Attributes& attrs)
			: MCHEmul::IOPeripheral (id, attrs),
			  _data (),
			  _dataJustLoaded (false),
			  _pinStatusChanged (false) // At the benginning they might be informed...
							{ }

		/** From 1 to 23A. */
		virtual bool PIN_UP (unsigned char nP) const
							{ return (true); } // By default when they are not connected...
		/** From 1 to 23B. */
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

		/**
		  *	The name of the fields are: \n
		  *	DATA			= Attribute with the name of the data connected to the peripheral.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		void setData (const MCHEmul::ExtendedDataMemoryBlocks& dt)
							{ _data = dt; }
		void setData (MCHEmul::ExtendedDataMemoryBlocks&& dt)
							{ _data = std::move (dt); }
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
	class NoEdgeConnectorPeripheral : public EdgeConnectorPeripheral
	{
		public:
		static const int _ID = -102;

		NoEdgeConnectorPeripheral ()
			: EdgeConnectorPeripheral (_ID, { }) // A nothing identificator...
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
