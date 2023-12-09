#include <FZ80/BIT.hpp>

// ---
std::vector <MCHEmul::UByte> FZ80::BIT_Index::shapeCodeWithData
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
_INST_IMPL (FZ80::BIT_0WithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (0, registerA ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_0WithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (0, registerB ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_0WithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (0, registerC ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_0WithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (0, registerD ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_0WithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (0, registerE ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_0WithH)
{
	assert (parameters ().size () == 2);

	return (executeWith (0, registerH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_0WithL)
{
	assert (parameters ().size () == 2);

	return (executeWith (0, registerL ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_0WithIndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (0, valueAddressHL ()));
}

// ---
_INST_IMPL (FZ80::BIT_0WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (0, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U1BIT_0WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (0, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U2BIT_0WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (0, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U3BIT_0WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (0, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U4BIT_0WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (0, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U5BIT_0WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (0, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U6BIT_0WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (0, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U7BIT_0WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (0, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::BIT_0WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (0, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U1BIT_0WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (0, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U2BIT_0WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (0, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U3BIT_0WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (0, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U4BIT_0WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (0, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U5BIT_0WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (0, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U6BIT_0WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (0, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U7BIT_0WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (0, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::BIT_1WithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (1, registerA ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_1WithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (1, registerB ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_1WithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (1, registerC ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_1WithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (1, registerD ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_1WithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (1, registerE ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_1WithH)
{
	assert (parameters ().size () == 2);

	return (executeWith (1, registerH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_1WithL)
{
	assert (parameters ().size () == 2);

	return (executeWith (1, registerL ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_1WithIndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (1, valueAddressHL ()));
}

// ---
_INST_IMPL (FZ80::BIT_1WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (1, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U1BIT_1WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (1, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U2BIT_1WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (1, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U3BIT_1WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (1, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U4BIT_1WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (1, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U5BIT_1WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (1, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U6BIT_1WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (1, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U7BIT_1WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (1, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::BIT_1WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (1, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U1BIT_1WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (1, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U2BIT_1WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (1, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U3BIT_1WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (1, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U4BIT_1WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (1, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U5BIT_1WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (1, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U6BIT_1WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (1, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U7BIT_1WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (1, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::BIT_2WithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (2, registerA ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_2WithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (2, registerB ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_2WithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (2, registerC ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_2WithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (2, registerD ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_2WithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (2, registerE ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_2WithH)
{
	assert (parameters ().size () == 2);

	return (executeWith (2, registerH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_2WithL)
{
	assert (parameters ().size () == 2);

	return (executeWith (2, registerL ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_2WithIndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (2, valueAddressHL ()));
}

// ---
_INST_IMPL (FZ80::BIT_2WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (2, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U1BIT_2WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (2, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U2BIT_2WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (2, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U3BIT_2WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (2, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U4BIT_2WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (2, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U5BIT_2WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (2, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U6BIT_2WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (2, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U7BIT_2WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (2, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::BIT_2WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (2, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U1BIT_2WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (2, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U2BIT_2WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (2, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U3BIT_2WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (2, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U4BIT_2WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (2, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U5BIT_2WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (2, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U6BIT_2WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (2, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U7BIT_2WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (2, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::BIT_3WithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (3, registerA ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_3WithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (3, registerB ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_3WithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (3, registerC ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_3WithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (3, registerD ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_3WithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (3, registerE ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_3WithH)
{
	assert (parameters ().size () == 2);

	return (executeWith (3, registerH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_3WithL)
{
	assert (parameters ().size () == 2);

	return (executeWith (3, registerL ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_3WithIndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (3, valueAddressHL ()));
}

// ---
_INST_IMPL (FZ80::BIT_3WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (3, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U1BIT_3WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (3, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U2BIT_3WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (3, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U3BIT_3WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (3, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U4BIT_3WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (3, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U5BIT_3WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (3, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U6BIT_3WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (3, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U7BIT_3WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (3, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::BIT_3WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (3, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U1BIT_3WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (3, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U2BIT_3WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (3, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U3BIT_3WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (3, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U4BIT_3WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (3, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U5BIT_3WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (3, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U6BIT_3WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (3, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U7BIT_3WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (3, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::BIT_4WithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (4, registerA ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_4WithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (4, registerB ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_4WithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (4, registerC ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_4WithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (4, registerD ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_4WithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (4, registerE ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_4WithH)
{
	assert (parameters ().size () == 2);

	return (executeWith (4, registerH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_4WithL)
{
	assert (parameters ().size () == 2);

	return (executeWith (4, registerL ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_4WithIndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (4, valueAddressHL ()));
}

// ---
_INST_IMPL (FZ80::BIT_4WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (4, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U1BIT_4WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (4, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U2BIT_4WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (4, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U3BIT_4WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (4, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U4BIT_4WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (4, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U5BIT_4WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (4, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U6BIT_4WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (4, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U7BIT_4WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (4, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::BIT_4WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (4, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U1BIT_4WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (4, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U2BIT_4WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (4, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U3BIT_4WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (4, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U4BIT_4WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (4, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U5BIT_4WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (4, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U6BIT_4WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (4, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U7BIT_4WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (4, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::BIT_5WithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (5, registerA ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_5WithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (5, registerB ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_5WithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (5, registerC ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_5WithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (5, registerD ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_5WithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (5, registerE ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_5WithH)
{
	assert (parameters ().size () == 2);

	return (executeWith (5, registerH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_5WithL)
{
	assert (parameters ().size () == 2);

	return (executeWith (5, registerL ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_5WithIndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (5, valueAddressHL ()));
}

// ---
_INST_IMPL (FZ80::BIT_5WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (5, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U1BIT_5WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (5, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U2BIT_5WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (5, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U3BIT_5WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (5, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U4BIT_5WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (5, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U5BIT_5WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (5, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U6BIT_5WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (5, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U7BIT_5WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (5, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::BIT_5WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (5, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U1BIT_5WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (5, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U2BIT_5WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (5, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U3BIT_5WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (5, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U4BIT_5WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (5, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U5BIT_5WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (5, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U6BIT_5WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (5, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U7BIT_5WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (5, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::BIT_6WithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (6, registerA ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_6WithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (6, registerB ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_6WithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (6, registerC ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_6WithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (6, registerD ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_6WithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (6, registerE ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_6WithH)
{
	assert (parameters ().size () == 2);

	return (executeWith (6, registerH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_6WithL)
{
	assert (parameters ().size () == 2);

	return (executeWith (6, registerL ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_6WithIndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (6, valueAddressHL ()));
}

// ---
_INST_IMPL (FZ80::BIT_6WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (6, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U1BIT_6WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (6, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U2BIT_6WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (6, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U3BIT_6WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (6, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U4BIT_6WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (6, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U5BIT_6WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (6, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U6BIT_6WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (6, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U7BIT_6WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (6, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::BIT_6WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (6, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U1BIT_6WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (6, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U2BIT_6WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (6, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U3BIT_6WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (6, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U4BIT_6WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (6, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U5BIT_6WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (6, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U6BIT_6WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (6, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U7BIT_6WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (6, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::BIT_7WithA)
{
	assert (parameters ().size () == 2);

	return (executeWith (7, registerA ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_7WithB)
{
	assert (parameters ().size () == 2);

	return (executeWith (7, registerB ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_7WithC)
{
	assert (parameters ().size () == 2);

	return (executeWith (7, registerC ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_7WithD)
{
	assert (parameters ().size () == 2);

	return (executeWith (7, registerD ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_7WithE)
{
	assert (parameters ().size () == 2);

	return (executeWith (7, registerE ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_7WithH)
{
	assert (parameters ().size () == 2);

	return (executeWith (7, registerH ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_7WithL)
{
	assert (parameters ().size () == 2);

	return (executeWith (7, registerL ().values ()[0]));
}

// ---
_INST_IMPL (FZ80::BIT_7WithIndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (7, valueAddressHL ()));
}

// ---
_INST_IMPL (FZ80::BIT_7WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (7, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U1BIT_7WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (7, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U2BIT_7WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (7, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U3BIT_7WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (7, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U4BIT_7WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (7, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U5BIT_7WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (7, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U6BIT_7WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (7, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U7BIT_7WithIndirectIndexIX)
{
	assert (parameters ().size () == 4);

	return (executeWith (7, valueAddressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::BIT_7WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3 is not used. It is the last code of the instruction actually

	return (executeWith (7, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U1BIT_7WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (7, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U2BIT_7WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (7, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U3BIT_7WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (7, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U4BIT_7WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (7, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U5BIT_7WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (7, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U6BIT_7WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (7, valueAddressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::U7BIT_7WithIndirectIndexIY)
{
	assert (parameters ().size () == 4);

	return (executeWith (7, valueAddressIY (parameters ()[2].value ())));
}
