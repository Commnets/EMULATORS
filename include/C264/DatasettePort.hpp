/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: DatasettePort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 04/02/2024 \n
 *	Description: The DatasetteIOPOrt within the C264 series is connected with some chips and memory!
 *	Versions: 1.0 Initial
 */

#ifndef __C264_DATASETTEPORT__
#define __C264_DATASETTEPORT__

#include <COMMODORE/incs.hpp>

namespace C264
{
	/** This class represents the DatasettePort. */
	class DatasetteIOPort final : public COMMODORE::DatasetteIOPort
	{
		public:
		DatasetteIOPort ()
			: COMMODORE::DatasetteIOPort ()
							{ }

		virtual void linkToChips (const MCHEmul::Chips& c) override;
	};
}

#endif
  
// End of the file
/*@}*/
