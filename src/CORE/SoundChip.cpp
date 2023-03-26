#include <CORE/soundChip.hpp>

// ---
bool MCHEmul::SoundChip::initialize ()
{
	if (_soundMemory != nullptr)
		delete (_soundMemory); // When reinit e.g....

	_soundMemory = createSoundMemory ();

	return (_soundMemory != nullptr);
}
