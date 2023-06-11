/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: IOPBuilder.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 12/08/2022 \n
 *	Description: To build the basic peripherals connected to a COMMODORE computer.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_IOPBUILDER__
#define __COMMODORE_IOPBUILDER__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	/** It can be even extende to add new peripherals. */
	class IOPeripheralBuilder : public MCHEmul::IOPeripheralBuilder
	{
		public:
		IOPeripheralBuilder ()
			: MCHEmul::IOPeripheralBuilder ()
				{ }

		protected:
		/** This method must be overloaded to include any new type of peripherals. */
		virtual MCHEmul::IOPeripheral* createPeripheral 
			(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const override;
	};
}

#endif
  
// End of the file
/*@}*/
