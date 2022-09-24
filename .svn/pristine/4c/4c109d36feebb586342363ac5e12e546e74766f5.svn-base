/** \ingroup F6500 */
/*@{*/

/**	
 *	@file	
 *	File: IRQInterrupt.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 14/05/2021 \n
 *	Description: Th IRQ type of interrupt in F6500.
 *	Versions: 1.0 Initial
 */

#ifndef __F6500_IRQINTERRUPT__
#define __F6500_IRQINTERRUPT__

#include <CORE/incs.hpp>

namespace F6500
{
	/** A Maskarable Interrupt. It could be avoided using the bit I of the status flag. */
	class IRQInterrupt : public MCHEmul::CPUInterrupt
	{
		public:
		static const unsigned int _ID = 0;

		IRQInterrupt ()
			: MCHEmul::CPUInterrupt (_ID)
							{ }

		protected:
		/** Only when the status flag B allows it. It is a Maskarable Interrupt. */
		virtual bool isTime (MCHEmul::CPU* c) const override; 
		virtual void executeOverImpl (MCHEmul::CPU* c, unsigned int& nC) override;
	};
}

#endif
  
// End of the file
/*@}*/