#include <C64/Sound.hpp>

// ---
C64::SoundSystem::SoundSystem ()
	: MCHEmul::SoundSystem (_ID, 50, 
		{ { "Name", "Sound System" },
		  { "Type", "Output" },
		  { "Frequency", "16kHz" } })
{
	// Nothing else to do...
}
