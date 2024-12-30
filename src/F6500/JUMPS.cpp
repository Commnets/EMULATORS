#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>
#include <F6500/IRQInterrupt.hpp>
#include <F6500/NMIInterrupt.hpp>

// ---
_INST_IMPL (F6500::BRK)
{
	assert (parameters ().size () == 1);

	// It is like IRQ...
	MCHEmul::ProgramCounter& pc = cpu () -> programCounter ();
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	// The BRK will emulate a IRQ situation by code (could be useful to debug code)
	// That IRQ will finish with a RTI, so the program counter will come back to 2 positions more that the original one
	// Because the instruction only occupies a single byte, the program counter saved is one position more...
	stack () -> push (pc.asAddress ().next (1).values ());
	// The status register is saved with the BREAK flag set...
	MCHEmul::UByte stV = st.values ()[0];
	stV.setBit (F6500::C6500::_BREAKFLAG, true);
	stack () -> push (MCHEmul::UBytes ({ stV }) /** Only 1 byte long. */);
	st.setBitStatus (F6500::C6500::_IRQFLAG, true);

	// Jump to the IRQ vector...
	pc.setAddress (MCHEmul::Address (memory () -> values 
		(static_cast <F6500::C6500*> (cpu ()) -> IRQVectorAddress (), 2), false /** Little - endian. */));

	return (!stack () -> overflow ());
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
	// Take into account that what is kept into the stack is not the next direction 
	// that is BTW what PC keeps at this point, but that positiom -1. In RTS will be reverted!
	stack () -> push (pc.asAddress ().previous (1).values ());

	pc.setAddress (address_absolute ());

	return (!stack () -> overflow ());
}

// ---
_INST_IMPL (F6500::RTI)
{
	assert (parameters ().size () == 1);

	// See the part of the logic where the interruptions are managed, 
	// to see how status register is also saved!
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	st.set (stack () -> pull (1));
	// The bit 5 must always be set in this implementation...
	st.setBitStatus (5, true);
	// ...and the bit 4 is psuhed back to false
	st.setBitStatus (F6500::C6500::_BREAKFLAG, false);

	// The interruption is no longer under execution but...which one?
	// Just in case both are set off...
	if (cpu () -> deepDebugActive ())
		cpu () -> deepDebugFile () -> writeSimpleLine ("->Interrupt code about to finish:");
	cpu () -> interrupt (F6500::IRQInterrupt::_ID) -> setInExecution (false);
	if (cpu () -> existsInterrupt (F6500::NMIInterrupt::_ID)) // Some versions of the 6500 doesn't have this type of interrupt...
		cpu () -> interrupt (F6500::NMIInterrupt::_ID) -> setInExecution (false);

	// When a interruption is lunched, the status and the program counter is stored,
	// so it has to be recovered just in the other way around...
	cpu () -> programCounter ().setAddress 
		(MCHEmul::Address (stack () -> pull (2), false /** Are recover in Little-endian format */));

	return (!stack () -> overflow ());
}

// ---
_INST_IMPL (F6500::RTS)
{
	assert (parameters ().size () == 1);

	// The position extracted from the stack has to be incremented in 1 
	// to get the real next execution position!
	cpu () -> programCounter ().setAddress 
		(MCHEmul::Address (stack () -> pull (2), false /** Are recover in Little-endian format */).next (1));

	return (!stack () -> overflow ());
}
