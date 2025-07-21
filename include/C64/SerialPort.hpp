/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: SerialPort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 20/07/2025 \n
 *	Description: To emulate the behaviour of the Serial Port in a C64.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_SERIALPORT__
#define __C64_SERIALPORT__

#include <CORE/incs.hpp>
#include <COMMODORE/incs.hpp>

namespace C64
{
	class CIA2;

	class SerialIOPort final : public COMMODORE::SerialIOPort
	{
		public:
		SerialIOPort ()
			: COMMODORE::SerialIOPort (),
			  _cia2 (nullptr)
							{ }
	
		private:
		/** It is affected when something happens in the ports. */
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* n) override;

		private:
		CIA2* _cia2;
	};
}

#endif
 
// End of the file
/*@}*/
