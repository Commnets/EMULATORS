/** \ingroup F6500 */
/*@{*/

/**	
 *	@file	
 *	File: IRQInterrupt.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 14/05/2021 \n
 *	Description: Th IRQ type of interrupt in F6500.
 *	Versions: 1.0 Initial
 */

#ifndef __F6500_IRQINTERRUPT__
#define __F6500_IRQINTERRUPT__

#include <CORE/incs.hpp>
#include <F6500/Interrupt.hpp>

namespace F6500
{
	/** A Maskarable Interrupt. It could be avoided using the bit I of the status flag. */
	class IRQInterrupt final : public Interrupt
	{
		public:
		static const int _ID = 0;

		IRQInterrupt ()
			: Interrupt (_ID, 0) // The lowest priority...
							{ setClassName ("IRQInterrupt"); }

		private:
		/** Only when the status flag B allows it. It is a Maskarable Interrupt. */
		virtual unsigned int isTime (MCHEmul::CPU* c, unsigned int cC) const override; 
		virtual bool executeOverImpl (MCHEmul::CPU* c, unsigned int cC) override;
	};
}

#endif
  
// End of the file
/*@}*/