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

	// The wrapper is also initialized if it is not nullptr...	
	if (_soundWrapper != nullptr)
		_soundWrapper -> initialize ();

	return (true);
}
