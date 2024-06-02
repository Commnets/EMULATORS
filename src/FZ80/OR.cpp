#include <FZ80/OR.hpp>

// ---
_INST_IMPL (FZ80::OR_A)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U1OR_A)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U2OR_A)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::OR_B)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerB ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U1OR_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U2OR_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::OR_C)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerC ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U1OR_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U2OR_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::OR_D)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerD ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U1OR_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U2OR_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::OR_E)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerE ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U1OR_E)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U2OR_E)
{
	assert (parameters ().size () == 2);

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

	return (executeWith (valueAddressIX ((char) parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::OR_IndirectIndexIY)
{
	assert (parameters ().size () == 3);

	return (executeWith (valueAddressIY ((char) parameters ()[2].value ())));
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
