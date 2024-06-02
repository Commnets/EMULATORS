#include <FZ80/DEC.hpp>

// ---
_INST_IMPL (FZ80::DEC_A)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA ()));
}

// ---
_INST_IMPL (FZ80::U1DEC_A)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA ()));
}

// ---
_INST_IMPL (FZ80::U2DEC_A)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA ()));
}

// ---
_INST_IMPL (FZ80::DEC_B)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerB ()));
}

// ---
_INST_IMPL (FZ80::U1DEC_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB ()));
}

// ---
_INST_IMPL (FZ80::U2DEC_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB ()));
}

// ---
_INST_IMPL (FZ80::DEC_C)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerC ()));
}

// ---
_INST_IMPL (FZ80::U1DEC_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC ()));
}

// ---
_INST_IMPL (FZ80::U2DEC_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC ()));
}

// ---
_INST_IMPL (FZ80::DEC_D)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerD ()));
}

// ---
_INST_IMPL (FZ80::U1DEC_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD ()));
}

// ---
_INST_IMPL (FZ80::U2DEC_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD ()));
}

// ---
_INST_IMPL (FZ80::DEC_E)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerE ()));
}

// ---
_INST_IMPL (FZ80::U1DEC_E)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE ()));
}

// ---
_INST_IMPL (FZ80::U2DEC_E)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE ()));
}

// ---
_INST_IMPL (FZ80::DEC_H)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerH ()));
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

	return (executeWith (addressIX ((char) parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::DEC_IndirectIndexIY)
{
	assert (parameters ().size () == 3);

	return (executeWith (addressIY ((char) parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::DEC_BC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerBC ()));
}

// ---
_INST_IMPL (FZ80::DEC_DE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerDE ()));
}

// ---
_INST_IMPL (FZ80::DEC_HL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerHL ()));
}

// ---
_INST_IMPL (FZ80::DEC_SP)
{
	assert (parameters ().size () == 1);

	memory () -> stack () -> setPosition (memory () -> stack () -> position () - 1);
	
	return (true);
}

// ---
_INST_IMPL (FZ80::DEC_IX)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIX ()));
}

// ---
_INST_IMPL (FZ80::DEC_IY)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIY ()));
}

// ---
_INST_IMPL (FZ80::DEC_IXH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXH ()));
}

// ---
_INST_IMPL (FZ80::DEC_IXL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXL ()));
}

// ---
_INST_IMPL (FZ80::DEC_IYH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYH ()));
}

// ---
_INST_IMPL (FZ80::DEC_IYL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYL ()));
}
