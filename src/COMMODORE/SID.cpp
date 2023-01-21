#include <COMMODORE/SID.hpp>

// ---
bool COMMODORE::SID::initialize ()
{
	assert (memoryRef () != nullptr);

	// Gets the memory block dedicated to the SID
	if (!(_SIDRegisters = 
		dynamic_cast <COMMODORE::SIDRegisters*> (memoryRef () -> subset (COMMODORE::SIDRegisters::_SIDREGS_SUBSET))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	// TODO

	return (true);
}

// ---
bool COMMODORE::SID::simulate (MCHEmul::CPU* cpu)
{
	// TODO

	return (true);
}

// ---
MCHEmul::InfoStructure COMMODORE::SID::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::Chip::getInfoStructure ();

	result.remove ("Memory"); // This is not neccesary...
	result.add ("Registers",	_SIDRegisters -> getInfoStructure ());

	return (result);
}
