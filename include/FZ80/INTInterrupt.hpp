/** \ingroup FZ80 */
/*@{*/

/**	
 *	@file	
 *	File: INTInterrupt.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 11/11/2023 \n
 *	Description: The INT type of interrupt in FZ80.
 *	Versions: 1.0 Initial
 */

#ifndef __FZ80_INTINTERRUPT__
#define __FZ80_INTINTERRUPT__

#include <CORE/incs.hpp>
#include <FZ80/Interrupt.hpp>

namespace FZ80
{
	/** A Maskarable Interrupt. It could be avoided using the bit I of the status flag. */
	class INTInterrupt final : public Interrupt
	{
		public:
		static const unsigned int _ID = 0;

		INTInterrupt ()
			: Interrupt (_ID)
							{ setClassName ("INTInterrupt"); }

		/**
		  *	The name of the fields are: \n
		  *	The ones from the CPUInterrupt +
		  *	ADDRESS			= The address where the IRQ should start the execution from.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		virtual bool isTime (MCHEmul::CPU* c, unsigned int cC) const override; 
		virtual bool executeOverImpl (MCHEmul::CPU* c, unsigned int cC) override;
	};
}

#endif
  
// End of the file
/*@}*/