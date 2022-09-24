#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::BRK)
{
	assert (parameters ().size () == 1);

	MCHEmul::ProgramCounter& pc = cpu () -> programCounter ();
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	stack () -> push (pc.asAddress ().bytes ());
	stack () -> push (st.values ());

	st.setBitStatus ("I", true); // No more interruptions so far...
	st.setBitStatus ("B", true);

	// Jump to the IRQ vector...
	pc.setAddress (MCHEmul::Address (memory () -> values 
		(dynamic_cast <F6500::C6510*> (cpu ()) -> IRQVectorAddress (), 2), false /** Little - endian. */));

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
	// The stack stores always high byte first. So no changes are needed...
	// Different story in RTI or RTS!
	stack () -> push (pc.asAddress ().values ());

	pc.setAddress (address_absolute ());

	return (true);
}

// ---
_INST_IMPL (F6500::RTI)
{
	assert (parameters ().size () == 1);

	// When a interruption is lunched, the status and the program counter is stored,
	// so it has to be recovered just in the other way around...
	cpu () -> programCounter ().setAddress 
		(MCHEmul::Address (stack () -> pull (2), false /** Are recover in Little-endian format */));

	// See the part of the logic where the interruptions are managed, 
	// to see how status register is also saved!
	cpu () -> statusRegister ().set (stack () -> pull (1)); 

	return (true);
}

// ---
_INST_IMPL (F6500::RTS)
{
	assert (parameters ().size () == 1);

	cpu () -> programCounter ().setAddress 
		(MCHEmul::Address (stack () -> pull (2), false /** Are recover in Little-endian format */));

	return (true);
}
