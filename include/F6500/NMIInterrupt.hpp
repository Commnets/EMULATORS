/** \ingroup F6500 */
/*@{*/

/**	
 *	@file	
 *	File: NMIInterrupt.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 14/05/2021 \n
 *	Description: Th NMI type of interrupt in F6500.
 *	Versions: 1.0 Initial
 */

#ifndef __F6500_NMIINTERRUPT__
#define __F6500_NMIINTERRUPT__

#include <CORE/incs.hpp>

namespace F6500
{
	/** A non Maskarable Interrupt. Any time it is invoked it has to be executed. */
	class NMIInterrupt : public MCHEmul::CPUInterrupt
	{
		public:
		static const int _ID = 1;

		NMIInterrupt ()
			: MCHEmul::CPUInterrupt (_ID)
							{ }

		protected:
		/** Always. It is a Non Maskarable Interrupt. */
		virtual bool isTime (MCHEmul::CPU* c) const override 
							{return (true); } 
		virtual void executeOverImpl (MCHEmul::CPU* c, unsigned int& nC) override;
	};
}

#endif
  
// End of the file
/*@}*/