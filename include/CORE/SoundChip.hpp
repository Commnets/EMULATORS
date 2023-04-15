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
#include <SDL.h>

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
		// Different events.
		/** When a SoundChip is ready to send out the sound buffer created,
			this event has to be sent. \n
			The event has to be received from a Sound IO Device (@see SoundSystem) for more info. */
		static const unsigned int _SOUNDREADY				= 101;

		SoundChip (int id, const Attributes& attrs = { })
			: Chip (id, attrs), 
			  _soundMemory (nullptr)
							{ }

		virtual ~SoundChip () override
							{ delete (_soundMemory); }

		/** To know the characteristic of the soun chip. */
		virtual SDL_AudioFormat type () const = 0;
		virtual int maxFrequency () const = 0;
		int samplingFrecuency () const
							{ return (maxFrequency () << 1); }
		unsigned char sampleSize () const
							{ return ((type () & 0x00ff) >> 3); } // From the type always... (@see format of SDL_AudioSpec)
		virtual unsigned char numberChannels () const = 0;
		// The theoretical number of elements of the buffer (frequency * channels * sampleSize) is divided by 10...
		// ..meaning that the buffer will be filled 100 times per second!...
		int soundBufferSize () const
							{ return ((samplingFrecuency () * (int) numberChannels () * (int) sampleSize ()) / 100); }

		// They will be nullptr if the initialize method hasn't been invoked...
		const SoundMemory* soundMemory () const
							{ return (_soundMemory); }
		SoundMemory* soundMemory ()
							{ return (_soundMemory); }

		virtual bool initialize () override;

		protected:
		/** Invoked from initialize method to create the right sound memory. \n
			The SounChip implementation must fill up this memory and notify _SOUNDREADY event when finished. */
		virtual SoundMemory* createSoundMemory ()
							{ return (new SoundMemory (soundBufferSize () / sampleSize (), sampleSize ())); }

		protected:
		SoundMemory* _soundMemory;
	};
}

#endif
  
// End of the file
/*@}*/

