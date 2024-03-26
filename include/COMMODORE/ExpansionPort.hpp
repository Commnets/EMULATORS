/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: ExpansionPort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 22/01/2023 \n
 *	Description:	To emulate the behaviour of the Expansion Port.
 *				 	Usually cartridges are connected here.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_EXPANSIONPORT__
#define __COMMODORE_EXPANSIONPORT__

#include <CORE/incs.hpp>
#include <COMMODORE/ExpansionPeripherals.hpp>

namespace COMMODORE
{
	/** This class represents the Expansion Port. \n
		The class is not final because when it is attached to a specific computer,
		the links to chip o memory zones could be different. */
	class ExpansionIOPort : public MCHEmul::IODevice
	{
		public:
		static const int _ID = 102;

		// Different events.
		/** When the data element of the expansion element is loaded, 
			this event will be sent. \n
			This event should imply a reinitializacion of the computer. \n
			At least, this is the situation in the C64. */
		static const unsigned int _EXPANSIONELEMENTIN		= 260;
		/** When the data is out or the element disconnected. \n
			Again this should imply a reinitialization of the computer. */
		static const unsigned int _EXPANSIONELEMENTOUT		= 261;

		ExpansionIOPort ();

		/** To know the element connected. */
		const ExpansionPeripheral* expansionElement () const
							{ return (_expansionElement); }
		ExpansionPeripheral* expansionElement ()
							{ return (_expansionElement); }

		// The expansion port is mape up of 44 pins...
		// 22 in one side (UP) andd 22 in the other side (DOWN).
		// The first 22 are nominated in numbers and the other 22 in letters...
		/** From 1 to 22. */
		bool PIN_UP (unsigned char nP) const
							{ return ((_expansionElement != nullptr) ? _expansionElement -> PIN_UP (nP) : false); }
		/** From A to Z. */
		bool PIN_DOWN (unsigned char nP) const
							{ return ((_expansionElement != nullptr) ? _expansionElement -> PIN_DOWN (nP) : false); }

		/** It verifies before adding it that whether the peripheral is something compatible. */
		virtual bool connectPeripheral (MCHEmul::IOPeripheral* p) override;
		/** It moves to null the _expansionElement data if needed. */
		virtual bool disconnectPeripheral (int id) override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		protected:
		/** The element connected. */
		ExpansionPeripheral* _expansionElement;
		/** The element just extracted. */
		bool _expansionElementOut;
	};
}

#endif
  
// End of the file
/*@}*/
