/** \ingroup F6500 */
/*@{*/

/**	
 *	@file	
 *	File: Interrupt.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 14/05/2021 \n
 *	Description: The generical definition of a interrupt in a 6500 processor.
 *	Versions: 1.0 Initial
 *			  1.1 Added the shared IRQ/NMI bus-cycle structure.
 */

#ifndef __F6500_C6500INTERRUPT__
#define __F6500_C6500INTERRUPT__

#include <CORE/incs.hpp>

namespace F6500
{
	/** The way they are launched is special. */
	class Interrupt : public MCHEmul::CPUInterrupt
	{
		public:
		Interrupt (int id, int pr);

		unsigned int readingCyclesTolaunch () const
							{ return ((unsigned int) busCycleData ()._numberReadCycles); }

		virtual void initialize () override;

		/**
		  *	The name of the fields are: \n
		  *	The ones from the CPUInterrupt +
		  *	REQCLOCK		= Attribute: The clock when the interrupt was requested. \n
		  * EXECCLOCK		= Attribute: The clock when the interrupt was executed. \n
		  *	ADDRESS			= Attribute: The address where the NMI should start the execution from.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		/** Determines whether the request was active at the interrupt-sampling point
			recorded by the last instruction executed by the 6500. */
		virtual unsigned int isTime (MCHEmul::CPU* c, unsigned int cC) const override;
		/** Nothing common has to be executed at this level. */
		virtual bool executeOverImpl (MCHEmul::CPU* c, unsigned int cC) override
							{ return (true); /** Not relevant. */ }

		protected:
		// Implementation
		/** Data about the moment of the execution. */
		MCHEmul::Address _exeAddress;
		mutable unsigned int _requestClock;
		mutable unsigned int _execClock;

		private:
		// Implementation
		/** IRQ and NMI share two initial reads, three stack writes and two vector reads. */
		static const MCHEmul::CycleStructure _CYCLESTRUCTURE;
	};
}

#endif
  
// End of the file
/*@}*/
