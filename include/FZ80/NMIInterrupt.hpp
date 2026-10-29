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

		NMIInterrupt ();

		/** Complete nominal NMI response, including both stack writes. */
		virtual const FZ80::MCyclesData* memoryAccessTiming () const override
							{ return (&_responseTiming); }

		private:
		virtual unsigned int isTime (MCHEmul::CPU* c, unsigned int cC) const override
							{ return (MCHEmul::CPUInterrupt::_EXECUTIONALLOWED); } // The NMI Interruptions are always accepted!
		virtual bool executeOverImpl (MCHEmul::CPU* c, unsigned int cC) override;

		private:
		/** Nominal entry span followed by the two return-address writes. */
		const FZ80::MCyclesStructure _responseMCycles;

		// Implementation
		/** Immutable timing indexes owned for the lifetime of this interrupt. */
		const FZ80::MCyclesData _responseTiming;
	};
}

#endif
  
// End of the file
/*@}*/