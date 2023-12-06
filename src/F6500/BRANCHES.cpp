#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
_INST_IMPL (F6500::BCC)
{
	if (!cpu () -> statusRegister ().bitStatus (F6500::C6500::_CARRYFLAG))
		executeBranch ();
	return (true);
}

// ---
_INST_IMPL (F6500::BCS)
{
	if (cpu () -> statusRegister ().bitStatus (F6500::C6500::_CARRYFLAG))
		executeBranch ();
	return (true);
}

// ---
_INST_IMPL (F6500::BEQ)
{
	if (cpu () -> statusRegister ().bitStatus (F6500::C6500::_ZEROFLAG))
		executeBranch ();
	return (true);
}

// ---
_INST_IMPL (F6500::BMI)
{
	if (cpu () -> statusRegister ().bitStatus (F6500::C6500::_NEGATIVEFLAG))
		executeBranch ();
	return (true);
}

// ---
_INST_IMPL (F6500::BNE)
{
	if (!cpu () -> statusRegister ().bitStatus (F6500::C6500::_ZEROFLAG))
		executeBranch ();
	return (true);
}

// ---
_INST_IMPL (F6500::BPL)
{
	if (!cpu () -> statusRegister ().bitStatus (F6500::C6500::_NEGATIVEFLAG))
		executeBranch ();
	return (true);
}

// ---
_INST_IMPL (F6500::BVC)
{
	if (!cpu () -> statusRegister ().bitStatus (F6500::C6500::_OVERFLOWFLAG))
		executeBranch ();
	return (true);
}

// ---
_INST_IMPL (F6500::BVS)
{
	if (cpu () -> statusRegister ().bitStatus (F6500::C6500::_OVERFLOWFLAG))
		executeBranch ();
	return (true);
}
