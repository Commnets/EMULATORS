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
		Interrupt (int id)
			: MCHEmul::CPUInterrupt (id, 7),
			  _instChecked (false),
			  _exeAddress ()
							{ }

		unsigned int readingCyclesTolaunch () const
							{ return (4); }

		/**
		  *	The name of the fields are: \n
		  *	The ones from the CPUInterrupt +
		  *	ADDRESS			= The address where the NMI should start the execution from.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		/** As the code is designed, the interrupt is invoked after the execution of one instruction,
			when the _lockCycles of the CPU have been actualized, 
			but it should have been launched at cC cycles (the chip decides when). \n
			In the 6500 family a interrupt can not be launched never before 2 cycles the end of the last instruction. */
		virtual bool isTime (MCHEmul::CPU* c, unsigned int cC) const override;
		/** Just to put back the counter to 0. */
		virtual bool executeOverImpl (MCHEmul::CPU* c, unsigned int cC) override
							{ _instChecked = false; /** Just in case. */ 
							  return (true); /** Not relevant. */ }

		protected:
		MCHEmul::Address _exeAddress;

		private:
		// Implementation
		mutable bool _instChecked;
	};
}

#endif
  
// End of the file
/*@}*/