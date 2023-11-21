#include <FZ80/Instructions.hpp>

// ---
bool FZ80::BITSHIFTRight_General::executeWith (MCHEmul::Register& r)
{
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	MCHEmul::UByte v = r.values ()[0];
	bool c = v.bit (7);
	v.shiftRightC (c);
	r.set ({ v });

	// Affecting the flags...
	st.setBitStatus (FZ80::CZ80::_CARRYFLAG, c);
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, (v.numberBitsOn () % 2) == 0);
	// The rest are not affected...

	return (true);
}

// ---
bool FZ80::BITSHIFTRight_General::executeWith (const MCHEmul::Address& a)
{
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	MCHEmul::UByte v = memory () -> value (a);
	bool c = v.bit (7);
	v.shiftRightC (c);
	memory () -> set (a, v); // Set back the value...

	// Affecting the flags...
	st.setBitStatus (FZ80::CZ80::_CARRYFLAG, c);
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, (v.numberBitsOn () % 2) == 0);
	// The rest are not affected...

	return (true);

}

// ---
bool FZ80::BITSHIFTRight_General::executeWith (const MCHEmul::Address& a, MCHEmul::Register& r)
{
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	MCHEmul::UByte v = memory () -> value (a);
	bool c = v.bit (7);
	v.shiftRightC (c);
	memory () -> set (a, v); // Set back the value...
	r.set ({ v }); // ...and put its value back into the register...

	// Affecting the flags...
	st.setBitStatus (FZ80::CZ80::_CARRYFLAG, c);
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, (v.numberBitsOn () % 2) == 0);
	// The rest are not affected...

	return (true);

}
// ---
_INST_IMPL (FZ80::RRC_A)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA ()));
}

// ---
_INST_IMPL (FZ80::RRC_ASlower)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA ()));
}

// ---
_INST_IMPL (FZ80::RRC_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB ()));
}

// ---
_INST_IMPL (FZ80::RRC_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC ()));
}

// ---
_INST_IMPL (FZ80::RRC_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD ()));
}

// ---
_INST_IMPL (FZ80::RRC_E)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE ()));
}

// ---
_INST_IMPL (FZ80::RRC_F)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerF ()));
}

// ---
_INST_IMPL (FZ80::RRC_L)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerL ()));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (addressHL ()));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ())));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerA ()));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerB ()));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerC ()));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerD ()));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerE ()));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerH ()));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerL ()));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ())));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerA ()));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerB ()));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerC ()));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerD ()));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerE ()));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerH ()));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerL ()));
}
