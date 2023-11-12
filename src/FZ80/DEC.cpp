#include <FZ80/Instructions.hpp>

// ---
bool FZ80::DEC_General::executeWith (MCHEmul::Register& r)
{
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// The operation...
	MCHEmul::UInt v  (r.values ()[0]);
	MCHEmul::UInt hv (r.values ()[0] & 0x0f); // Just half byte!
	bool i80 = (v == MCHEmul::UInt (MCHEmul::UByte (0x80)));
	v  -= MCHEmul::UInt::_1; // Increment...
	hv -= MCHEmul::UInt::_1; // ..just to see whether there is hafl carry!
	r.set (v.bytes ()); // Put back the info

	// How the flags are affected...
	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, true);
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, i80); // Only if it was 0x80 before the operation...
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, hv [0].bit (4)); // If true, there will have been half carry!
	st.setBitStatus (FZ80::CZ80::_ZEROFLAG, v == MCHEmul::UByte::_0);
	st.setBitStatus (FZ80::CZ80::_SIGNFLAG, v.negative ());

	return (true);
}

// ---
bool FZ80::DEC_General::executeWith (const MCHEmul::Address& a)
{
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// The operation...
	MCHEmul::UInt v (memory () -> value (a));
	bool i7F = (v == MCHEmul::UInt (MCHEmul::UByte (0x7f)));
	v -= 1; // DECrement...
	memory () -> set (a, v.values ()[0]);

	// How the flags are affected...
	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, false);
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, i7F); // Only if it was 7F before the operation...
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, true);
	st.setBitStatus (FZ80::CZ80::_ZEROFLAG, v == MCHEmul::UByte::_0);
	st.setBitStatus (FZ80::CZ80::_SIGNFLAG, v.negative ());

	return (true);
}

// ---
_INST_IMPL (FZ80::DEC_A)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA ()));
}

// ---
_INST_IMPL (FZ80::DEC_B)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerB ()));
}

// ---
_INST_IMPL (FZ80::DEC_C)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerC ()));
}

// ---
_INST_IMPL (FZ80::DEC_D)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerD ()));
}

// ---
_INST_IMPL (FZ80::DEC_E)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerE ()));
}

// ---
_INST_IMPL (FZ80::DEC_F)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerF ()));
}

// ---
_INST_IMPL (FZ80::DEC_L)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerL ()));
}

// ---
_INST_IMPL (FZ80::DEC_IndirectHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (addressHL ()));
}

// ---
_INST_IMPL (FZ80::DEC_IndirectIndexIX)
{
	assert (parameters ().size () == 3);

	return (executeWith (addressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::DEC_IndirectIndexIY)
{
	assert (parameters ().size () == 3);

	return (executeWith (addressIY (parameters ()[2].value ())));
}
