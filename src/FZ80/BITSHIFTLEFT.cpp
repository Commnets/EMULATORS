#include <FZ80/BITSHIFTLEFT.hpp>

// ---
_INST_IMPL (FZ80::RLC_A)
{
	assert (parameters ().size () == 1);

	return (executeRotateWith (registerA (), false));
}

// ---
_INST_IMPL (FZ80::RLC_ASlower)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerA (), false));
}

// ---
_INST_IMPL (FZ80::RLC_B)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerB (), false));
}

// ---
_INST_IMPL (FZ80::RLC_C)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerC (), false));
}

// ---
_INST_IMPL (FZ80::RLC_D)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerD (), false));
}

// ---
_INST_IMPL (FZ80::RLC_E)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerE (), false));
}

// ---
_INST_IMPL (FZ80::RLC_H)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerH (), false));
}

// ---
_INST_IMPL (FZ80::RLC_L)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerL (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (addressHL (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerA (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerB (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerC (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerD (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerE (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerH (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerL (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerA (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerB (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerC (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerD (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerE (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerH (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerL (), false));
}

// ---
_INST_IMPL (FZ80::RL_A)
{
	assert (parameters ().size () == 1);

	return (executeRotateWith (registerA (), true));
}

// ---
_INST_IMPL (FZ80::RL_ASlower)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerA (), true));
}

// ---
_INST_IMPL (FZ80::RL_B)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerB (), true));
}

// ---
_INST_IMPL (FZ80::RL_C)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerC (), true));
}

// ---
_INST_IMPL (FZ80::RL_D)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerD (), true));
}

// ---
_INST_IMPL (FZ80::RL_E)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerE (), true));
}

// ---
_INST_IMPL (FZ80::RL_H)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerH (), true));
}

// ---
_INST_IMPL (FZ80::RL_L)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (registerL (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeRotateWith (addressHL (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerA (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerB (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerC (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerD (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerE (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerH (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIX (parameters ()[3].value ()), registerL (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerA (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerB (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerC (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerD (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerE (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerH (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeRotateWith (addressIY (parameters ()[3].value ()), registerL (), true));
}

// ---
_INST_IMPL (FZ80::RLD)
{
	assert (parameters ().size () == 2);

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	const MCHEmul::UByte& v  = valueAddressHL ();
	const MCHEmul::UByte& a  = valueRegisterA ();
	MCHEmul::UByte nA = (a & 0xf0) | ((v & 0xf0) >> 4);
	MCHEmul::UByte nV = ((v & 0x0f) << 4) | (a & 0x0f);
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
_INST_IMPL (FZ80::SLA_A)
{
	assert (parameters ().size () == 1);

	return (executeShiftWith (registerA (), false));
}

// ---
_INST_IMPL (FZ80::SLA_B)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerB (), false));
}

// ---
_INST_IMPL (FZ80::SLA_C)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerC (), false));
}

// ---
_INST_IMPL (FZ80::SLA_D)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerD (), false));
}

// ---
_INST_IMPL (FZ80::SLA_E)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerE (), false));
}

// ---
_INST_IMPL (FZ80::SLA_H)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerH (), false));
}

// ---
_INST_IMPL (FZ80::SLA_L)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerL (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (addressHL (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerA (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerB (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerC (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerD (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerE (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerH (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerL (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerA (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerB (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerC (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerD (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerE (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerH (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerL (), false));
}

// ---
_INST_IMPL (FZ80::SLL_A)
{
	assert (parameters ().size () == 1);

	return (executeShiftWith (registerA (), true));
}

// ---
_INST_IMPL (FZ80::SLL_B)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerB (), true));
}

// ---
_INST_IMPL (FZ80::SLL_C)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerC (), true));
}

// ---
_INST_IMPL (FZ80::SLL_D)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerD (), true));
}

// ---
_INST_IMPL (FZ80::SLL_E)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerE (), true));
}

// ---
_INST_IMPL (FZ80::SLL_H)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerH (), true));
}

// ---
_INST_IMPL (FZ80::SLL_L)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (registerL (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeShiftWith (addressHL (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerA (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerB (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerC (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerD (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerE (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerH (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIX (parameters ()[3].value ()), registerL (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerA (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerB (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerC (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerD (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerE (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerH (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeShiftWith (addressIY (parameters ()[3].value ()), registerL (), true));
}
