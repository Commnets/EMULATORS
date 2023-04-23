#include <CORE/soundChip.hpp>

// ---
MCHEmul::InfoStructure MCHEmul::SoundLibWrapper::getInfoStructure () const
{
	MCHEmul::InfoStructure result = std::move (MCHEmul::InfoClass::getInfoStructure ());

	result.add ("ATTRS", _attributes);

	return (result);
}

// ---
bool MCHEmul::SoundChip::initialize ()
{
	if (_soundMemory != nullptr)
		delete (_soundMemory); // When reinit e.g....

	_soundMemory = createSoundMemory ();

	return (_soundMemory != nullptr);
}
