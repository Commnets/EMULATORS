#include <FZ80/STACK.hpp>

// ---
_INST_IMPL (FZ80::PUSH_AF)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerAF ()));
}

// ---
_INST_IMPL (FZ80::PUSH_BC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerBC ()));
}

// ---
_INST_IMPL (FZ80::PUSH_DE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerDE ()));
}

// ---
_INST_IMPL (FZ80::PUSH_HL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerHL ()));
}

// ---
_INST_IMPL (FZ80::PUSH_IX)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIX ()));
}

// ---
_INST_IMPL (FZ80::PUSH_IY)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIY ()));
}

// ---
_INST_IMPL (FZ80::POP_AF)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerAF ()));
}

// ---
_INST_IMPL (FZ80::POP_BC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerBC ()));
}

// ---
_INST_IMPL (FZ80::POP_DE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerDE ()));
}

// ---
_INST_IMPL (FZ80::POP_HL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerHL ()));
}

// ---
_INST_IMPL (FZ80::POP_IX)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIX ()));
}

// ---
_INST_IMPL (FZ80::POP_IY)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIY ()));
}
