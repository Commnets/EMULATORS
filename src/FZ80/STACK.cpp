#include <FZ80/Instructions.hpp>

// ---
bool FZ80::PUSH_General::executeWith (MCHEmul::RefRegisters& r)
{
	// The values are kept in the same order that they appear...
	stack () -> push (MCHEmul::UBytes ({ r [0] -> values ()[0], r [1]  -> values ()[0] })); // 2 bytes always...

	return (!stack () -> overflow ());
}

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
bool FZ80::POP_General::executeWith (MCHEmul::RefRegisters& r)
{
	// The bystes are extracted in the other way around...
	MCHEmul::UBytes dt = stack () -> pull (2); // 2 bytes always...
	r [1] -> set ({ dt [0].value () });
	r [0] -> set ({ dt [1].value () });

	return (!stack () -> overflow ());
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
