/** \ingroup VIC20 */
/*@{*/

/**	
 *	@file	
 *	File: DatasettePort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
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

	/** This class represents the DatasettePort. */
	class DatasetteIOPort final : public COMMODORE::DatasetteIOPort
	{
		public:
		DatasetteIOPort ()
			: COMMODORE::DatasetteIOPort (),
			  _via1 (nullptr)
							{ }

		virtual void linkToChips (const MCHEmul::Chips& c) override;

		private:
		VIA1* _via1;
	};
}

#endif
  
// End of the file
/*@}*/
