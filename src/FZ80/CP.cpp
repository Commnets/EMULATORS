#include <FZ80/Instructions.hpp>

// ---
bool FZ80::CP_General::executeWith (const MCHEmul::UByte& v)
{
	MCHEmul::Register& r = registerA ();
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// The operation...
	MCHEmul::UInt rst  = MCHEmul::UInt (r.values ()[0]) - MCHEmul::UInt (v);
	MCHEmul::UInt rstH = MCHEmul::UInt (r.values ()[0] & 0x0f) - MCHEmul::UInt (v & 0x0f); // Just to calculate the half borrow!

	// How the flags are affected...
	st.setBitStatus (FZ80::CZ80::_CARRYFLAG, rst.carry ());
	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, true); // Always!
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, rst.overflow ());
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, rstH [0].bit (4)); // When true, there will have been a half borrow!
	st.setBitStatus (FZ80::CZ80::_ZEROFLAG, rst == MCHEmul::UInt::_0);
	st.setBitStatus (FZ80::CZ80::_SIGNFLAG, rst.negative ());

	return (true);
}

// ---
_INST_IMPL (FZ80::CP_WithA)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerA ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::CP_WithB)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerB ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::CP_WithC)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerC ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::CP_WithD)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerD ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::CP_WithE)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerE ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::CP_WithH)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::CP_WithL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerL ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::CP_WithIndirectHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (valueAddressHL ()));
}

// ---
_INST_IMPL (FZ80::CP_WithIndirectIndexIX)
{
	assert (parameters ().size () == 3);

	return (executeWith (valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::CP_WithIndirectIndexIY)
{
	assert (parameters ().size () == 3);

	return (executeWith (valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::CP)
{
	assert (parameters ().size () == 2);

	return (executeWith (parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::CP_WithIXH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::CP_WithIXL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXL ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::CP_WithIYH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::CP_WithIYL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYL ().values ()[0]));
}
