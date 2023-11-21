#include <FZ80/Instructions.hpp>

// ---
bool FZ80::BITSHIFTLeft_General::executeWith (MCHEmul::Register& r)
{
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	MCHEmul::UByte v = r.values ()[0];
	bool c = v.bit (7);
	v.shiftLeftC (c);
	r.set ({ v });

	// Affecting the flags...
	st.setBitStatus (FZ80::CZ80::_CARRYFLAG, c);
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, (v.numberBitsOn () % 2) == 0);
	// The rest are not affected...

	return (true);
}

// ---
bool FZ80::BITSHIFTLeft_General::executeWith (const MCHEmul::Address& a)
{
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	MCHEmul::UByte v = memory () -> value (a);
	bool c = v.bit (7);
	v.shiftLeftC (c);
	memory () -> set (a, v); // Set back the value...

	// Affecting the flags...
	st.setBitStatus (FZ80::CZ80::_CARRYFLAG, c);
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, (v.numberBitsOn () % 2) == 0);
	// The rest are not affected...

	return (true);
}

// ---
bool FZ80::BITSHIFTLeft_General::executeWith (const MCHEmul::Address& a, MCHEmul::Register& r)
{
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	MCHEmul::UByte v = memory () -> value (a);
	bool c = v.bit (7);
	v.shiftLeftC (c);
	memory () -> set (a, v); // Set back the value...
	r.set ({ v }); // And store it in the register...

	// Affecting the flags...
	st.setBitStatus (FZ80::CZ80::_CARRYFLAG, c);
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, (v.numberBitsOn () % 2) == 0);
	// The rest are not affected...

	return (true);
}

// ---
_INST_IMPL (FZ80::RLC_A)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA ()));
}

// ---
_INST_IMPL (FZ80::RLC_ASlower)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA ()));
}

// ---
_INST_IMPL (FZ80::RLC_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB ()));
}

// ---
_INST_IMPL (FZ80::RLC_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC ()));
}

// ---
_INST_IMPL (FZ80::RLC_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD ()));
}

// ---
_INST_IMPL (FZ80::RLC_E)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE ()));
}

// ---
_INST_IMPL (FZ80::RLC_H)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerH ()));
}

// ---
_INST_IMPL (FZ80::RLC_L)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerL ()));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (addressHL ()));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ())));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerA ()));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerB ()));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerC ()));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerD ()));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerE ()));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerH ()));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerL ()));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ())));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerA ()));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerB ()));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerC ()));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerD ()));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerE ()));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerH ()));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerL ()));
}
