#include <FZ80/ROTATELEFT.hpp>

// ---
std::vector <MCHEmul::UByte> FZ80::ROTATELeft_Index::shapeCodeWithData
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
_INST_IMPL (FZ80::RLC_A)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), false));
}

// ---
_INST_IMPL (FZ80::RLC_ASlower)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), false));
}

// ---
_INST_IMPL (FZ80::RLC_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), false));
}

// ---
_INST_IMPL (FZ80::RLC_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), false));
}

// ---
_INST_IMPL (FZ80::RLC_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), false));
}

// ---
_INST_IMPL (FZ80::RLC_E)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), false));
}

// ---
_INST_IMPL (FZ80::RLC_H)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerH (), false));
}

// ---
_INST_IMPL (FZ80::RLC_L)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerL (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (addressHL (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerA (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerB (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerC (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerD (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerE (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerH (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerL (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerA (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerB (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerC (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerD (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerE (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerH (), false));
}

// ---
_INST_IMPL (FZ80::RLC_IndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerL (), false));
}

// ---
_INST_IMPL (FZ80::RL_A)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), true));
}

// ---
_INST_IMPL (FZ80::RL_ASlower)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), true));
}

// ---
_INST_IMPL (FZ80::RL_B)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerB (), true));
}

// ---
_INST_IMPL (FZ80::RL_C)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerC (), true));
}

// ---
_INST_IMPL (FZ80::RL_D)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerD (), true));
}

// ---
_INST_IMPL (FZ80::RL_E)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerE (), true));
}

// ---
_INST_IMPL (FZ80::RL_H)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerH (), true));
}

// ---
_INST_IMPL (FZ80::RL_L)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerL (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (addressHL (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIX)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIXCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerA (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIXCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerB (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIXCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerC (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIXCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerD (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIXCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerE (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIXCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerH (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIXCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIX ((char) parameters ()[2].value ()), registerL (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIY)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIYCopyA)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerA (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIYCopyB)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerB (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIYCopyC)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerC (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIYCopyD)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerD (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIYCopyE)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerE (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIYCopyH)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerH (), true));
}

// ---
_INST_IMPL (FZ80::RL_IndirectIndexIYCopyL)
{
	assert (parameters ().size () == 4);

	// The parameter 3rd is not used...

	return (executeWith (addressIY ((char) parameters ()[2].value ()), registerL (), true));
}

// ---
_INST_IMPL (FZ80::RLD)
{
	assert (parameters ().size () == 2);

	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	const MCHEmul::UByte& v  = valueAddressHL ();
	const MCHEmul::UByte& a  = valueRegisterA ();
	MCHEmul::UByte nA ((a.value () & 0xf0) | ((v.value () & 0xf0) >> 4));
	MCHEmul::UByte nV (((v.value() & 0x0f) << 4) | (a.value () & 0x0f));
	memory () -> set (addressHL (), nV);
	registerA ().set ({ nA });

	// How flags are accepted...
	// Carry flag is nor accepted!
	st.setBitStatus (CZ80::_NEGATIVEFLAG, false); // Always!
	st.setBitStatus (CZ80::_PARITYOVERFLOWFLAG, (nA.numberBitsOn () % 2) == 0);
	st.setBitStatus (CZ80::_BIT3FLAG, nA.bit (3)); // Undocumented...
	st.setBitStatus (CZ80::_HALFCARRYFLAG, false);
	st.setBitStatus (CZ80::_BIT5FLAG, nA.bit (5)); // Undocumented...
	st.setBitStatus (CZ80::_ZEROFLAG, nA == MCHEmul::UByte::_0);
	st.setBitStatus (CZ80::_SIGNFLAG, nA.bit (7));

	return (true);
}
