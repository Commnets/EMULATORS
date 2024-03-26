/** \ingroup SINCLAIR */
/*@{*/

/**	
 *	@file	
 *	File: IOPBuilder.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 24/03/2024 \n
 *	Description: To build the basic peripherals connected to a SINCLAIR computer.
 *	Versions: 1.0 Initial
 */

#ifndef __SINCLAIR_IOPBUILDER__
#define __SINCLAIR_IOPBUILDER__

#include <CORE/incs.hpp>

namespace SINCLAIR
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
