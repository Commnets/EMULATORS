/** \ingroup MSX */
/*@{*/

/**	
 *	@file	
 *	File: DatasettePort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 17/07/2026 \n
 *	Description:	To emulate the behaviour of the Datasette Port.
 *				 	Usually the datasette Unit is connected here.
 *	Versions: 1.0 Initial
 */

#ifndef __MSX_DATASETTEPORT__
#define __MSX_DATASETTEPORT__

#include <CORE/incs.hpp>

namespace MSX
{
	/** This class represents the place a MSX Datasette connects. \n
		The trap-injection peripheral uses the standard datasette port contract.
		Physical cassette signal wiring through PPI and PSG is intentionally separate. */
	class DatasetteIOPort final : public MCHEmul::DatasetteIOPort
	{
		public:
		static const int _ID = 100;

		DatasetteIOPort ();
	};
}

#endif
  
// End of the file
/*@}*/
