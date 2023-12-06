#include <FZ80/CALL.hpp>

// ---
_INST_IMPL (FZ80::CALL)
{
	assert (parameters ().size () == 3);

	executeBranch ();

	return (true);
}

// ---
_INST_IMPL (FZ80::CALL_NZ)
{
	assert (parameters ().size () == 3);

	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_NEGATIVEFLAG))
		executeBranch ();

	return (true);
}

// ---
_INST_IMPL (FZ80::CALL_Z)
{
	assert (parameters ().size () == 3);

	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_NEGATIVEFLAG))
		executeBranch ();

	return (true);
}

// ---
_INST_IMPL (FZ80::CALL_NC)
{
	assert (parameters ().size () == 3);

	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_CARRYFLAG))
		executeBranch ();

	return (true);
}

// ---
_INST_IMPL (FZ80::CALL_C)
{
	assert (parameters ().size () == 3);

	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_CARRYFLAG))
		executeBranch ();

	return (true);
}

// ---
_INST_IMPL (FZ80::CALL_PO)
{
	assert (parameters ().size () == 3);

	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG))
		executeBranch ();

	return (true);
}

// ---
_INST_IMPL (FZ80::CALL_PE)
{
	assert (parameters ().size () == 3);

	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG))
		executeBranch ();

	return (true);
}

// ---
_INST_IMPL (FZ80::CALL_P)
{
	assert (parameters ().size () == 3);

	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_SIGNFLAG))
		executeBranch ();

	return (true);
}

// ---
_INST_IMPL (FZ80::CALL_M)
{
	assert (parameters ().size () == 3);

	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_SIGNFLAG))
		executeBranch ();

	return (true);
}

// ---
_INST_IMPL (FZ80::RET)
{
	assert (parameters ().size () == 1);

	executeReturn ();

	return (true);
}

// ---
_INST_IMPL (FZ80::RET_NZ)
{
	assert (parameters ().size () == 1);

	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_NEGATIVEFLAG))
		executeReturn ();

	return (true);
}

// ---
_INST_IMPL (FZ80::RET_Z)
{
	assert (parameters ().size () == 1);

	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_NEGATIVEFLAG))
		executeReturn ();

	return (true);
}

// ---
_INST_IMPL (FZ80::RET_NC)
{
	assert (parameters ().size () == 1);

	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_CARRYFLAG))
		executeReturn ();

	return (true);
}

// ---
_INST_IMPL (FZ80::RET_C)
{
	assert (parameters ().size () == 1);

	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_CARRYFLAG))
		executeReturn ();

	return (true);
}

// ---
_INST_IMPL (FZ80::RET_PO)
{
	assert (parameters ().size () == 1);

	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG))
		executeReturn ();

	return (true);
}

// ---
_INST_IMPL (FZ80::RET_PE)
{
	assert (parameters ().size () == 1);

	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG))
		executeReturn ();

	return (true);
}

// ---
_INST_IMPL (FZ80::RET_P)
{
	assert (parameters ().size () == 1);

	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_SIGNFLAG))
		executeReturn ();

	return (true);
}

// ---
_INST_IMPL (FZ80::RET_M)
{
	assert (parameters ().size () == 1);

	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_SIGNFLAG))
		executeReturn ();

	return (true);
}
