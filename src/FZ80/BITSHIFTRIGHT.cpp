#include <FZ80/BITSHIFTRIGHT.hpp>

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

// ---
_INST_IMPL (FZ80::SRA_A)
{
	assert (parameters ().size () == 1);

	return (executeShiftWith (registerA (), false));
}

// ---
_INST_IMPL (FZ80::SRA_B)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerB (), false));
}

// ---
_INST_IMPL (FZ80::SRA_C)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerC (), false));
}

// ---
_INST_IMPL (FZ80::SRA_D)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerD (), false));
}

// ---
_INST_IMPL (FZ80::SRA_E)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerE (), false));
}

// ---
_INST_IMPL (FZ80::SRA_H)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerH (), false));
}

// ---
_INST_IMPL (FZ80::SRA_L)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerL (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (addressHL (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerA (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerB (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerC (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerD (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerE (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerH (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerL (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerA (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerB (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerC (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerD (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerE (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerH (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerL (), false));
}

// ---
_INST_IMPL (FZ80::SRL_A)
{
	assert (parameters ().size () == 1);

	return (executeShiftWith (registerA (), true));
}

// ---
_INST_IMPL (FZ80::SRL_B)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerB (), true));
}

// ---
_INST_IMPL (FZ80::SRL_C)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerC (), true));
}

// ---
_INST_IMPL (FZ80::SRL_D)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerD (), true));
}

// ---
_INST_IMPL (FZ80::SRL_E)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerE (), true));
}

// ---
_INST_IMPL (FZ80::SRL_H)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerH (), true));
}

// ---
_INST_IMPL (FZ80::SRL_L)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerL (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (addressHL (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerA (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerB (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerC (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerD (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerE (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerH (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerL (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerA (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerB (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerC (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerD (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerE (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerH (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerL (), true));
}
