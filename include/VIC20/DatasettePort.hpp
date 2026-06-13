/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: DatasettePort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 03/01/2024 \n
 *	Description: The DatasetteIOPOrt within the VIC20 is connected with some chips and memory!
 *	Versions: 1.0 Initial
 */

#ifndef __VIC20_DATASETTEPORT__
#define __VIC20_DATASETTEPORT__

#include <COMMODORE/incs.hpp>

namespace VIC20
{
	class VIA1;
	class VIA2;

	/** This class represents the DatasettePort. */
	class DatasetteIOPort final : public COMMODORE::DatasetteIOPort
	{
		public:
		DatasetteIOPort ()
			: COMMODORE::DatasetteIOPort (),
			  _via1 (nullptr), _via2 (nullptr)
							{ }

		/** 
		  * The DatasettePort is connected with the VIA1 and the VIA2: \n
		  *	VIA1: \n
		  *	----- \n
		  *	- PA6 reads tape SENSE, active low when any datasette key is pressed. \n
		  *	- CA2 controls the datasette motor. CA2 low means motor running. \n
		  *	VIA2: \n
		  *	----- \n
		  *	- CA1 receives tape READ transitions. \n
		  *	- PB3 drives tape WRITE. \n
		  **/ 
		virtual void linkToChips (const MCHEmul::Chips& c) override;

		private:
		VIA1* _via1;
		VIA2* _via2;
	};
}

#endif
  
// End of the file
/*@}*/
