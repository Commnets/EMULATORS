#include <FZ80/ROTATERIGHT.hpp>

// ---
std::vector <MCHEmul::UByte> FZ80::ROTATERight_Index::shapeCodeWithData
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
_INST_IMPL (FZ80::RRC_A)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), false));
}

// ---
_INST_IMPL (FZ80::RRC_ASlower)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), false));
}

// ---
_INST_IMPL (FZ80::RRC_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), false));
}

// ---
_INST_IMPL (FZ80::RRC_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), false));
}

// ---
_INST_IMPL (FZ80::RRC_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), false));
}

// ---
_INST_IMPL (FZ80::RRC_E)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), false));
}

// ---
_INST_IMPL (FZ80::RRC_F)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerF (), false));
}

// ---
_INST_IMPL (FZ80::RRC_L)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerL (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (addressHL (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerA (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerB (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerC (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerD (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerE (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerH (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerL (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerA (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerB (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerC (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerD (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerE (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerH (), false));
}

// ---
_INST_IMPL (FZ80::RRC_IndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerL (), false));
}

// ---
_INST_IMPL (FZ80::RR_A)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), true));
}

// ---
_INST_IMPL (FZ80::RR_ASlower)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), true));
}

// ---
_INST_IMPL (FZ80::RR_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), true));
}

// ---
_INST_IMPL (FZ80::RR_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), true));
}

// ---
_INST_IMPL (FZ80::RR_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), true));
}

// ---
_INST_IMPL (FZ80::RR_E)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), true));
}

// ---
_INST_IMPL (FZ80::RR_F)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerF (), true));
}

// ---
_INST_IMPL (FZ80::RR_L)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerL (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (addressHL (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerA (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerB (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerC (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerD (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerE (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerH (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX (parameters ()[3].value ()), registerL (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerA (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerB (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerC (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerD (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerE (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerH (), true));
}

// ---
_INST_IMPL (FZ80::RR_IndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY (parameters ()[3].value ()), registerL (), true));
}

// ---
_INST_IMPL (FZ80::RRD)
{
	assert (parameters ().size () == 2);

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	const MCHEmul::UByte& v  = valueAddressHL ();
	const MCHEmul::UByte& a  = valueRegisterA ();
	MCHEmul::UByte nA = (a & 0xf0) | (v & 0x0f);
	MCHEmul::UByte nV = ((a & 0x0f) << 4) | ((v & 0xf0) >> 4);
	memory () -> set (addressHL (), nV);
	registerA ().set ({ nA });

	// How flags are accepted...
	// Carry flag is nor accepted!
	st.setBitStatus (CZ80::_NEGATIVEFLAG, false); // Always!
	st.setBitStatus (CZ80::_PARITYOVERFLOWFLAG, (nA.numberBitsOn () % 2) == 0);
	st.setBitStatus (CZ80::_BIT3FLAG, nA.bit (3)); // Undocumented...
	st.setBitStatus (CZ80::_HALFCARRYFLAG, false);
	st.setBitStatus (CZ80::_BIT5FLAG, nA.bit (5)); // Undocumented...
	st.setBitStatus (CZ80::_NEGATIVEFLAG, nA == MCHEmul::UByte::_0);
	st.setBitStatus (CZ80::_SIGNFLAG, nA.bit (7));

	return (true);
}
