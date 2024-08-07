#include <FZ80/SHIFTRIGHT.hpp>

// ---
std::vector <MCHEmul::UByte> FZ80::SHIFTRight_Index::shapeCodeWithData
	(const std::vector <std::vector <MCHEmul::UByte>>& b, bool& e) const
{
	// Build the instruction...
	std::vector <MCHEmul::UByte> result = 
		MCHEmul::UInt::fromUnsignedInt ((code () & 0xffff00) >> 8).bytes (); // First 2 bytes of its code...
	for (const auto& i : b)
		result.insert (result.end (), i.begin (), i.end ()); // Then the data...
	result.insert (result.end (), (unsigned char) (code () & 0xff)); //...and finally the last byte of the code!

	// Is the result right in length?
	// If not it was because either the number of parameters was wrong
	// or the code of the instruction was longer that expected
	// There might happen however that with both mistakes at the same time the length was ok!
	e = (result.size () != memoryPositions ());

	return (result);
}

// ---
_INST_IMPL (FZ80::SRA_A)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), true));
}

// ---
_INST_IMPL (FZ80::SRA_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), true));
}

// ---
_INST_IMPL (FZ80::SRA_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), true));
}

// ---
_INST_IMPL (FZ80::SRA_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), true));
}

// ---
_INST_IMPL (FZ80::SRA_E)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), true));
}

// ---
_INST_IMPL (FZ80::SRA_H)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerH (), true));
}

// ---
_INST_IMPL (FZ80::SRA_L)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerL (), true));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (addressHL (), true));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), true));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerA (), true));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerB (), true));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerC (), true));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerD (), true));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerE (), true));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerH (), true));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerL (), true));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), true));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerA (), true));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerB (), true));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerC (), true));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerD (), true));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerE (), true));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerH (), true));
}

// ---
_INST_IMPL (FZ80::SRA_IndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerL (), true));
}

// ---
_INST_IMPL (FZ80::SRL_A)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), false));
}

// ---
_INST_IMPL (FZ80::SRL_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), false));
}

// ---
_INST_IMPL (FZ80::SRL_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), false));
}

// ---
_INST_IMPL (FZ80::SRL_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), false));
}

// ---
_INST_IMPL (FZ80::SRL_E)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), false));
}

// ---
_INST_IMPL (FZ80::SRL_H)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerH (), false));
}

// ---
_INST_IMPL (FZ80::SRL_L)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerL (), false));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (addressHL (), false));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), false));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerA (), false));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerB (), false));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerC (), false));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerD (), false));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerE (), false));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerH (), false));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerL (), false));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), false));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerA (), false));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerB (), false));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerC (), false));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerD (), false));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerE (), false));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerH (), false));
}

// ---
_INST_IMPL (FZ80::SRL_IndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerL (), false));
}
