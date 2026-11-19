/** \ingroup CORE */
/*@{*/

/**	
 *	@file	
 *	File: Sound.hpp \n
 *	Framework: CPU Emulators library \n
 *	Author: Ignacio Cea Fornies (EMULATORS library) \n
 *	Creation Date: 13/01/2021 \n
 *	Description: To manage the outputs to the Sound system.
 *	Versions: 1.0 Initial
 */

#ifndef __MCHEMUL_SOUND__
#define __MCHEMUL_SOUND__

#include <CORE/IO.hpp>
#include <CORE/SoundChip.hpp>
#include <CORE/SDL.hpp>
#include <vector>

namespace MCHEmul
{
	/** Represents the generation of outputs to the sound system. */
	class SoundSystem : public IODevice
	{
		public:
		SoundSystem (int id, 
			SDL_AudioFormat tp, int sF, unsigned char nC,
				const Attributes& attrs = { }, double mADelay = 0.05 /** Meaning 5%. */);

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

		private:
#if MCHEMUL_SDL_VERSION == 2
		bool openAudioSDL2 ();
		bool initializeAudioSDL2 (size_t& normalBufferCapacity);
		bool queueSoundSDL2 ();
#elif MCHEMUL_SDL_VERSION == 3
		bool openAudioSDL3 ();
		bool initializeAudioSDL3 (size_t& normalBufferCapacity);
		bool queueSoundSDL3 ();
#endif

		/** Completes a sound block with the requested number of additional bytes. \n
			The buffer is modified in place and the returned value is its final valid
			size. No data is added unless _addAdditionalSoundData is true. \n
			additionalData must contain complete output frames and the buffer must
			already include enough capacity, reserved during initialize (). */
		size_t fillAdditionalSoundData (
			std::vector <unsigned char>& soundBuffer,
			size_t soundSize,
			size_t additionalData);

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
		/** SDL2 owns this device directly. SDL3's playback stream owns it. */
		SDL_AudioDeviceID _deviceId;
		/** SDL2's obtained output format, or SDL3's playback stream input format. */
		SDL_AudioSpec _audioSpec;
#if MCHEMUL_SDL_VERSION == 2
		bool _conversionNeeded;
		SDL_AudioCVT _conversionData;
#elif MCHEMUL_SDL_VERSION == 3
		/** Owned conversion stream: chip samples to SoundSystem output frames. */
		SDL_AudioStream* _conversionStream;
		/** Owned playback stream; destroying it also closes its device. */
		SDL_AudioStream* _playbackStream;
#endif
		/** Reusable conversion workspace followed by the delay compensation area. */
		std::vector <unsigned char> _conversionBuffer;
		/** Bytes available for normal conversion, excluding compensation space. */
		size_t _normalBufferCapacity;
		/** Playback queue limit in bytes of the submitted audio format. */
		size_t _maximumQueuedAudioBytes;

		// Special parameters dedicated to adjust the buffer simulated...
		/** The maximum delay allowed in the buffer. \n
			It is a percentage of the total buffer size. */
		double _maxAllowedDelay;
		/** Size in bytes of one output frame, including all its channels. */
		size_t _outputFrameSize;
		/** Maximum number of complete output frames reserved to compensate delay. \n
			This space is allocated but not used or queued yet. */
		size_t _maximumAdditionalFrames;
		/** Frequency, in counters per second, of SDL's high-resolution clock. */
		unsigned long long _performanceCounterFrequency;
		/** Nominal number of high-resolution counters between two sound-ready events. */
		unsigned long long _nominalSoundReadyCounters;
		/** Maximum number of delayed counters accepted according to _maxAllowedDelay. */
		unsigned long long _maximumAllowedDelayCounters;
		/** High-resolution counter captured when the previous sound-ready event arrived. \n
			A zero value means that there is no previous event to compare with yet. */
		unsigned long long _lastSoundReadyCounter;
		/** Real number of counters elapsed between the last two sound-ready events. */
		unsigned long long _lastSoundReadyElapsedCounters;
		/** Counters by which the last sound-ready event exceeded its nominal period. */
		unsigned long long _soundReadyDelayCounters;
		/** Additional bytes that would be needed to cover the accepted delay. \n
			It is always zero or a multiple of _outputFrameSize. */
		size_t _additionalSoundData;
		/** Whether the last sound-ready event arrived after its nominal time. */
		bool _soundReadyDelayed;
		/** Whether _additionalSoundData contains a compensable amount of data. \n
			The reserved data is only described by these variables and is not used yet. */
		bool _addAdditionalSoundData;
	};
}

#endif

// End of the file
/*@}*/