#include <C64/Sound.hpp>

// ---
C64::SoundSystem::SoundSystem ()
	: MCHEmul::SoundSystem (_ID, 
		AUDIO_F32, 8000 /** = 4 KHz * 2 */, 2, 80, // It is represented in a typical windows system...
		{ { "Name", "Sound System" },
		  { "Type", "Output" },
		  { "Max Frequency", "4kHz" } })
{
	// Nothing else to do...
}
