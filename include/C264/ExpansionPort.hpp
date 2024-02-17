/** \ingroup C264 */
/*@{*/

/**	
 *	@file	
 *	File: ExpansionPort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 04/02/2024 \n
 *	Description:	Just to give name to the different PINS of the ExpansionPort in the C264 series.
 *	Versions: 1.0 Initial
 */

#ifndef __C264_EXPANSIONPORT__
#define __C264_EXPANSIONPORT__

#include <COMMODORE/incs.hpp>

namespace C264
{
	class ExpansionIOPort final : public COMMODORE::ExpansionIOPort
	{
		public:
		// Different events.
		/** Event to inform the structure of the memory. 
			This is event is sent when the cartridge is ready to work and it is to restructure 
			the way the memory is configured at the C264. */
		static const unsigned int _C264EXPANSIONPORTMEMLINESACTUALIZED = 360;

		ExpansionIOPort ()
			: COMMODORE::ExpansionIOPort ()
							{ }

		/** Overload the parent method, 
			because any change in the lines defining the structure of the memory has to be notified,
			and not only a change in the data loaded that is what happened at parent level only. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;
	};
}

#endif
  
// End of the file
/*@}*/
