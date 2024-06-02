#include <FZ80/CP.hpp>

// ---
_INST_IMPL (FZ80::CP_WithA)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U1CP_WithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U2CP_WithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::CP_WithB)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerB ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U1CP_WithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U2CP_WithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::CP_WithC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerC ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U1CP_WithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U2CP_WithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::CP_WithD)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerD ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U1CP_WithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U2CP_WithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::CP_WithE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerE ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U1CP_WithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::U2CP_WithE)
{
	assert (parameters ().size () == 2);


	return (executeWith (registerE ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::CP_WithH)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::CP_WithL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerL ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::CP_WithIndirectHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (valueAddressHL ()));
}

// ---
_INST_IMPL (FZ80::CP_WithIndirectIndexIX)
{
	assert (parameters ().size () == 3);

	return (executeWith (valueAddressIX ((char) parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::CP_WithIndirectIndexIY)
{
	assert (parameters ().size () == 3);

	return (executeWith (valueAddressIY ((char) parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::CP)
{
	assert (parameters ().size () == 2);

	return (executeWith (parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::CP_WithIXH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::CP_WithIXL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXL ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::CP_WithIYH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::CP_WithIYL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYL ().values ()[0]));
}
