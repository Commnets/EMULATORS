/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: DatasettePort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 21/01/2023 \n
 *	Description:	To emulate the behaviour of the Datasette Port.
 *				 	Usually the 1530 Unit (or 1531) is connected here.
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_DATASETTEPORT__
#define __COMMODORE_DATASETTEPORT__

#include <CORE/incs.hpp>

namespace COMMODORE
{
	class DatasettePeripheral;

	/** This class represents the place a COMMODORRE Datasette connects. \n
		It is pretty much like the standard one, but assigned already a specific Id. \n
		The class is not final because when it is attached to a specific computer,
		the links to chip o memory zones could be different. */
	class DatasetteIOPort : public MCHEmul::DatasetteIOPort
	{
		public:
		static const int _ID = 100;

		DatasetteIOPort ();
	};
}

#endif
  
// End of the file
/*@}*/
