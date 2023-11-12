/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: Interrupt.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/11/2023 \n
 *	Description: The generical definition of a interrupt in a Z80 processor.
 *	Versions: 1.0 Initial
 */

#ifndef __FZ80_CZ80INTERRUPT__
#define __FZ80_CZ80INTERRUPT__

#include <CORE/incs.hpp>

namespace FZ80
{
	/** The way they are launched is special. */
	class Interrupt : public MCHEmul::CPUInterrupt
	{
		public:
		Interrupt (int id)
			: MCHEmul::CPUInterrupt (id, 7)
							{ }

		protected:
		/** As the code is designed, the interrupt is invoked after the execution of one instruction,
			when the _lockCycles of the CPU have been actualized, 
			but it should have been launched at cC cycles (the chip decides when). \n
			In the 6500 family a interrupt can not be launched never before 2 cycles the end of the last instruction. */
		virtual bool isTime (MCHEmul::CPU* c, unsigned int cC) const override;
		/** Just to put back the counter to 0. */
		virtual bool executeOverImpl (MCHEmul::CPU* c, unsigned int cC) override;
	};
}

#endif
  
// End of the file
/*@}*/