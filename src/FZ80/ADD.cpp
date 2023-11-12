#include <FZ80/Instructions.hpp>

// ---
bool FZ80::ADD_General::executeWith (MCHEmul::Register& r, const MCHEmul::UByte& v, bool c)
{
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// The operation...
	bool a = c ? (st.bitStatus (FZ80::CZ80::_CARRYFLAG) ? true : false) : false;
	MCHEmul::UInt rst  = MCHEmul::UInt (r.values ()[0]).add (MCHEmul::UInt (v, a));
	MCHEmul::UInt rstH = MCHEmul::UInt (r.values ()[0] & 0x0f).add (MCHEmul::UInt (v & 0x0f, a)); // Just to calculate the half carry!
	r.set (rst.values ()); // and stored back...

	// How the flags are affected...
	st.setBitStatus (FZ80::CZ80::_CARRYFLAG, rst.carry ());
	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, false); // Always!
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, rst.overflow ());
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, rstH [0].bit (4)); // When true, there will have been a half carry!
	st.setBitStatus (FZ80::CZ80::_ZEROFLAG, rst == MCHEmul::UInt::_0);
	st.setBitStatus (FZ80::CZ80::_SIGNFLAG, rst.negative ());

	return (true);
}

// ---
_INST_IMPL (FZ80::ADD_AWithA)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerA ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithB)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerB ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerC ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithD)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerD ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerE ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithF)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerF ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerL ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithIndirectHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), valueAddressHL (), false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithIndirectIndexIX)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerA (), valueAddressIX (parameters ()[2].value ()), false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithIndirectIndexIY)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerA (), valueAddressIY (parameters ()[2].value ()), false));
}

// ---
_INST_IMPL (FZ80::ADD_A)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), parameters ()[1].value (), false));
}

// ---
_INST_IMPL (FZ80::ADC_AWithA)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerA ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithB)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerB ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerC ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithD)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerD ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerE ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithF)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerF ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerL ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithIndirectHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), valueAddressHL (), true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithIndirectIndexIX)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerA (), valueAddressIX (parameters ()[2].value ()), true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithIndirectIndexIY)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerA (), valueAddressIY (parameters ()[2].value ()), true));
}

// ---
_INST_IMPL (FZ80::ADC_A)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), parameters ()[1].value (), true));
}
