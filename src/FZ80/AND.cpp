#include <FZ80/AND.hpp>

// ---
_INST_IMPL (FZ80::AND_A)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U1AND_A)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U2AND_A)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::AND_B)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerB ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U1AND_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U2AND_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::AND_C)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerC ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U1AND_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U2AND_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::AND_D)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerD ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U1AND_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U2AND_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::AND_E)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerE ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U1AND_E)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U2AND_E)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::AND_H)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::AND_L)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerL ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::AND_IndirectHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (valueAddressHL ()));
}

// ---
_INST_IMPL (FZ80::AND_IndirectIndexIX)
{
	assert (parameters ().size () == 3);

	return (executeWith (valueAddressIX ((char) parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::AND_IndirectIndexIY)
{
	assert (parameters ().size () == 3);

	return (executeWith (valueAddressIY ((char) parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::AND)
{
	assert (parameters ().size () == 2);

	return (executeWith (parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::AND_IXH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::AND_IXL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXL ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::AND_IYH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::AND_IYL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYL ().values ()[0]));
}
