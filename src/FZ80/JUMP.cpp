#include <FZ80/JUMP.hpp>

// ---
_INST_IMPL (FZ80::JP)
{
	assert (parameters ().size () == 3);

	cpu () -> programCounter ().
		setAddress (MCHEmul::Address ({ parameters ()[2].value (), parameters ()[1].value () }, true)); 
	// Little endian, but already done in the way the parameters are moved...
	// So true instead false (this is quicker!)

	return (true);
}

// ---
_INST_IMPL (FZ80::JP_NZ)
{
	assert (parameters ().size () == 3);

	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_NEGATIVEFLAG))
		cpu () -> programCounter ().
			setAddress (MCHEmul::Address ({ parameters ()[2].value (), parameters ()[1].value () }, true)); 

	return (true);
}

// ---
_INST_IMPL (FZ80::JP_Z)
{
	assert (parameters ().size () == 3);

	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_NEGATIVEFLAG))
		cpu () -> programCounter ().
			setAddress (MCHEmul::Address ({ parameters ()[2].value (), parameters ()[1].value () }, true)); 

	return (true);
}

// ---
_INST_IMPL (FZ80::JP_NC)
{
	assert (parameters ().size () == 3);

	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_CARRYFLAG))
		cpu () -> programCounter ().
			setAddress (MCHEmul::Address ({ parameters ()[2].value (), parameters ()[1].value () }, true)); 

	return (true);
}

// ---
_INST_IMPL (FZ80::JP_C)
{
	assert (parameters ().size () == 3);

	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_CARRYFLAG))
		cpu () -> programCounter ().
			setAddress (MCHEmul::Address ({ parameters ()[2].value (), parameters ()[1].value () }, true)); 

	return (true);
}

// ---
_INST_IMPL (FZ80::JP_PO)
{
	assert (parameters ().size () == 3);

	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG))
		cpu () -> programCounter ().
			setAddress (MCHEmul::Address ({ parameters ()[2].value (), parameters ()[1].value () }, true)); 

	return (true);
}

// ---
_INST_IMPL (FZ80::JP_PE)
{
	assert (parameters ().size () == 3);

	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG))
		cpu () -> programCounter ().
			setAddress (MCHEmul::Address ({ parameters ()[2].value (), parameters ()[1].value () }, true)); 

	return (true);
}

// ---
_INST_IMPL (FZ80::JP_P)
{
	assert (parameters ().size () == 3);

	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_SIGNFLAG))
		cpu () -> programCounter ().
			setAddress (MCHEmul::Address ({ parameters ()[2].value (), parameters ()[1].value () }, true)); 

	return (true);
}

// ---
_INST_IMPL (FZ80::JP_M)
{
	assert (parameters ().size () == 3);

	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_SIGNFLAG))
		cpu () -> programCounter ().
			setAddress (MCHEmul::Address ({ parameters ()[2].value (), parameters ()[1].value () }, true)); 

	return (true);
}

// ---
_INST_IMPL (FZ80::JP_IndirectHL)
{
	assert (parameters ().size () == 1);

	cpu () -> programCounter ().setAddress (addressHL ());

	return (true);
}

// ---
_INST_IMPL (FZ80::JP_IndirectIX)
{
	assert (parameters ().size () == 2);

	cpu () -> programCounter ().setAddress (addressIX ());

	return (true);
}

// ---
_INST_IMPL (FZ80::JP_IndirectIY)
{
	assert (parameters ().size () == 2);

	cpu () -> programCounter ().setAddress (addressIY ());

	return (true);
}

// ---
_INST_IMPL (FZ80::JR)
{
	assert (parameters ().size () == 2);

	executeBranch (false);

	return (true);
}

// ---
_INST_IMPL (FZ80::JR_NZ)
{
	assert (parameters ().size () == 2);

	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_ZEROFLAG))
		executeBranch (true);

	return (true);
}

// ---
_INST_IMPL (FZ80::JR_Z)
{
	assert (parameters ().size () == 2);

	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_ZEROFLAG))
		executeBranch (true);

	return (true);
}

// ---
_INST_IMPL (FZ80::JR_NC)
{
	assert (parameters ().size () == 2);

	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_CARRYFLAG))
		executeBranch (true);

	return (true);
}

// ---
_INST_IMPL (FZ80::JR_C)
{
	assert (parameters ().size () == 2);

	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_CARRYFLAG))
		executeBranch (true);

	return (true);
}

// ---
_INST_IMPL (FZ80::DJNZ)
{
	assert (parameters ().size () == 2);

	unsigned char r = registerB ().values ()[0].value ();
	bool c = ((r -= 1) == MCHEmul::UByte::_0);
	registerB ().set ({ r }); // decrement 1 in the register B...
	if (!c)
		executeBranch (true);

	return (true);
}
