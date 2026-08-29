#include <F6500/Instructions.hpp>
#include <F6500/C6510.hpp>

// ---
unsigned int F6500::BCC::clockCyclesToExecute (
	MCHEmul::CPU* c, MCHEmul::Memory* m, const MCHEmul::Address& a) const
{
	return (clockCyclesForBranch (m, a,
		!c -> statusRegister ().bitStatus (F6500::C6500::_CARRYFLAG)));
}

// ---
_INST_IMPL (F6500::BCC)
{
	if (!cpu () -> statusRegister ().bitStatus (F6500::C6500::_CARRYFLAG))
		executeBranch ();
	return (true);
}

// ---
unsigned int F6500::BCS::clockCyclesToExecute (
	MCHEmul::CPU* c, MCHEmul::Memory* m, const MCHEmul::Address& a) const
{
	return (clockCyclesForBranch (m, a,
		c -> statusRegister ().bitStatus (F6500::C6500::_CARRYFLAG)));
}

// ---
_INST_IMPL (F6500::BCS)
{
	if (cpu () -> statusRegister ().bitStatus (F6500::C6500::_CARRYFLAG))
		executeBranch ();
	return (true);
}

// ---
unsigned int F6500::BEQ::clockCyclesToExecute (
	MCHEmul::CPU* c, MCHEmul::Memory* m, const MCHEmul::Address& a) const
{
	return (clockCyclesForBranch (m, a,
		c -> statusRegister ().bitStatus (F6500::C6500::_ZEROFLAG)));
}

// ---
_INST_IMPL (F6500::BEQ)
{
	if (cpu () -> statusRegister ().bitStatus (F6500::C6500::_ZEROFLAG))
		executeBranch ();
	return (true);
}

// ---
unsigned int F6500::BMI::clockCyclesToExecute (
	MCHEmul::CPU* c, MCHEmul::Memory* m, const MCHEmul::Address& a) const
{
	return (clockCyclesForBranch (m, a,
		c -> statusRegister ().bitStatus (F6500::C6500::_NEGATIVEFLAG)));
}

// ---
_INST_IMPL (F6500::BMI)
{
	if (cpu () -> statusRegister ().bitStatus (F6500::C6500::_NEGATIVEFLAG))
		executeBranch ();
	return (true);
}

// ---
unsigned int F6500::BNE::clockCyclesToExecute (
	MCHEmul::CPU* c, MCHEmul::Memory* m, const MCHEmul::Address& a) const
{
	return (clockCyclesForBranch (m, a,
		!c -> statusRegister ().bitStatus (F6500::C6500::_ZEROFLAG)));
}

// ---
_INST_IMPL (F6500::BNE)
{
	if (!cpu () -> statusRegister ().bitStatus (F6500::C6500::_ZEROFLAG))
		executeBranch ();
	return (true);
}

// ---
unsigned int F6500::BPL::clockCyclesToExecute (
	MCHEmul::CPU* c, MCHEmul::Memory* m, const MCHEmul::Address& a) const
{
	return (clockCyclesForBranch (m, a,
		!c -> statusRegister ().bitStatus (F6500::C6500::_NEGATIVEFLAG)));
}

// ---
_INST_IMPL (F6500::BPL)
{
	if (!cpu () -> statusRegister ().bitStatus (F6500::C6500::_NEGATIVEFLAG))
		executeBranch ();
	return (true);
}

// ---
unsigned int F6500::BVC::clockCyclesToExecute (
	MCHEmul::CPU* c, MCHEmul::Memory* m, const MCHEmul::Address& a) const
{
	return (clockCyclesForBranch (m, a,
		!c -> statusRegister ().bitStatus (F6500::C6500::_OVERFLOWFLAG)));
}

// ---
_INST_IMPL (F6500::BVC)
{
	if (!cpu () -> statusRegister ().bitStatus (F6500::C6500::_OVERFLOWFLAG))
		executeBranch ();
	return (true);
}

// ---
unsigned int F6500::BVS::clockCyclesToExecute (
	MCHEmul::CPU* c, MCHEmul::Memory* m, const MCHEmul::Address& a) const
{
	return (clockCyclesForBranch (m, a,
		c -> statusRegister ().bitStatus (F6500::C6500::_OVERFLOWFLAG)));
}

// ---
_INST_IMPL (F6500::BVS)
{
	if (cpu () -> statusRegister ().bitStatus (F6500::C6500::_OVERFLOWFLAG))
		executeBranch ();
	return (true);
}
