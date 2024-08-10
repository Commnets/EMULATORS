/** \ingroup ZXSPECTRUM */
/*@{*/

/**	
 *	@file	
 *	File: DatasettePort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 09/08/2024 \n
 *	Description:	The DatasetteIOPOrt within the ZXSpectrum is direcly a in/out sound port!
 *	Versions: 1.0 Initial
 */

#ifndef __ZXSPECTRUM_DATASETTEPORT__
#define __ZXSPECTRUM_DATASETTEPORT__

#include <SINCLAIR/incs.hpp>

namespace ZXSPECTRUM
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
