/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: Sound.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 13/01/2021 \n
 *	Description: To manage the outputs to the Sound system.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_SOUND__
#define __MCHEMUL_SOUND__

#include <CORE/IO.hpp>
#include <CORE/Clock.hpp>

namespace MCHEmul
{
	/** Represents the generation of outputs to the sound system. */
	class SoundSystem : public IODevice
	{
		public:
		SoundSystem (int id, double hz, const Attributes& attrs = { });

		virtual bool initialize () override;

		virtual bool simulate (CPU* cpu) override;

		protected:
		/** The parsing frequency. */
		const double _hertzs;
		
		// Implementation
		/** A clock to control the frequency of parsing. */
		Clock _clock;
	};
}

#endif

// End of the file
/*@}*/