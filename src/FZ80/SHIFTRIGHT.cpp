#include <FZ80/SHIFTRIGHT.hpp>

// ---
_INST_IMPL (FZ80::SRA_A)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), false));
}

// ---
_INST_IMPL (FZ80::SRA_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), false));
}

// ---
_INST_IMPL (FZ80::SRA_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), false));
}

// ---
_INST_IMPL (FZ80::SRA_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), false));
}

// ---
_INST_IMPL (FZ80::SRA_E)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), false));
}

// ---
_INST_IMPL (FZ80::SRA_H)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerH (), false));
}

// ---
_INST_IMPL (FZ80::SRA_L)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerL (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (addressHL (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerA (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerB (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerC (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerD (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerE (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerH (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerL (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerA (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerB (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerC (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerD (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerE (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerH (), false));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerL (), false));
}

// ---
_INST_IMPL (FZ80::SRL_A)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), true));
}

// ---
_INST_IMPL (FZ80::SRL_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), true));
}

// ---
_INST_IMPL (FZ80::SRL_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), true));
}

// ---
_INST_IMPL (FZ80::SRL_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), true));
}

// ---
_INST_IMPL (FZ80::SRL_E)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), true));
}

// ---
_INST_IMPL (FZ80::SRL_H)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerH (), true));
}

// ---
_INST_IMPL (FZ80::SRL_L)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerL (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (addressHL (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerA (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerB (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerC (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerD (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerE (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerH (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerL (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerA (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerB (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerC (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerD (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerE (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerH (), true));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerL (), true));
}
