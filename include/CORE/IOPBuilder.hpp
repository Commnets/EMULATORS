/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: IOPBuilder.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 12/08/2022 \n
 *	Description: To build any peripherial connected to a computer.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_IOPBUILDER__
#define __MCHEMUL_IOPBUILDER__

#include <CORE/global.hpp>
#include <CORE/IOPeripheral.hpp>

namespace MCHEmul
{
	/** This builder is to create any type of peripherical that might be connected to the computer simulated. \n
		The builder is the owner of the periphericals created,
		and they will be deleted as this object is also deleted. */
	class IOPeripheralBuilder
	{
		public:
		IOPeripheralBuilder () = default;

		IOPeripheralBuilder (const IOPeripheralBuilder&) = delete;

		IOPeripheralBuilder& operator = (const IOPeripheralBuilder&) = delete;

		virtual ~IOPeripheralBuilder () 
							{ for (auto i : _peripherals) delete (i.second); }

		/** If the peripherical requeted already existed (one with the same id) a reference to it would be returned,
			otherwise it sill created...if any! */
		IOPeripheral* peripheral (int id, const Attributes& attrs) const;

		protected:
		/** This method must be overloaded to include any new type of peripherals. */
		virtual IOPeripheral* createPeripheral (int id, const Attributes& prms) const = 0;

		protected:
		mutable IOPeripherals _peripherals;
	};
}

#endif
  
// End of the file
/*@}*/
