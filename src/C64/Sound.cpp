#include <C64/Sound.hpp>

// ---
C64::SoundSystem::SoundSystem ()
	: MCHEmul::SoundSystem (_ID, 50, 
		{ { "Type", "Sound System" },
		  { "Frequency", "16kHz" } })
{
	// Nothing else to do...
}
