#include <VIC20/VIA2.hpp>
#include <VIC20/Memory.hpp>

// ---
bool VIC20::VIA2::initialize ()
{
	assert (memoryRef () != nullptr);

	// Gets the memory block dedicated to the CIA2
	if (!(_VIA2Registers = 
		dynamic_cast <VIC20::VIA2Registers*> (memoryRef () -> subset (_registersId))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	return (COMMODORE::VIA::initialize ());
}
