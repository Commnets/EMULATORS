#include <FZ80/SUB.hpp>

// ---
_INST_IMPL (FZ80::SUB_AWithA)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::U1SUB_AWithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::U2SUB_AWithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithB)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerB ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::U1SUB_AWithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::U2SUB_AWithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerC ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::U1SUB_AWithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::U2SUB_AWithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithD)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerD ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::U1SUB_AWithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::U2SUB_AWithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerE ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::U1SUB_AWithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::U2SUB_AWithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithH)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerH ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerL ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithIndirectHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (valueAddressHL (), false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithIndirectIndexIX)
{
	assert (parameters ().size () == 3);

	return (executeWith (valueAddressIX (parameters ()[2].value ()), false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithIndirectIndexIY)
{
	assert (parameters ().size () == 3);

	return (executeWith (valueAddressIY (parameters ()[2].value ()), false));
}

// ---
_INST_IMPL (FZ80::SUB_A)
{
	assert (parameters ().size () == 2);

	return (executeWith (parameters ()[1].value (), false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithIXH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXH ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::SUB_AWithIXL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXL ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::SUB_AWithIYH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYH ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::SUB_AWithIYL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYL ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithA)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::U1SBC_AWithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::U2SBC_AWithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithB)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerB ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::U1SBC_AWithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::U2SBC_AWithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerC ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::U1SBC_AWithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::U2SBC_AWithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithD)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerD ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::U1SBC_AWithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::U2SBC_AWithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerE ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::U1SBC_AWithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::U2SBC_AWithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithH)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerH ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerL ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithIndirectHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (valueAddressHL (), true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithIndirectIndexIX)
{
	assert (parameters ().size () == 3);

	return (executeWith (valueAddressIX (parameters ()[2].value ()), true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithIndirectIndexIY)
{
	assert (parameters ().size () == 3);

	return (executeWith (valueAddressIY (parameters ()[2].value ()), true));
}

// ---
_INST_IMPL (FZ80::SBC_A)
{
	assert (parameters ().size () == 2);

	return (executeWith (parameters ()[1].value (), true));
}

// ---
_INST_IMPL (FZ80::SBC_HLWithBC)
{
	assert (parameters ().size () == 2);

	return (executeWith (valueRegisterBC ()));
}

// ---
_INST_IMPL (FZ80::SBC_HLWithDE)
{
	assert (parameters ().size () == 2);

	return (executeWith (valueRegisterDE ()));
}

// ---
_INST_IMPL (FZ80::SBC_HLWithHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (valueRegisterHL ()));
}

// ---
_INST_IMPL (FZ80::SBC_HLWithSP)
{
	assert (parameters ().size () == 2);

	return (executeWith (valueRegisterSP ()));
}

// ---
_INST_IMPL (FZ80::SBC_AWithIXH)
{
	assert (parameters ().size () == 2);

	return (executeWith (valueRegisterIXH (), true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithIXL)
{
	assert (parameters ().size () == 2);

	return (executeWith (valueRegisterIXL (), true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithIYH)
{
	assert (parameters ().size () == 2);

	return (executeWith (valueRegisterIYH (), true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithIYL)
{
	assert (parameters ().size () == 2);

	return (executeWith (valueRegisterIYL (), true));
}
