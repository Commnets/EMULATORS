/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: Interrupt.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
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
			: MCHEmul::CPUInterrupt (id, 7),
			  _exeAddress ()
							{ }

		/**
		  *	The name of the fields are: \n
		  *	The ones from the CPUInterrupt +
		  *	ADDRESS			= The address where the NMI should start the execution from.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		protected:
		// Implementation
		MCHEmul::Address _exeAddress;
	};
}

#endif
  
// End of the file
/*@}*/