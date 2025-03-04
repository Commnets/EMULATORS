#include <FZ80/XOR.hpp>

// ---
_INST_IMPL (FZ80::XOR_A)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U1XOR_A)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U2XOR_A)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::XOR_B)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerB ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U1XOR_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U2XOR_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::XOR_C)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerC ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U1XOR_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U2XOR_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::XOR_D)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerD ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U1XOR_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U2XOR_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::XOR_E)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerE ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U1XOR_E)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U2XOR_E)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::XOR_H)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::XOR_L)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerL ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::XOR_IndirectHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (valueAddressHL ()));
}

// ---
_INST_IMPL (FZ80::XOR_IndirectIndexIX)
{
	assert (parameters ().size () == 3);

	return (executeWith (valueAddressIX ((char) parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::XOR_IndirectIndexIY)
{
	assert (parameters ().size () == 3);

	return (executeWith (valueAddressIY ((char) parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::XOR)
{
	assert (parameters ().size () == 2);

	return (executeWith (parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::XOR_IXH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::XOR_IXL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXL ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::XOR_IYH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::XOR_IYL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYL ().values ()[0]));
}
