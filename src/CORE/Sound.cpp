#include <CORE/Sound.hpp>

// ---
MCHEmul::SoundSystem::SoundSystem (int id, double hz, const MCHEmul::Attributes& attrs)
	: IODevice (Type::_OUTPUT, id, attrs),
	  _hertzs (hz),
	  _clock ((unsigned int) hz)
{ 
	// TODO
}

// ---
bool MCHEmul::SoundSystem::initialize ()
{
	// TODO

	return (true);
}

// ---
bool MCHEmul::SoundSystem::simulate ()
{
	// TODO

	return (true);
}
