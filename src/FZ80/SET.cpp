#include <FZ80/SET.hpp>

// ---
std::vector <MCHEmul::UByte> FZ80::SET_Index::shapeCodeWithData
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
_INST_IMPL (FZ80::SET_0WithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (0, registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_0WithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (0, registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_0WithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (0, registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_0WithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (0, registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_0WithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (0, registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_0WithH)
{
	assert (parameters ().size () == 2);

	return (executeWith (0, registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_0WithL)
{
	assert (parameters ().size () == 2);

	return (executeWith (0, registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_0WithIndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (0, addressHL ()));
}

// ---
_INST_IMPL (FZ80::SET_0WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (0, addressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::SET_0WithIndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (0, addressIY (parameters ()[2].value ()), registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_0WithIndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (0, addressIY (parameters ()[2].value ()), registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_0WithIndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (0, addressIY (parameters ()[2].value ()), registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_0WithIndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (0, addressIY (parameters ()[2].value ()), registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_0WithIndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (0, addressIY (parameters ()[2].value ()), registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_0WithIndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (0, addressIY (parameters ()[2].value ()), registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_0WithIndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (0, addressIY (parameters ()[2].value ()), registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_0WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (0, addressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::SET_0WithIndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (0, addressIY (parameters ()[2].value ()), registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_0WithIndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (0, addressIY (parameters ()[2].value ()), registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_0WithIndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (0, addressIY (parameters ()[2].value ()), registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_0WithIndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (0, addressIY (parameters ()[2].value ()), registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_0WithIndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (0, addressIY (parameters ()[2].value ()), registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_0WithIndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (0, addressIY (parameters ()[2].value ()), registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_0WithIndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (0, addressIY (parameters ()[2].value ()), registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_1WithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (1, registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_1WithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (1, registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_1WithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (1, registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_1WithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (1, registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_1WithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (1, registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_1WithH)
{
	assert (parameters ().size () == 2);

	return (executeWith (1, registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_1WithL)
{
	assert (parameters ().size () == 2);

	return (executeWith (1, registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_1WithIndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (1, addressHL ()));
}

// ---
_INST_IMPL (FZ80::SET_1WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (1, addressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::SET_1WithIndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (1, addressIY (parameters ()[2].value ()), registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_1WithIndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (1, addressIY (parameters ()[2].value ()), registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_1WithIndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (1, addressIY (parameters ()[2].value ()), registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_1WithIndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (1, addressIY (parameters ()[2].value ()), registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_1WithIndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (1, addressIY (parameters ()[2].value ()), registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_1WithIndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (1, addressIY (parameters ()[2].value ()), registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_1WithIndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (1, addressIY (parameters ()[2].value ()), registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_1WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (1, addressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::SET_1WithIndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (1, addressIY (parameters ()[2].value ()), registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_1WithIndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (1, addressIY (parameters ()[2].value ()), registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_1WithIndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (1, addressIY (parameters ()[2].value ()), registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_1WithIndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (1, addressIY (parameters ()[2].value ()), registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_1WithIndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (1, addressIY (parameters ()[2].value ()), registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_1WithIndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (1, addressIY (parameters ()[2].value ()), registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_1WithIndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (1, addressIY (parameters ()[2].value ()), registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_2WithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (2, registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_2WithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (2, registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_2WithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (2, registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_2WithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (2, registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_2WithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (2, registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_2WithH)
{
	assert (parameters ().size () == 2);

	return (executeWith (2, registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_2WithL)
{
	assert (parameters ().size () == 2);

	return (executeWith (2, registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_2WithIndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (2, addressHL ()));
}

// ---
_INST_IMPL (FZ80::SET_2WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (2, addressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::SET_2WithIndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (2, addressIY (parameters ()[2].value ()), registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_2WithIndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (2, addressIY (parameters ()[2].value ()), registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_2WithIndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (2, addressIY (parameters ()[2].value ()), registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_2WithIndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (2, addressIY (parameters ()[2].value ()), registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_2WithIndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (2, addressIY (parameters ()[2].value ()), registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_2WithIndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (2, addressIY (parameters ()[2].value ()), registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_2WithIndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (2, addressIY (parameters ()[2].value ()), registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_2WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (2, addressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::SET_2WithIndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (2, addressIY (parameters ()[2].value ()), registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_2WithIndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (2, addressIY (parameters ()[2].value ()), registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_2WithIndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (2, addressIY (parameters ()[2].value ()), registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_2WithIndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (2, addressIY (parameters ()[2].value ()), registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_2WithIndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (2, addressIY (parameters ()[2].value ()), registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_2WithIndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (2, addressIY (parameters ()[2].value ()), registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_2WithIndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (2, addressIY (parameters ()[2].value ()), registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_3WithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (3, registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_3WithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (3, registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_3WithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (3, registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_3WithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (3, registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_3WithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (3, registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_3WithH)
{
	assert (parameters ().size () == 2);

	return (executeWith (3, registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_3WithL)
{
	assert (parameters ().size () == 2);

	return (executeWith (3, registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_3WithIndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (3, addressHL ()));
}

// ---
_INST_IMPL (FZ80::SET_3WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (3, addressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::SET_3WithIndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (3, addressIY (parameters ()[2].value ()), registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_3WithIndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (3, addressIY (parameters ()[2].value ()), registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_3WithIndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (3, addressIY (parameters ()[2].value ()), registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_3WithIndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (3, addressIY (parameters ()[2].value ()), registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_3WithIndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (3, addressIY (parameters ()[2].value ()), registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_3WithIndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (3, addressIY (parameters ()[2].value ()), registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_3WithIndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (3, addressIY (parameters ()[2].value ()), registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_3WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (3, addressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::SET_3WithIndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (3, addressIY (parameters ()[2].value ()), registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_3WithIndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (3, addressIY (parameters ()[2].value ()), registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_3WithIndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (3, addressIY (parameters ()[2].value ()), registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_3WithIndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (3, addressIY (parameters ()[2].value ()), registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_3WithIndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (3, addressIY (parameters ()[2].value ()), registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_3WithIndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (3, addressIY (parameters ()[2].value ()), registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_3WithIndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (3, addressIY (parameters ()[2].value ()), registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_4WithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (4, registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_4WithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (4, registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_4WithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (4, registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_4WithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (4, registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_4WithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (4, registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_4WithH)
{
	assert (parameters ().size () == 2);

	return (executeWith (4, registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_4WithL)
{
	assert (parameters ().size () == 2);

	return (executeWith (4, registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_4WithIndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (4, addressHL ()));
}

// ---
_INST_IMPL (FZ80::SET_4WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (4, addressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::SET_4WithIndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (4, addressIY (parameters ()[2].value ()), registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_4WithIndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (4, addressIY (parameters ()[2].value ()), registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_4WithIndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (4, addressIY (parameters ()[2].value ()), registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_4WithIndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (4, addressIY (parameters ()[2].value ()), registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_4WithIndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (4, addressIY (parameters ()[2].value ()), registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_4WithIndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (4, addressIY (parameters ()[2].value ()), registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_4WithIndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (4, addressIY (parameters ()[2].value ()), registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_4WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (4, addressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::SET_4WithIndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (4, addressIY (parameters ()[2].value ()), registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_4WithIndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (4, addressIY (parameters ()[2].value ()), registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_4WithIndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (4, addressIY (parameters ()[2].value ()), registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_4WithIndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (4, addressIY (parameters ()[2].value ()), registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_4WithIndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (4, addressIY (parameters ()[2].value ()), registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_4WithIndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (4, addressIY (parameters ()[2].value ()), registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_4WithIndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (4, addressIY (parameters ()[2].value ()), registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_5WithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (5, registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_5WithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (5, registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_5WithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (5, registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_5WithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (5, registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_5WithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (5, registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_5WithH)
{
	assert (parameters ().size () == 2);

	return (executeWith (5, registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_5WithL)
{
	assert (parameters ().size () == 2);

	return (executeWith (5, registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_5WithIndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (5, addressHL ()));
}

// ---
_INST_IMPL (FZ80::SET_5WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (5, addressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::SET_5WithIndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (5, addressIY (parameters ()[2].value ()), registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_5WithIndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (5, addressIY (parameters ()[2].value ()), registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_5WithIndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (5, addressIY (parameters ()[2].value ()), registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_5WithIndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (5, addressIY (parameters ()[2].value ()), registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_5WithIndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (5, addressIY (parameters ()[2].value ()), registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_5WithIndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (5, addressIY (parameters ()[2].value ()), registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_5WithIndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (5, addressIY (parameters ()[2].value ()), registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_5WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (5, addressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::SET_5WithIndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (5, addressIY (parameters ()[2].value ()), registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_5WithIndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (5, addressIY (parameters ()[2].value ()), registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_5WithIndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (5, addressIY (parameters ()[2].value ()), registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_5WithIndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (5, addressIY (parameters ()[2].value ()), registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_5WithIndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (5, addressIY (parameters ()[2].value ()), registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_5WithIndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (5, addressIY (parameters ()[2].value ()), registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_5WithIndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (5, addressIY (parameters ()[2].value ()), registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_6WithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (6, registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_6WithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (6, registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_6WithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (6, registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_6WithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (6, registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_6WithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (6, registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_6WithH)
{
	assert (parameters ().size () == 2);

	return (executeWith (6, registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_6WithL)
{
	assert (parameters ().size () == 2);

	return (executeWith (6, registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_6WithIndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (6, addressHL ()));
}

// ---
_INST_IMPL (FZ80::SET_6WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (6, addressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::SET_6WithIndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (6, addressIY (parameters ()[2].value ()), registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_6WithIndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (6, addressIY (parameters ()[2].value ()), registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_6WithIndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (6, addressIY (parameters ()[2].value ()), registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_6WithIndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (6, addressIY (parameters ()[2].value ()), registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_6WithIndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (6, addressIY (parameters ()[2].value ()), registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_6WithIndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (6, addressIY (parameters ()[2].value ()), registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_6WithIndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (6, addressIY (parameters ()[2].value ()), registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_6WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (6, addressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::SET_6WithIndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (6, addressIY (parameters ()[2].value ()), registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_6WithIndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (6, addressIY (parameters ()[2].value ()), registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_6WithIndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (6, addressIY (parameters ()[2].value ()), registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_6WithIndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (6, addressIY (parameters ()[2].value ()), registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_6WithIndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (6, addressIY (parameters ()[2].value ()), registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_6WithIndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (6, addressIY (parameters ()[2].value ()), registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_6WithIndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (6, addressIY (parameters ()[2].value ()), registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_7WithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (7, registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_7WithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (7, registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_7WithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (7, registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_7WithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (7, registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_7WithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (7, registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_7WithH)
{
	assert (parameters ().size () == 2);

	return (executeWith (7, registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_7WithL)
{
	assert (parameters ().size () == 2);

	return (executeWith (7, registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_7WithIndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (7, addressHL ()));
}

// ---
_INST_IMPL (FZ80::SET_7WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (7, addressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::SET_7WithIndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (7, addressIY (parameters ()[2].value ()), registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_7WithIndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (7, addressIY (parameters ()[2].value ()), registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_7WithIndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (7, addressIY (parameters ()[2].value ()), registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_7WithIndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (7, addressIY (parameters ()[2].value ()), registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_7WithIndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (7, addressIY (parameters ()[2].value ()), registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_7WithIndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (7, addressIY (parameters ()[2].value ()), registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_7WithIndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (7, addressIY (parameters ()[2].value ()), registerL ()));
}

// ---
_INST_IMPL (FZ80::SET_7WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (7, addressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::SET_7WithIndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (7, addressIY (parameters ()[2].value ()), registerA ()));
}

// ---
_INST_IMPL (FZ80::SET_7WithIndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (7, addressIY (parameters ()[2].value ()), registerB ()));
}

// ---
_INST_IMPL (FZ80::SET_7WithIndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (7, addressIY (parameters ()[2].value ()), registerC ()));
}

// ---
_INST_IMPL (FZ80::SET_7WithIndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (7, addressIY (parameters ()[2].value ()), registerD ()));
}

// ---
_INST_IMPL (FZ80::SET_7WithIndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (7, addressIY (parameters ()[2].value ()), registerE ()));
}

// ---
_INST_IMPL (FZ80::SET_7WithIndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (7, addressIY (parameters ()[2].value ()), registerH ()));
}

// ---
_INST_IMPL (FZ80::SET_7WithIndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (7, addressIY (parameters ()[2].value ()), registerL ()));
}
