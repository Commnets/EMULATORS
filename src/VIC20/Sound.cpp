#include <VIC20/Sound.hpp>

// ---
VIC20::SoundSystem::SoundSystem ()
	: MCHEmul::SoundSystem (_ID, 
		AUDIO_F32SYS, 44100, 1,
		{ { "Name", "Sound System" },
		  { "Type", "Output" },
		  { "Max Frequency", "4kHz" } })
{
	// Nothing else to do...
}
