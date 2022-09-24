#include <C64/CIA.hpp>
#include <C64/Memory.hpp>
#include <C64/VICII.hpp>

// ---
bool C64::CIA1::initialize ()
{
	assert (memoryRef () != nullptr);

	// Gets the memory block dedicated to the CIA1
	if (!(_CIA1Registers = 
		dynamic_cast <C64::CIA1Registers*> (memoryRef () -> subset (C64::Memory::_CIA1_SUBSET))))
	{
		_lastError = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_CIA1Registers -> initialize ();

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

	// Gets the memory block dedicated to the CIA2
	if (!(_CIA2Registers = 
		dynamic_cast <C64::CIA2Registers*> (memoryRef () -> subset (C64::Memory::_CIA2_SUBSET))))
	{
		_lastError = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_CIA2Registers -> initialize ();

	return (true);
}

// ---
bool C64::CIA2::simulate (MCHEmul::CPU* cpu)
{
	_VICIIRef -> setBank (_CIA2Registers -> VICIIBank ());

	return (true);
}
