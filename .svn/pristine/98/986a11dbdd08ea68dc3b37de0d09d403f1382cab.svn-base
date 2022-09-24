#include <C64/CIA.hpp>
#include <C64/C64.hpp>

// ---
bool C64::CIA1::initialize ()
{
	assert (memoryRef () != nullptr);

	if (!MCHEmul::Chip::initialize ())
		return (false); 

	// Gets the memory block dedicated to the CIA1
	if (!(_CIA1Registers = 
		dynamic_cast <C64::CIA1Registers*> (memoryRef () -> block (C64::Commodore64::_CIA1_MEMORY))))
	{
		_lastError = MCHEmul::_INIT_ERROR;

		return (false);
	}

	return (true);
}

// ---
bool C64::CIA1::simulate (MCHEmul::CPU* cpu)
{
	// TODO

	return (true);
}

// ---
bool C64::CIA2::initialize ()
{
	assert (memoryRef () != nullptr);

	if (!MCHEmul::Chip::initialize ())
		return (false); 

	// Gets the memory block dedicated to the CIA2
	if (!(_CIA2Registers = 
		dynamic_cast <C64::CIA2Registers*> (memoryRef () -> block (C64::Commodore64::_CIA2_MEMORY))))
	{
		_lastError = MCHEmul::_INIT_ERROR;

		return (false);
	}

	return (true);
}

// ---
bool C64::CIA2::simulate (MCHEmul::CPU* cpu)
{
	// TODO

	return (true);
}
