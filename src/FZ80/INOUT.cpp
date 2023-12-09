#include <FZ80/INOUT.hpp>

// ---
_INST_IMPL (FZ80::IN_A)
{
	assert (parameters ().size () == 2);

	return (executeAWith (parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::IN_AFromPort)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerA (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::IN_BFromPort)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerB (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::IN_CFromPort)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerC (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::IN_DFromPort)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerD (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::IN_EFromPort)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerE (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::IN_HFromPort)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerH (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::IN_LFromPort)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerL (), parameters ()[3].value ()));
}

// ---
_INST_IMPL (FZ80::IN_FFromPort)
{
	assert (parameters ().size () == 3);

	return (executeWith (parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::OUT_A)
{
	assert (parameters ().size () == 2);

	return (executeAWith (parameters ()[1].value ()));
}

// ---
_INST_IMPL (FZ80::OUT_AToPort)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerA (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::OUT_BToPort)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerB (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::OUT_CToPort)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerC (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::OUT_DToPort)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerD (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::OUT_EToPort)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerE (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::OUT_HToPort)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerH (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::OUT_LToPort)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerL (), parameters ()[2].value ()));
}

// ---
_INST_IMPL (FZ80::OUT_0ToPort)
{
	assert (parameters ().size () == 2);

	return (execute0With (0));
}
