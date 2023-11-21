#include <FZ80/Instructions.hpp>

// ---
bool FZ80::OR_General::executeWith (const MCHEmul::UByte& v)
{
	MCHEmul::Register& a = registerA ();
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// The operation...
	MCHEmul::UByte r = a.values ()[0] | v;
	a.set ({ r }); // and stored back...

	// How the flags are affected...
	st.setBitStatus (FZ80::CZ80::_CARRYFLAG, false);
	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, false);
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, (r.numberBitsOn () % 2) == 0); // If it isn even number then it is set...
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, true);
	st.setBitStatus (FZ80::CZ80::_ZEROFLAG, r == MCHEmul::UByte::_0);
	st.setBitStatus (FZ80::CZ80::_SIGNFLAG, r.bit (7));

	return (true);
}

// ---
_INST_IMPL (FZ80::OR_A)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerA ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::OR_B)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerB ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::OR_C)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerC ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::OR_D)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerD ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::OR_E)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerE ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::OR_H)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::OR_L)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerL ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::OR_IndirectHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (valueAddressHL ()));
}

// ---
_INST_IMPL (FZ80::OR_IndirectIndexIX)
{
	assert (parameters ().size () == 3);

	return (executeWith (valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::OR_IndirectIndexIY)
{
	assert (parameters ().size () == 3);

	return (executeWith (valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::OR)
{
	assert (parameters ().size () == 2);

	return (executeWith (parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::OR_IXH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::OR_IXL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXL ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::OR_IYH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::OR_IYL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYL ().values ()[0]));
}
