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
