/** \ingroup F6500 */
/*@{*/

/**	
 *	@file	
 *	File: NMIInterrupt.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 14/05/2021 \n
 *	Description: Th NMI type of interrupt in F6500.
 *	Versions: 1.0 Initial
 */

#ifndef __F6500_NMIINTERRUPT__
#define __F6500_NMIINTERRUPT__

#include <CORE/incs.hpp>
#include <F6500/Interrupt.hpp>

namespace F6500
{
	/** A non Maskarable Interrupt. Any time it is invoked it has to be executed. */
	class NMIInterrupt final : public Interrupt
	{
		public:
		static const int _ID = 1;

		NMIInterrupt ()
			: Interrupt (_ID)
							{ setClassName ("NMIInterrupt"); }

		private:
		virtual bool executeOverImpl (MCHEmul::CPU* c, unsigned int cC) override;
	};
}

#endif
  
// End of the file
/*@}*/