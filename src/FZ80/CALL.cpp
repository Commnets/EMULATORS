#include <FZ80/CALL.hpp>
#include <FZ80/NMIInterrupt.hpp>

// ---
_INST_IMPL (FZ80::CALL)
{
	assert (parameters ().size () == 3);

	return (executeBranch (false));
}

// ---
_INST_IMPL (FZ80::CALL_NZ)
{
	assert (parameters ().size () == 3);

	bool result = true;
	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_ZEROFLAG))
		result = executeBranch (true);

	return (result);
}

// ---
_INST_IMPL (FZ80::CALL_Z)
{
	assert (parameters ().size () == 3);

	bool result = true;
	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_ZEROFLAG))
		result = executeBranch (true);

	return (result);
}

// ---
_INST_IMPL (FZ80::CALL_NC)
{
	assert (parameters ().size () == 3);

	bool result = true;
	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_CARRYFLAG))
		result = executeBranch (true);

	return (result);
}

// ---
_INST_IMPL (FZ80::CALL_C)
{
	assert (parameters ().size () == 3);

	bool result = true;
	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_CARRYFLAG))
		result = executeBranch (true);

	return (result);
}

// ---
_INST_IMPL (FZ80::CALL_PO)
{
	assert (parameters ().size () == 3);

	bool result = true;
	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG))
		result = executeBranch (true);

	return (result);
}

// ---
_INST_IMPL (FZ80::CALL_PE)
{
	assert (parameters ().size () == 3);

	bool result = true;
	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG))
		result = executeBranch (true);

	return (result);
}

// ---
_INST_IMPL (FZ80::CALL_P)
{
	assert (parameters ().size () == 3);

	bool result = true;
	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_SIGNFLAG))
		result = executeBranch (true);

	return (result);
}

// ---
_INST_IMPL (FZ80::CALL_M)
{
	assert (parameters ().size () == 3);

	bool result = true;
	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_SIGNFLAG))
		result = executeBranch (true);

	return (result);
}

// ---
_INST_IMPL (FZ80::RST_00)
{
	assert (parameters ().size () == 1);

	return (executeBranch (0));
}

// ---
_INST_IMPL (FZ80::RST_08)
{
	assert (parameters ().size () == 1);

	return (executeBranch (1));
}

// ---
_INST_IMPL (FZ80::RST_10)
{
	assert (parameters ().size () == 1);

	return (executeBranch (2));
}

// ---
_INST_IMPL (FZ80::RST_18)
{
	assert (parameters ().size () == 1);

	return (executeBranch (3));
}

// ---
_INST_IMPL (FZ80::RST_20)
{
	assert (parameters ().size () == 1);

	return (executeBranch (4));
}

// ---
_INST_IMPL (FZ80::RST_28)
{
	assert (parameters ().size () == 1);

	return (executeBranch (5));
}

// ---
_INST_IMPL (FZ80::RST_30)
{
	assert (parameters ().size () == 1);

	return (executeBranch (6));
}

// ---
_INST_IMPL (FZ80::RST_38)
{
	assert (parameters ().size () == 1);

	return (executeBranch (7));
}

// ---
_INST_IMPL (FZ80::RET)
{
	assert (parameters ().size () == 1);

	return (executeReturn (false));
}

// ---
_INST_IMPL (FZ80::RET_NZ)
{
	assert (parameters ().size () == 1);

	bool result = true;
	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_ZEROFLAG))
		result = executeReturn (true);

	return (result);
}

// ---
_INST_IMPL (FZ80::RET_Z)
{
	assert (parameters ().size () == 1);

	bool result = true;
	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_ZEROFLAG))
		result = executeReturn (true);

	return (result);
}

// ---
_INST_IMPL (FZ80::RET_NC)
{
	assert (parameters ().size () == 1);

	bool result = true;
	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_CARRYFLAG))
		result = executeReturn (true);

	return (result);
}

// ---
_INST_IMPL (FZ80::RET_C)
{
	assert (parameters ().size () == 1);

	bool result = true;
	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_CARRYFLAG))
		result = executeReturn (true);

	return (result);
}

// ---
_INST_IMPL (FZ80::RET_PO)
{
	assert (parameters ().size () == 1);

	bool result = true;
	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG))
		result = executeReturn (true);

	return (result);
}

// ---
_INST_IMPL (FZ80::RET_PE)
{
	assert (parameters ().size () == 1);

	bool result = true;
	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG))
		result = executeReturn (true);

	return (result);
}

// ---
_INST_IMPL (FZ80::RET_P)
{
	assert (parameters ().size () == 1);

	bool result = true;
	if (!cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_SIGNFLAG))
		result = executeReturn (true);

	return (result);
}

// ---
_INST_IMPL (FZ80::RET_M)
{
	assert (parameters ().size () == 1);

	bool result = true;
	if (cpu () -> statusRegister ().bitStatus (FZ80::CZ80::_SIGNFLAG))
		result = executeReturn (true);

	return (result);
}

// ---
_INST_IMPL (FZ80::RET_I)
{
	assert (parameters ().size () == 2);

	// Undocumented!
	static_cast <FZ80::CZ80*> (cpu ()) -> setIFF1 (static_cast <FZ80::CZ80*> (cpu ()) -> IFF2 ());

	if (cpu () -> deepDebugActive ())
		*cpu () -> deepDebugFile () << "<-Interrupt code about to finish\n";
	cpu () -> interrupt (FZ80::INTInterrupt::_ID) -> setInExecution (false);

	return (executeReturn (false));
}

// ---
_INST_IMPL (FZ80::RET_N)
{
	assert (parameters ().size () == 2);

	if (cpu () -> deepDebugActive ())
		*cpu () -> deepDebugFile () << "<-Interrupt code about to finish\n";
	static_cast <FZ80::CZ80*> (cpu ()) -> setIFF1 (static_cast <FZ80::CZ80*> (cpu ()) -> IFF2 ());

	cpu () -> interrupt (FZ80::NMIInterrupt::_ID) -> setInExecution (false);

	return (executeReturn (false));
}
