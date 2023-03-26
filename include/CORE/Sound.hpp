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
			unsigned short tp, int sF, unsigned char nC, unsigned short nS, const Attributes& attrs = { });

		~SoundSystem ();

		virtual void linkToChips (const Chips& c) override;

		/** The reference to the sopund chip. 
			It can be null as it is not mandatory the relation between them. */
		const SoundChip* graphicalChip () const
							{ return (_soundChip); }
		SoundChip* soundChip ()
							{ return (_soundChip); }

		/** To get the value of parameters. */
		unsigned short type () const
							{ return (_type); }
		int samplingFrequency () const
							{ return (_samplingFrequency); }
		unsigned char numberChannels () const
							{ return (_numberChannels); }
		unsigned short numberSamples () const
							{ return (_numberSamples); }

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
		const unsigned short _type;
		/** The sampling frequency. \n
			It must be double than the maximum allowed (according with the characteristics of the chip connected). */
		const int _samplingFrequency;
		/** The number of channels to be used. */
		const unsigned char _numberChannels;
		/** The number of samples any chunk of information sent from the chip. */
		const unsigned short _numberSamples;
	
		// Implementation
		SoundChip* _soundChip;
		/** To indicate that the sounds at chip level are ready
			and the soundc ard can be actualized. */
		bool _soundReady;
		/** The audio data. */
		SDL_AudioDeviceID _deviceId;
		SDL_AudioSpec _audioSpec;
	};
}

#endif

// End of the file
/*@}*/