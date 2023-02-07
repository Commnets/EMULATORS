#include <CORE/Sound.hpp>

// ---
MCHEmul::SoundSystem::SoundSystem (int id, double hz, const MCHEmul::Attributes& attrs)
	: MCHEmul::IODevice (Type::_OUTPUT, id, attrs),
	  _hertzs (hz),
	  _clock ((unsigned int) hz)
{ 
	setClassName ("SOUNDSYSTEM");
}

// ---
bool MCHEmul::SoundSystem::initialize ()
{
	// TODO

	return (true);
}

// ---
bool MCHEmul::SoundSystem::simulate (MCHEmul::CPU* cpu)
{
	if (!MCHEmul::IODevice::simulate (cpu))
		return (false);

	// TODO

	return (true);
}
