#include <C64/Sound.hpp>

// ---
C64::SoundSystem::SoundSystem ()
	: MCHEmul::SoundSystem (_ID, 
		AUDIO_F32, 44100 /** = 2 KHz * 2 */, 2,
		{ { "Name", "Sound System" },
		  { "Type", "Output" },
		  { "Max Frequency", "4kHz" } })
{
	// Nothing else to do...
}
