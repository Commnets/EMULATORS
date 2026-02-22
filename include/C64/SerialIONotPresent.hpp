/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: SerialIONotPresent.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 08/02/2026 \n
 *	Description: To emulate the behaviour of the system when a serial peripheral is invoked and it is not present.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_SERIALIONOTPRESENT__
#define __C64_SERIALIONOTPRESENT__

#include <COMMODORE/incs.hpp>

namespace C64
{
	/** To simulate that a device is not present. */
	class SerialNotPresentIOPeripheralSimulation final : 
		public COMMODORE::SerialNotPresentIOPeripheralSimulation
	{
		public:
		SerialNotPresentIOPeripheralSimulation ()
			: COMMODORE::SerialNotPresentIOPeripheralSimulation (_DEFINITION)
							{ }

		private:
		/** The definition of the traps for this device. */
		static const Definition _DEFINITION;
	};
}

#endif
  
// End of the file
/*@}*/
