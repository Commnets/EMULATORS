/** \ingroup ZXSPECTRUM */
/*@{*/

/**	
 *	@file	
 *	File: IOPBuilder.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/08/2024 \n
 *	Description: To build the specific peripherals connected to a ZXSpectrum computer.
 *	Versions: 1.0 Initial
 */

#ifndef __ZXSPECTRUM_IOPBUILDER__
#define __ZXSPECTRUM_IOPBUILDER__

#include <SINCLAIR/incs.hpp>

namespace ZXSPECTRUM
{
	class IOPeripheralBuilder final : public SINCLAIR::IOPeripheralBuilder
	{
		public:
		IOPeripheralBuilder ()
			: SINCLAIR::IOPeripheralBuilder ()
				{ }

		protected:
		virtual MCHEmul::IOPeripheral* createPeripheral 
			(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const override;
	};
}

#endif
  
// End of the file
/*@}*/
