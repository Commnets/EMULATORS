#include <FZ80/Instructions.hpp>

// ---
bool FZ80::BITSHIFTRight_General::executeRotateWith (MCHEmul::Register& r, bool crr)
{
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	MCHEmul::UByte v = r.values ()[0];
	bool c = v.bit (7);
	v.shiftRightC (crr ? st.bitStatus (FZ80::CZ80::_CARRYFLAG) : c);
	r.set ({ v });

	// When the register affect is A and is the documented version, 
	// the flags affected are different (some less) than in non documented versions...
	if (&r == &registerA () && parameters ().size () == 1)
	{
		// How the flags are accepted...
		st.setBitStatus (FZ80::CZ80::_CARRYFLAG, c);
		st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, false); // Always
		// Flag Parity/Overflow not affected...
		st.setBitStatus (FZ80::CZ80::_BIT3FLAG, v.bit (3)); // Undocumented...
		st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, false);
		st.setBitStatus (FZ80::CZ80::_BIT5FLAG, v.bit (5)); // Undocumented...
		// Flag Negative not affected...
		// Flag Sign not affected...
	}
	else
		affectFlagsWith (v, c);

	return (true);
}

// ---
bool FZ80::BITSHIFTRight_General::executeRotateWith (const MCHEmul::Address& a, bool crr)
{
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	MCHEmul::UByte v = memory () -> value (a);
	bool c = v.bit (7);
	v.shiftRightC (crr ? st.bitStatus (FZ80::CZ80::_CARRYFLAG) : c);
	memory () -> set (a, v); // Set back the value...

	affectFlagsWith (v, c);

	return (true);
}

// ---
bool FZ80::BITSHIFTRight_General::executeRotateWith (const MCHEmul::Address& a, MCHEmul::Register& r, bool crr)
{
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	MCHEmul::UByte v = memory () -> value (a);
	bool c = v.bit (7);
	v.shiftLeftC (crr ? st.bitStatus (FZ80::CZ80::_CARRYFLAG) : c);
	memory () -> set (a, v); // Set back the value...
	r.set ({ v }); // And store it in the register...

	affectFlagsWith (v, c);

	return (true);
}

// ---
bool FZ80::BITSHIFTRight_General::executeShiftWith (MCHEmul::Register& r, bool crr)
{
	// TODO

	return (true);
}

// ---
bool FZ80::BITSHIFTRight_General::executeShiftWith (const MCHEmul::Address& a, bool crr)
{
	// TODO

	return (true);
}

// ---
bool FZ80::BITSHIFTRight_General::executeShiftWith (const MCHEmul::Address& a, MCHEmul::Register& r, bool crr)
{
	// TODO

	return (true);
}

// ---
_INST_IMPL (FZ80::RRC_A)
{
	assert (parameters ().size () == 1);

	return (executeRotateWith (registerA (), false));
}

// ---
_INST_IMPL (FZ80::RRC_ASlower)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerA (), false));
}

// ---
_INST_IMPL (FZ80::RRC_B)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerB (), false));
}

// ---
_INST_IMPL (FZ80::RRC_C)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerC (), false));
}

// ---
_INST_IMPL (FZ80::RRC_D)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerD (), false));
}

// ---
_INST_IMPL (FZ80::RRC_E)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerE (), false));
}

// ---
_INST_IMPL (FZ80::RRC_F)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerF (), false));
}

// ---
_INST_IMPL (FZ80::RRC_L)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerL (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (addressHL (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerA (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerB (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerC (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerD (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerE (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerH (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerL (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerA (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerB (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerC (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerD (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerE (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerH (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerL (), false));
}

// ---
_INST_IMPL (FZ80::RR_A)
{
	assert (parameters ().size () == 1);

	return (executeRotateWith (registerA (), true));
}

// ---
_INST_IMPL (FZ80::RR_ASlower)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerA (), true));
}

// ---
_INST_IMPL (FZ80::RR_B)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerB (), true));
}

// ---
_INST_IMPL (FZ80::RR_C)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerC (), true));
}

// ---
_INST_IMPL (FZ80::RR_D)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerD (), true));
}

// ---
_INST_IMPL (FZ80::RR_E)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerE (), true));
}

// ---
_INST_IMPL (FZ80::RR_F)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerF (), true));
}

// ---
_INST_IMPL (FZ80::RR_L)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerL (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (addressHL (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerA (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerB (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerC (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerD (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerE (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerH (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerL (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerA (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerB (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerC (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerD (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerE (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerH (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerL (), true));
}

// ---
_INST_IMPL (FZ80::RRD)
{
	assert (parameters ().size () == 2);

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	const MCHEmul::UByte& v  = valueAddressHL ();
	const MCHEmul::UByte& a  = valueRegisterA ();
	MCHEmul::UByte nA = (a & 0xf0) | (v & 0x0f);
	MCHEmul::UByte nV = ((a & 0x0f) << 4) | ((v & 0xf0) >> 4);
	memory () -> set (addressHL (), nV);
	registerA ().set ({ nA });

	// How flags are accepted...
	// Carry flag is nor accepted!
	st.setBitStatus (CZ80::_NEGATIVEFLAG, false); // Always!
	st.setBitStatus (CZ80::_PARITYOVERFLOWFLAG, (nA.numberBitsOn () % 2) == 0);
	st.setBitStatus (CZ80::_BIT3FLAG, nA.bit (3)); // Undocumented...
	st.setBitStatus (CZ80::_HALFCARRYFLAG, false);
	st.setBitStatus (CZ80::_BIT5FLAG, nA.bit (5)); // Undocumented...
	st.setBitStatus (CZ80::_NEGATIVEFLAG, nA == MCHEmul::UByte::_0);
	st.setBitStatus (CZ80::_SIGNFLAG, nA.bit (7));

	return (true);
}
