#include <VIC20/VIA1.hpp>
#include <VIC20/Memory.hpp>

// ---
bool VIC20::VIA1::initialize ()
{
	assert (memoryRef () != nullptr);

	// Gets the memory block dedicated to the CIA2
	if (!(_VIA1Registers = 
		dynamic_cast <VIC20::VIA1Registers*> (memoryRef () -> subset (_registersId))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	return (COMMODORE::VIA::initialize ());
}
