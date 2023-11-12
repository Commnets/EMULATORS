#include <FZ80/Instructions.hpp>

// ---
bool FZ80::SUB_General::executeWith (MCHEmul::Register& r, const MCHEmul::UByte& v, bool c)
{
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// The operation...
	bool a = c ? (st.bitStatus (FZ80::CZ80::_CARRYFLAG) ? true : false) : false;
	MCHEmul::UInt rst  = MCHEmul::UInt (r.values ()[0]).substract (MCHEmul::UInt (v), a);
	MCHEmul::UInt rstH = MCHEmul::UInt (r.values ()[0] & 0x0f).substract (MCHEmul::UInt (v & 0x0f), a); // Just to calculate the half carry!
	r.set (rst.values ()); // and stored back...

	// How the flags are affected...
	st.setBitStatus (FZ80::CZ80::_CARRYFLAG, rst.carry ());
	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, true); // Always!
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, rst.overflow ());
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, rstH [0].bit (4)); // When true, there will have been a half carry!
	st.setBitStatus (FZ80::CZ80::_ZEROFLAG, rst == MCHEmul::UInt::_0);
	st.setBitStatus (FZ80::CZ80::_SIGNFLAG, rst.negative ());

	return (true);
}

// ---
_INST_IMPL (FZ80::SUB_AWithA)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerA ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithB)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerB ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerC ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithD)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerD ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerE ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithF)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerF ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerL ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithIndirectHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), valueAddressHL (), false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithIndirectIndexIX)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerA (), valueAddressIX (parameters ()[2].value ()), false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithIndirectIndexIY)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerA (), valueAddressIY (parameters ()[2].value ()), false));
}

// ---
_INST_IMPL (FZ80::SUB_A)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), parameters ()[1].value (), false));
}

// ---
_INST_IMPL (FZ80::SBC_AWithA)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerA ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithB)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerB ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerC ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithD)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerD ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerE ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithF)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerF ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerL ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithIndirectHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), valueAddressHL (), true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithIndirectIndexIX)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerA (), valueAddressIX (parameters ()[2].value ()), true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithIndirectIndexIY)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerA (), valueAddressIY (parameters ()[2].value ()), true));
}

// ---
_INST_IMPL (FZ80::SBC_A)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), parameters ()[1].value (), true));
}
