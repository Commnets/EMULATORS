#include <CORE/SoundChip.hpp>

// ---
bool MCHEmul::SoundChip::initialize ()
{
	if (_soundMemory != nullptr)
		delete (_soundMemory); // When reinit e.g....

	if ((_soundMemory = createSoundMemory ()) == nullptr)
		return (false);

	// Must be initialized to avoid garbage data...
	_soundMemory -> initialize ();

	return (true);
}
