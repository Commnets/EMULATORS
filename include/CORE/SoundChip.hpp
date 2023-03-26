/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: SoundChip.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Forniés (EMULATORS library) \n
 *	Creation Date: 24/03/2023 \n
 *	Description: A special type of chip mamanging a sound memory.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_SOUNDCHIP__
#define __MCHEMUL_SOUNDCHIP__

#include <CORE/Chip.hpp>
#include <CORE/SoundMemory.hpp>

namespace MCHEmul
{
	/**
	  * SoundChip is very connected with a Sound. \n
	  * The sound chip uses the SoundMemory to write. \n
	  * The same object will be taken by the Sound to finaly put the sound out to the sound card. \n
	  * The instance of the SoundMemory object used is created in the method "createSoundMemory ()" that has to be overload. \n
	  * When a sound is ready (from the Chip) a notification has to be issed.
	  * That notification will be received at the Sound System represetation to move it into the sound card.
	  */
	class SoundChip : public Chip
	{
		public:
		/** Different events. */
		static const unsigned int _SOUNDREADY				= 101;

		SoundChip (int id, const Attributes& attrs = { })
			: Chip (id, attrs), 
			  _soundMemory (nullptr)
							{ }

		virtual ~SoundChip () override
							{ delete (_soundMemory); }

		const SoundMemory* soundMemory () const
							{ return (_soundMemory); }
		SoundMemory* soundMemory ()
							{ return (_soundMemory); }

		virtual bool initialize () override;

		protected:
		/** Invoked from initialize to create the right sound memory. */
		virtual SoundMemory* createSoundMemory () = 0;

		protected:
		SoundMemory* _soundMemory;
	};
}

#endif
  
// End of the file
/*@}*/

