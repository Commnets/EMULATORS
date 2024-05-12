#include <FZ80/CALL.hpp>
#include <FZ80/NMIInterrupt.hpp>

// ---
_INST_IMPL (FZ80::CALL)
{
	assert (parameters ().size () == 3);

	executeBranch (false);

	return (true);
}

// ---
_INST_IMPL (FZ80::CALL_NZ)
{
	assert (parameters ().size () == 3);

	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_ZEROFLAG))
		executeBranch (true);

	return (true);
}

// ---
_INST_IMPL (FZ80::CALL_Z)
{
	assert (parameters ().size () == 3);

	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_ZEROFLAG))
		executeBranch (true);

	return (true);
}

// ---
_INST_IMPL (FZ80::CALL_NC)
{
	assert (parameters ().size () == 3);

	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_CARRYFLAG))
		executeBranch (true);

	return (true);
}

// ---
_INST_IMPL (FZ80::CALL_C)
{
	assert (parameters ().size () == 3);

	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_CARRYFLAG))
		executeBranch (true);

	return (true);
}

// ---
_INST_IMPL (FZ80::CALL_PO)
{
	assert (parameters ().size () == 3);

	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG))
		executeBranch (true);

	return (true);
}

// ---
_INST_IMPL (FZ80::CALL_PE)
{
	assert (parameters ().size () == 3);

	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG))
		executeBranch (true);

	return (true);
}

// ---
_INST_IMPL (FZ80::CALL_P)
{
	assert (parameters ().size () == 3);

	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_SIGNFLAG))
		executeBranch (true);

	return (true);
}

// ---
_INST_IMPL (FZ80::CALL_M)
{
	assert (parameters ().size () == 3);

	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_SIGNFLAG))
		executeBranch (true);

	return (true);
}

// ---
_INST_IMPL (FZ80::RST_00)
{
	assert (parameters ().size () == 1);

	executeBranch (0);

	return (true);
}

// ---
_INST_IMPL (FZ80::RST_08)
{
	assert (parameters ().size () == 1);

	executeBranch (1);

	return (true);
}

// ---
_INST_IMPL (FZ80::RST_10)
{
	assert (parameters ().size () == 1);

	executeBranch (2);

	return (true);
}

// ---
_INST_IMPL (FZ80::RST_18)
{
	assert (parameters ().size () == 1);

	executeBranch (3);

	return (true);
}

// ---
_INST_IMPL (FZ80::RST_20)
{
	assert (parameters ().size () == 1);

	executeBranch (4);

	return (true);
}

// ---
_INST_IMPL (FZ80::RST_28)
{
	assert (parameters ().size () == 1);

	executeBranch (5);

	return (true);
}

// ---
_INST_IMPL (FZ80::RST_30)
{
	assert (parameters ().size () == 1);

	executeBranch (6);

	return (true);
}

// ---
_INST_IMPL (FZ80::RST_38)
{
	assert (parameters ().size () == 1);

	executeBranch (7);

	return (true);
}

// ---
_INST_IMPL (FZ80::RET)
{
	assert (parameters ().size () == 1);

	executeReturn (false);

	return (true);
}

// ---
_INST_IMPL (FZ80::RET_NZ)
{
	assert (parameters ().size () == 1);

	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_ZEROFLAG))
		executeReturn (true);

	return (true);
}

// ---
_INST_IMPL (FZ80::RET_Z)
{
	assert (parameters ().size () == 1);

	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_ZEROFLAG))
		executeReturn (true);

	return (true);
}

// ---
_INST_IMPL (FZ80::RET_NC)
{
	assert (parameters ().size () == 1);

	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_CARRYFLAG))
		executeReturn (true);

	return (true);
}

// ---
_INST_IMPL (FZ80::RET_C)
{
	assert (parameters ().size () == 1);

	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_CARRYFLAG))
		executeReturn (true);

	return (true);
}

// ---
_INST_IMPL (FZ80::RET_PO)
{
	assert (parameters ().size () == 1);

	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG))
		executeReturn (true);

	return (true);
}

// ---
_INST_IMPL (FZ80::RET_PE)
{
	assert (parameters ().size () == 1);

	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG))
		executeReturn (true);

	return (true);
}

// ---
_INST_IMPL (FZ80::RET_P)
{
	assert (parameters ().size () == 1);

	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_SIGNFLAG))
		executeReturn (true);

	return (true);
}

// ---
_INST_IMPL (FZ80::RET_M)
{
	assert (parameters ().size () == 1);

	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_SIGNFLAG))
		executeReturn (true);

	return (true);
}

// ---
_INST_IMPL (FZ80::RET_I)
{
	assert (parameters ().size () == 2);

	// Undocumented!
	static_cast <FZ80::CZ80*> (cpu ()) -> setIFF1 (static_cast <FZ80::CZ80*> (cpu ()) -> IFF2 ());

	cpu () -> interrupt (FZ80::INTInterrupt::_ID) -> setInExecution (false);

	executeReturn (false);

	return (true);
}

// ---
_INST_IMPL (FZ80::RET_N)
{
	assert (parameters ().size () == 2);

	static_cast <FZ80::CZ80*> (cpu ()) -> setIFF1 (static_cast <FZ80::CZ80*> (cpu ()) -> IFF2 ());

	cpu () -> interrupt (FZ80::NMIInterrupt::_ID) -> setInExecution (false);

	executeReturn (false);

	return (true);
}
