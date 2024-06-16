/** \ingroup ZX81 */
/*@{*/

/**	
 *	@file	
 *	File: DatasettePort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 14/06/2024 \n
 *	Description:	The DatasetteIOPOrt within the ZX81 is direcly a in/out sound port!
 *	Versions: 1.0 Initial
 */

#ifndef __ZX81_DATASETTEPORT__
#define __ZX81_DATASETTEPORT__

#include <SINCLAIR/incs.hpp>

namespace ZX81
{
	/** This class represents the DatasettePort. */
	class DatasetteIOPort final : public MCHEmul::DatasetteIOPort
	{
		public:
		static const int _ID = 100; // Similar to other semulators...

		DatasetteIOPort ();
	};
}

#endif
  
// End of the file
/*@}*/
