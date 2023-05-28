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
	class NMIInterrupt final : public MCHEmul::CPUInterrupt
	{
		public:
		static const int _ID = 1;

		NMIInterrupt ()
			: MCHEmul::CPUInterrupt (_ID),
			  _exeAddress ()
							{ setClassName ("NMIInterrupt"); }

		/**
		  *	The name of the fields are: \n
		  *	The ones from the CPUInterrupt +
		  *	ADDRESS			= The address where the NMI should start the execution from.
		  */
		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		/** Always. It is a Non Maskarable Interrupt. */
		virtual bool isTime (MCHEmul::CPU* c) const override 
							{return (true); } 
		virtual bool executeOverImpl (MCHEmul::CPU* c, unsigned int& nC) override;

		private:
		// Implementation
		MCHEmul::Address _exeAddress;
	};
}

#endif
  
// End of the file
/*@}*/