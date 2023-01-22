/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: DatasettePort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 22/01/2023 \n
 *	Description:	The DatasetteIOPOrt within the C64 is connected with some chips and memoty!
 *	Versions: 1.0 Initial
 */

#ifndef __C64_DATASETTEPORT__
#define __C64_DATASETTEPORT__

#include <COMMODORE/incs.hpp>

namespace C64
{
	class CIA1;
	class SpecialFunctionsChip;

	/** This class represents the UserIOPort. */
	class DatasetteIOPort final : public COMMODORE::DatasetteIOPort
	{
		public:
		DatasetteIOPort ()
			: COMMODORE::DatasetteIOPort ()
							{ }

		virtual void linkToChips (const MCHEmul::Chips& c) override;

		private:
		CIA1* _cia1;
		SpecialFunctionsChip* _specialChip;
	};
}

#endif
  
// End of the file
/*@}*/
