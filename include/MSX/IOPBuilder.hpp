/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: IOPBuilder.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 29/03/2025 \n
 *	Description: To build the specific peripherals connected to a MSX computer.
 *	Versions: 1.0 Initial
 */

#ifndef __MSX_IOPBUILDER__
#define __MSX_IOPBUILDER__

#include <SINCLAIR/incs.hpp>

namespace MSX
{
	class IOPeripheralBuilder final : public MCHEmul::IOPeripheralBuilder
	{
		public:
		IOPeripheralBuilder ()
			: MCHEmul::IOPeripheralBuilder ()
				{ }

		protected:
		virtual MCHEmul::IOPeripheral* createPeripheral 
			(int id, MCHEmul::Computer* c, const MCHEmul::Attributes& prms) const override;
	};
}

#endif
  
// End of the file
/*@}*/
