#include <MSX/Sound.hpp>

// ---
MSX::SoundSystem::SoundSystem ()
	: MCHEmul::SoundSystem (_ID, 
		AUDIO_F32SYS, 44100, 2,
		{ { "Name", "Sound System" },
		  { "Type", "Output" },
		  { "Max Frequency", "4kHz" } })
{
	// Nothing else to do...
}
