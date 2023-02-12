/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: 1530Datasette.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 21/01/2023 \n
 *	Description: To emulate the behaviour of the 1530 Datasette Unit.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_1530DATASETTE__
#define __COMMODORE_1530DATASETTE__

#include <CORE/incs.hpp>
#include <COMMODORE/DatasettePeripherals.hpp>

namespace COMMODORE
{
	/** This unit was used in many COMMODORE models. \n
		Mainly all of them except COMMODORE16. */
	class Datasette1530 : public DatasettePeripheral
	{
		public:
		static const int _ID = 100;

		Datasette1530 ();

		virtual bool executeCommand (int id, const MCHEmul::Strings& prms) override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;
	};
}

#endif
  
// End of the file
/*@}*/
