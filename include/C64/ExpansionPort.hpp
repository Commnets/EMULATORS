/** \ingroup C64 */
/*@{*/

/**	
 *	@file	
 *	File: ExpansionPort.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 22/01/2023 \n
 *	Description:	Just to give name to the different PINS of the ExpansionPort.
 *	Versions: 1.0 Initial
 */

#ifndef __C64_EXPANSIONPORT__
#define __C64_EXPANSIONPORT__

#include <COMMODORE/incs.hpp>

namespace C64
{
	class PLA;

	class ExpansionIOPort final : public COMMODORE::ExpansionIOPort
	{
		public:
		/** Name of the different PINS in the C64 implementation of the Expansion Port. */
		static const unsigned char _GAME = 8;
		static const unsigned char _EXROM = 9;

		// Different events.
		/** Event to inform the structure of the memory. 
			This is event is sent when the cartridge is ready to work and it is to restructure 
			the way the memory is configured at the C64. */
		static const unsigned int _C64EXPANSIONPORTMEMLINESACTUALIZED = 360;

		ExpansionIOPort ()
			: COMMODORE::ExpansionIOPort (),
			  _pla (nullptr)
							{ }

		/** The C64 Expansion port is observed from the PLA chip. */
		virtual void linkToChips (const MCHEmul::Chips& c) override;

		/** Overload the parent method, 
			because any change in the lines defining the structure of the memory has to be notified,
			and not only a change in the data loaded that is what happened at parent level only. */
		virtual bool simulate (MCHEmul::CPU* cpu) override;

		/** Events comming from the memory. */
		virtual void processEvent (const MCHEmul::Event& evnt, MCHEmul::Notifier* ntfier) override;

		private:
		PLA* _pla;
	};
}

#endif
  
// End of the file
/*@}*/
