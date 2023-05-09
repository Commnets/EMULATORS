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
#include <CORE/SoundChip.hpp>
#include <SDL.h>

namespace MCHEmul
{
	/** Represents the generation of outputs to the sound system. */
	class SoundSystem : public IODevice
	{
		public:
		SoundSystem (int id, 
			SDL_AudioFormat tp, int sF, unsigned char nC, const Attributes& attrs = { });

		~SoundSystem ();

		virtual void linkToChips (const Chips& c) override;

		/** To put the sound on/off */
		bool silence () const
							{ return (_silence); }
		void setSilence (bool s)
							{ _silence = s; }

		/** The reference to the sopund chip. 
			It can be null as it is not mandatory the relation between them. */
		const SoundChip* soundChip () const
							{ return (_soundChip); }
		SoundChip* soundChip ()
							{ return (_soundChip); }

		/** To get the value of parameters. */
		SDL_AudioFormat type () const
							{ return (_type); }
		int samplingFrequency () const
							{ return (_samplingFrequency); }
		unsigned char numberChannels () const
							{ return (_numberChannels); }

		/** Not possible to connect any peripheral. */
		virtual bool connectPeripheral (IOPeripheral* p) override
							{ return (false); }

		virtual bool initialize () override;

		virtual bool simulate (CPU* cpu) override;

		/**
		  *	Adding to parent fields:
		  * NAME	= Name of the screen. \n
		  * HERTZS	= The refresh rate. \n
		  */
		virtual InfoStructure getInfoStructure () const override;

		protected:
		virtual void processEvent (const Event& evnt, Notifier* n) override;

		protected:
		/** The type of any sample. \n
			It is used by SDL to determine later the structure of the internal buffer used
			in the communication with the sound card. */
		SDL_AudioFormat _type;
		/** The sampling frequency. \n
			It must be double than the maximum allowed (according with the characteristics of the chip connected). */
		int _samplingFrequency;
		/** The number of channels to be used. */
		unsigned char _numberChannels;
		/** true when there is no sound, and false in other case. */
		bool _silence;
	
		// Implementation
		SoundChip* _soundChip;
		/** To indicate that the sounds at chip level are ready
			and the soundc ard can be actualized. */
		bool _soundReady;
		/** The audio data. */
		SDL_AudioDeviceID _deviceId;
		SDL_AudioSpec _audioSpec;
		/** To convert the sound data from the chip format to the sound card format when needed. \n
			To identify whether the conversion of info is needed. */
		bool _conversionNeeded;
		SDL_AudioCVT _conversionData;
	};
}

#endif

// End of the file
/*@}*/