#include <C64/SID.hpp>
#include <C64/Memory.hpp>

// ---
bool C64::SID::initialize ()
{
	assert (memoryRef () != nullptr);

	// Gets the memory block dedicated to the SID
	if (!(_SIDRegisters = 
		dynamic_cast <C64::SIDRegisters*> (memoryRef () -> subset (C64::Memory::_SIDREGS_SUBSET))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	// TODO

	return (true);
}

// ---
bool C64::SID::simulate (MCHEmul::CPU* cpu)
{
	// TODO
	return (true);
}

// ---
MCHEmul::InfoStructure C64::SID::getInfoStructure () const
{
	MCHEmul::InfoStructure result = MCHEmul::Chip::getInfoStructure ();

	result.remove ("Memory"); // This is not neccesary...
	result.add ("Registers",	_SIDRegisters -> getInfoStructure ());

	return (result);
}
