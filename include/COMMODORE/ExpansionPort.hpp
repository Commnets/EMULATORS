/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: ExpansionPort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 22/01/2023 \n
 *	Description:	To emulate the behaviour of the Expajsion Port.
 *				 	Usually cartridges are connected here.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_EXPANSIONPORT__
#define __COMMODORE_EXPANSIONPORT__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	class ExpansionPeripheral;

	/** This class represents the Expansion Port. \n
		The class is not final because when it is attached to a specific computer,
		the links to chip o memory zones could be different. */
	class ExpansionIOPort : public MCHEmul::IODevice
	{
		public:
		static const int _ID = 102;

		/** Different events. */
		static const unsigned int _CARTRIDGEIN		= 300;

		ExpansionIOPort ();

		virtual bool initialize () override
							{ _firstExecution = true; return (true); }

		/** It verifies before adding it that whether the peripherial is somtehing compatible. */
		virtual bool connectPeripheral (MCHEmul::IOPeripheral* p) override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		private:
		// Implementation...
		ExpansionPeripheral* _expansionElement;
		bool _firstExecution;
	};
}

#endif
  
// End of the file
/*@}*/
