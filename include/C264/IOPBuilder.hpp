/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: IOPBuilder.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 04/02/2024 \n
 *	Description: To build the specific peripherals connected to a C264 series computers.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_IOPBUILDER__
#define __C264_IOPBUILDER__

#include <COMMODORE/incs.hpp>

namespace C264
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
