#include <FZ80/ALUCTRL.hpp>

// ---
_INST_IMPL (FZ80::DAA)
{
	assert (parameters ().size () == 1);

	// TODO

	return (true);
}

// ---
_INST_IMPL (FZ80::CPL)
{
	assert (parameters ().size () == 1);

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	registerA ().set ({ ~registerA ().values ()[0] });

	// How the flags are affected...
	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, true);  // Always!
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, true); // Always!

	return (true);
}

// ---
_INST_IMPL (FZ80::NEG)
{
	assert (parameters ().size () == 2);

	// TODO

	return (true);
}

// ---
_INST_IMPL (FZ80::CCF)
{
	assert (parameters ().size () == 1);

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();
	bool pC = st.bitStatus (FZ80::CZ80::_CARRYFLAG);

	// How the flags are affected...
	st.setBitStatus (FZ80::CZ80::_CARRYFLAG, !pC);
	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, false);  // Always!
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, pC);

	return (true);
}

// ---
_INST_IMPL (FZ80::SCF)
{
	assert (parameters ().size () == 1);

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// How the flags are affected...
	st.setBitStatus (FZ80::CZ80::_CARRYFLAG, true);
	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, false);  // Always!
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, false); // Always!

	return (true);
}

// ---
_INST_IMPL (FZ80::NOP)
{
	assert (parameters ().size () == 1);

	// Does nothing...

	return (true);
}

// ---
_INST_IMPL (FZ80::HALT)
{
	assert (parameters ().size () == 1);

	// Execute the NOP instruction (does nothing) 
	// until the request for an interruption arrives!
	_FINISH = (cpu () -> interruptRequested () != -1);

	return (true);
}

// ---
_INST_IMPL (FZ80::DI)
{
	assert (parameters ().size () == 1);

	static_cast <FZ80::CZ80*> (cpu ()) -> setIFF1 (false);
	static_cast <FZ80::CZ80*> (cpu ()) -> setIFF2 (false);

	return (true);
}

// ---
_INST_IMPL (FZ80::EI)
{
	assert (parameters ().size () == 1);

	static_cast <FZ80::CZ80*> (cpu ()) -> setIFF1 (true);
	static_cast <FZ80::CZ80*> (cpu ()) -> setIFF2 (true);

	return (true);
}

// ---
_INST_IMPL (FZ80::IM_0)
{
	assert (parameters ().size () == 2);

	static_cast <FZ80::CZ80*> (cpu ()) -> setINTMode (0);

	return (true);
}

// ---
_INST_IMPL (FZ80::IM_1)
{
	assert (parameters ().size () == 2);

	static_cast <FZ80::CZ80*> (cpu ()) -> setINTMode (1);

	return (true);
}

// ---
_INST_IMPL (FZ80::IM_2)
{
	assert (parameters ().size () == 2);

	static_cast <FZ80::CZ80*> (cpu ()) -> setINTMode (2);

	return (true);
}
