/** \ingroup COMMODORE */
/*@{*/

/**	
 *	@file	
 *	File: SID.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 13/01/2023 \n
 *	Description: SID Chip Emulation.
 *				 The emulation is created using the RESID library:
 *				 http://www.zimmers.net/anonftp/pub/cbm/crossplatform/emulators/resid/index.html
 *				 This library has been integrated in many emulators like VICE (my reference).
 *	Versions: 1.0 Initial
 */

#ifndef __COMMODORE_SID__
#define __COMMODORE_SID__

#include <CORE/incs.hpp>
#include <COMMODORE/SIDLibWrapper.hpp>
#include <COMMODORE/SIDRegisters.hpp>

namespace COMMODORE
{
	/** The chip is to produce the sound. */
	class SID : public MCHEmul::SoundChip
	{
		public:
		static const unsigned int _ID = 105;

		/** The parameter is the clock speed. */
		SID (unsigned int cF);

		/** The main characteristics of the SID. */
		virtual SDL_AudioFormat type () const override
							{ return (AUDIO_U8); }
		virtual int maxFrequency () const override
							{ return (44100); }
		virtual unsigned char numberChannels () const override
							{ return (1); }

		virtual bool initialize () override;

		virtual bool simulate (MCHEmul::CPU* cpu) override;

		virtual MCHEmul::InfoStructure getInfoStructure () const override;

		private:
		SIDRegisters* _SIDRegisters;

		// Implementation
		/** A reference to the SID implementation of the resid library. */
		mutable unsigned int _lastClockCycles;
	};
}

#endif
  
// End of the file
/*@}*/
