#include <C64/CIA1.hpp>
#include <C64/Memory.hpp>

// ---
bool C64::CIA1::initialize ()
{
	assert (memoryRef () != nullptr);

	// Gets the memory block dedicated to the CIA1
	// That has special uses with the keyboard and joysticks...
	if (!(_CIA1Registers = 
		dynamic_cast <C64::CIA1Registers*> (memoryRef () -> subset (C64::CIA1Registers::_CIA1_SUBSET))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	return (C64::CIA::initialize ());
}
