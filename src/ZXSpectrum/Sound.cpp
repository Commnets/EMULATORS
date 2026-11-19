#include <ZXSpectrum/Sound.hpp>

// ---
ZXSPECTRUM::SoundSystem::SoundSystem ()
	: MCHEmul::SoundSystem (_ID, 
#if MCHEMUL_SDL_VERSION == 2
		AUDIO_F32SYS, 44100, 2,
#elif MCHEMUL_SDL_VERSION == 3
		SDL_AUDIO_F32, 44100, 2,
#endif
		{ { "Name", "Sound System" },
		  { "Type", "Output" },
		  { "Max Frequency", "4kHz" } })
{
	// Nothing else to do...
}
