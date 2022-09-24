#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>
#include <core/CPU.hpp>
#include <core/Stack.hpp>

_INST_IMPL (F6500::BRK)
{
	assert (parameters ().size () == 1);

	// The treatement of the interruption is managed at the main body...
	cpu () -> statusRegister ().setBitStatus ("B", true);

	return (true);
}

// ---
_INST_IMPL (F6500::JMP_Absolute)
{
	cpu () -> programCounter ().setAddress (address_absolute ());

	return (true);
}

// ---
_INST_IMPL (F6500::JMP_Indirect)
{
	cpu () -> programCounter ().setAddress (address_indirect ());

	return (true);
}

// ---
_INST_IMPL (F6500::JSR_Absolute)
{
	assert (parameters ().size () == 3);

	MCHEmul::ProgramCounter& pc = cpu () -> programCounter ();

	// The address always is kept in Big-endian format, 
	// but the architecture of this CPU is Little-endian.
	// This is why bytes are reversed before being stored into the stack...
	stack () -> push (pc.asAddress ().value ().bytes ().reverse ());

	pc.setAddress (address_absolute ());

	return (true);
}

// ---
_INST_IMPL (F6500::RTI)
{
	assert (parameters ().size () == 1);

	// When a interruption is lunched, the status and the program counter is stored,
	// so it has to be recovered just in the other way around...
	cpu () -> programCounter ().setAddress (MCHEmul::Address (stack () -> pull (2), false));

	cpu () -> statusRegister ().set (stack () -> pull (1)); 

	return (true);
}

// ---
_INST_IMPL (F6500::RTS)
{
	assert (parameters ().size () == 1);

	cpu () -> programCounter ().setAddress (MCHEmul::Address (stack () -> pull (2), false));

	return (true);
}
