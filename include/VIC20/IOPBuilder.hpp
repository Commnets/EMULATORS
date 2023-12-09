/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: IOPBuilder.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 09/12/2023 \n
 *	Description: To build the specific peripherals connected to a VIC20 computer.
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_IOPBUILDER__
#define __VIC20_IOPBUILDER__

#include <COMMODORE/incs.hpp>

namespace VIC20
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
