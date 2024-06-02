#include <FZ80/INC.hpp>

// ---
_INST_IMPL (FZ80::INC_A)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA ()));
}

// ---
_INST_IMPL (FZ80::U1INC_A)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA ()));
}

// ---
_INST_IMPL (FZ80::U2INC_A)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA ()));
}

// ---
_INST_IMPL (FZ80::INC_B)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerB ()));
}

// ---
_INST_IMPL (FZ80::U1INC_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB ()));
}

// ---
_INST_IMPL (FZ80::U2INC_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB ()));
}

// ---
_INST_IMPL (FZ80::INC_C)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerC ()));
}

// ---
_INST_IMPL (FZ80::U1INC_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC ()));
}

// ---
_INST_IMPL (FZ80::U2INC_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC ()));
}

// ---
_INST_IMPL (FZ80::INC_D)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerD ()));
}

// ---
_INST_IMPL (FZ80::U1INC_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD ()));
}

// ---
_INST_IMPL (FZ80::U2INC_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD ()));
}

// ---
_INST_IMPL (FZ80::INC_E)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerE ()));
}

// ---
_INST_IMPL (FZ80::U1INC_E)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE ()));
}

// ---
_INST_IMPL (FZ80::U2INC_E)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE ()));
}

// ---
_INST_IMPL (FZ80::INC_H)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerH ()));
}

// ---
_INST_IMPL (FZ80::INC_L)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerL ()));
}

// ---
_INST_IMPL (FZ80::INC_IndirectHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (addressHL ()));
}

// ---
_INST_IMPL (FZ80::INC_IndirectIndexIX)
{
	assert (parameters ().size () == 3);

	return (executeWith (addressIX ((char) parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::INC_IndirectIndexIY)
{
	assert (parameters ().size () == 3);

	return (executeWith (addressIY ((char) parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::INC_BC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerBC ()));
}

// ---
_INST_IMPL (FZ80::INC_DE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerDE ()));
}

// ---
_INST_IMPL (FZ80::INC_HL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerHL ()));
}

// ---
_INST_IMPL (FZ80::INC_SP)
{
	assert (parameters ().size () == 1);

	memory () -> stack () -> setPosition (memory () -> stack () -> position () + 1);
	
	return (true);
}

// ---
_INST_IMPL (FZ80::INC_IX)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIX ()));
}

// ---
_INST_IMPL (FZ80::INC_IY)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIY ()));
}

// ---
_INST_IMPL (FZ80::INC_IXH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXH ()));
}

// ---
_INST_IMPL (FZ80::INC_IXL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXL ()));
}

// ---
_INST_IMPL (FZ80::INC_IYH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYH ()));
}

// ---
_INST_IMPL (FZ80::INC_IYL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYL ()));
}
