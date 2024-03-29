/** \ingroup ZX81 */
/*@{*/

/**	
 *	@file	
 *	File: EdgeConnector.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 25/03/2024 \n
 *	Description:	To emulate the behaviour of the Edge Connector.
 *				 	Usually cartridges are connected here.
 *	Versions: 1.0 Initial
 */

#ifndef __ZX81_EGDECONNECTOR__
#define __ZX81_EGDECONNECTOR__

#include <CORE/incs.hpp>
#include <ZX81/EdgeConnectorPeripherals.hpp>

namespace ZX81
{
	class EdgeConnector : public MCHEmul::IODevice
	{
		public:
		static const int _ID = 203;

		// Different events.
		/** When the data element of the expansion element is loaded, 
			this event will be sent. \n
			This event should imply a reinitializacion of the computer. \n
			At least, this is the situation in the C64. */
		static const unsigned int _EXPANSIONELEMENTIN		= 260;
		/** When the data is out or the element disconnected. \n
			Again this should imply a reinitialization of the computer. */
		static const unsigned int _EXPANSIONELEMENTOUT		= 261;

		EdgeConnector ();

		/** To know the element connected. */
		const EdgeConnectorPeripheral* expansionElement () const
							{ return (_expansionElement); }
		EdgeConnectorPeripheral* expansionElement ()
							{ return (_expansionElement); }

		// The expansion port is mape up of 46 pins...
		// 23 in one side (UP) andd 23 in the other side (DOWN).
		/** From 1 to 23A. */
		bool PIN_UP (unsigned char nP) const
							{ return ((_expansionElement != nullptr) ? _expansionElement -> PIN_UP (nP) : false); }
		/** From 1 to 23B. */
		bool PIN_DOWN (unsigned char nP) const
							{ return ((_expansionElement != nullptr) ? _expansionElement -> PIN_DOWN (nP) : false); }

		/** It verifies before adding it that whether the peripheral is something compatible. */
		virtual bool connectPeripheral (MCHEmul::IOPeripheral* p) override;
		/** It moves to null the _expansionElement data if needed. */
		virtual bool disconnectPeripheral (int id) override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		protected:
		/** The element connected. */
		EdgeConnectorPeripheral* _expansionElement;
		/** The element just extracted. */
		bool _expansionElementOut;
	};
}

#endif
  
// End of the file
/*@}*/
