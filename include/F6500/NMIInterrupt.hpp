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
	/** A non Maskarable Interrupt. \n
		Any time it is invoked it has to be executed. \n 
		But once has been _considered ( == true) none else is possible 
		until is not longer _considered (_considereed = false) */
	class NMIInterrupt final : public Interrupt
	{
		public:
		static const int _ID = 1;

		NMIInterrupt ()
			: Interrupt (_ID, 1), // More priority than IRQ...
			  _considered (false)
							{ setClassName ("NMIInterrupt"); }

		virtual bool admitNewInterruptRequest () const override
							{ return (_considered == false); }
		virtual void setNewInterruptRequestAdmitted (bool iR) override
							{ _considered = iR; }

		private:
		virtual bool executeOverImpl (MCHEmul::CPU* c, unsigned int cC) override;

		private:
		bool _considered; // @see above
	};
}

#endif
  
// End of the file
/*@}*/