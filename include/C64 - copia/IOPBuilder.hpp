/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: IOPBuilder.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 04/03/2023 \n
 *	Description: To build the specific peripherals connected to a C64 computer.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_IOPBUILDER__
#define __C64_IOPBUILDER__

#include <COMMODORE/incs.hpp>

namespace C64
{
	class IOPeripheralBuilder final : public COMMODORE::IOPeripheralBuilder
	{
		public:
		IOPeripheralBuilder ()
			: COMMODORE::IOPeripheralBuilder ()
				{ }

		protected:
		virtual MCHEmul::IOPeripheral* createPeripheral 
			(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const override;
	};
}

#endif
  
// End of the file
/*@}*/
