/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: NMIInterrupt.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 11/11/2023 \n
 *	Description: The NMI type of interrupt in FZ80.
 *	Versions: 1.0 Initial
 */

#ifndef __FZ80_NMIINTERRUPT__
#define __FZ80_NMIINTERRUPT__

#include <CORE/incs.hpp>
#include <FZ80/Interrupt.hpp>

namespace FZ80
{
	/** A non Maskarable Interrupt. Any time it is invoked it has to be executed. */
	class NMIInterrupt final : public Interrupt
	{
		public:
		static const int _ID = 1;

		NMIInterrupt ()
			: Interrupt (_ID, 11 /** like a restart operation. */)
							{ setClassName ("NMIInterrupt"); }

		private:
		virtual bool isTime (MCHEmul::CPU* c, unsigned int cC) const override
							{ return (true); } // The NMI Interruptions are always accepted!
		virtual bool executeOverImpl (MCHEmul::CPU* c, unsigned int cC) override;
	};
}

#endif
  
// End of the file
/*@}*/