#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
void F6500::BXX_General::executeBranch ()
{
	int jR = MCHEmul::UInt ({ value_relative () }).asInt (); // The value can be negative meaning back jump!

	if (jR == 0)
		return; // No need to continue...

	MCHEmul::ProgramCounter& pc = cpu () -> programCounter ();
	if (jR > 0) pc.increment ((size_t) jR);
	else pc.decrement ((size_t) -jR); // Parameter to "decrement" always positive...
}

// ---
_INST_IMPL (F6500::BCC)
{
	if (!cpu () -> statusRegister ().bitStatus ("C"))
		executeBranch ();
	return (true);
}

// ---
_INST_IMPL (F6500::BCS)
{
	if (cpu () -> statusRegister ().bitStatus ("C"))
		executeBranch ();
	return (true);
}

// ---
_INST_IMPL (F6500::BEQ)
{
	if (cpu () -> statusRegister ().bitStatus ("Z"))
		executeBranch ();
	return (true);
}

// ---
_INST_IMPL (F6500::BMI)
{
	if (cpu () -> statusRegister ().bitStatus ("N"))
		executeBranch ();
	return (true);
}

// ---
_INST_IMPL (F6500::BNE)
{
	if (!cpu () -> statusRegister ().bitStatus ("Z"))
		executeBranch ();
	return (true);
}

// ---
_INST_IMPL (F6500::BPL)
{
	if (!cpu () -> statusRegister ().bitStatus ("N"))
		executeBranch ();
	return (true);
}

// ---
_INST_IMPL (F6500::BVC)
{
	if (!cpu () -> statusRegister ().bitStatus ("V"))
		executeBranch ();
	return (true);
}

// ---
_INST_IMPL (F6500::BVS)
{
	if (cpu () -> statusRegister ().bitStatus ("V"))
		executeBranch ();
	return (true);
}
