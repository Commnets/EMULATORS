#include <FZ80/SHIFTLEFT.hpp>

// ---
_INST_IMPL (FZ80::SLA_A)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), false));
}

// ---
_INST_IMPL (FZ80::SLA_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), false));
}

// ---
_INST_IMPL (FZ80::SLA_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), false));
}

// ---
_INST_IMPL (FZ80::SLA_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), false));
}

// ---
_INST_IMPL (FZ80::SLA_E)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), false));
}

// ---
_INST_IMPL (FZ80::SLA_H)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerH (), false));
}

// ---
_INST_IMPL (FZ80::SLA_L)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerL (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (addressHL (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerA (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerB (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerC (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerD (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerE (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerH (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerL (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerA (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerB (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerC (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerD (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerE (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerH (), false));
}

// ---
_INST_IMPL (FZ80::SLA_IndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerL (), false));
}

// ---
_INST_IMPL (FZ80::SLL_A)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), true));
}

// ---
_INST_IMPL (FZ80::SLL_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), true));
}

// ---
_INST_IMPL (FZ80::SLL_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), true));
}

// ---
_INST_IMPL (FZ80::SLL_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), true));
}

// ---
_INST_IMPL (FZ80::SLL_E)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), true));
}

// ---
_INST_IMPL (FZ80::SLL_H)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerH (), true));
}

// ---
_INST_IMPL (FZ80::SLL_L)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerL (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (addressHL (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerA (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerB (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerC (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerD (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerE (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerH (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerL (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerA (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerB (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerC (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerD (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerE (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerH (), true));
}

// ---
_INST_IMPL (FZ80::SLL_IndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerL (), true));
}
