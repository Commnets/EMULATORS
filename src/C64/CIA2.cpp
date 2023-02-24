#include <C64/CIA2.hpp>
#include <C64/Memory.hpp>

// ---
bool C64::CIA2::initialize ()
{
	assert (memoryRef () != nullptr);

	// Gets the memory block dedicated to the CIA2
	if (!(_CIA2Registers = 
		dynamic_cast <C64::CIA2Registers*> (memoryRef () -> subset (C64::CIA2Registers::_CIA2_SUBSET))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	return (COMMODORE::CIA::initialize ());
}

// ---
bool C64::CIA2::simulate (MCHEmul::CPU* cpu)
{
	if (_CIA2Registers -> changesAtPositions ())
		notify (COMMODORE::VICII::_BANK0SET + (unsigned int) _CIA2Registers -> VICIIBank ());

	return (COMMODORE::CIA::simulate (cpu));
}
