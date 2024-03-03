#include <C264/C6529B2.hpp>
#include <C264/C6529B2Registers.hpp>

// ---
bool C264::C6529B2::initialize ()
{
	assert (memoryRef () != nullptr);

	// The parent is not initialized because the registers are not the parent ones...

	// Gets the memory block dedicated to this register...
	if (!(_C6529B2Registers = 
		dynamic_cast <C264::C6529B2Registers*> (memoryRef () -> subset (C264::C6529B2Registers::_C6529B2REGS_SUBSET))))
	{
		_error = MCHEmul::_INIT_ERROR;

		return (false);
	}

	_C6529BRegisters = _C6529B2Registers;
	_C6529B2Registers -> initialize ();

	return (true);
}

// ---
bool C264::C6529B2::simulate (MCHEmul::CPU* cpu)
{ 
	// TODO

	return (true); 
}
