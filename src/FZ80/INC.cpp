#include <FZ80/Instructions.hpp>

// ---
bool FZ80::INC_General::executeWith (MCHEmul::Register& r)
{
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// The operation...
	MCHEmul::UInt v	 (r.values ()[0]); // 1 byte long...
	MCHEmul::UInt hv (r.values ()[0] & 0x0f);
	bool i7F = (v == MCHEmul::UInt (MCHEmul::UByte (0x7f)));
	v  += MCHEmul::UInt::_1; // Increment...
	hv += MCHEmul::UInt::_1; // ..just to see whether there is hafl carry!
	r.set (v.bytes ()); // Put back the info

	// How the flags are affected...
	// The carry flag is not modified...
	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, false); // Just on when the last instruction is a substract!
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, v.overflow ());
	st.setBitStatus (FZ80::CZ80::_BIT3FLAG, v [0].bit (3)); // Undocumented...
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, hv [0].bit (4)); // If true, there will have been half carry!
	st.setBitStatus (FZ80::CZ80::_BIT5FLAG, v [0].bit (5)); // Undocumented...
	st.setBitStatus (FZ80::CZ80::_ZEROFLAG, v == MCHEmul::UByte::_0);
	st.setBitStatus (FZ80::CZ80::_SIGNFLAG, v.negative ());

	return (true);
}

// ---
bool FZ80::INC_General::executeWith (MCHEmul::RefRegisters& r)
{
	// The operation...
	MCHEmul::UInt v	({ r [0] -> values ()[0], r [1] -> values ()[0] });
	v  += MCHEmul::UInt::_1; // Increment...
	r [0] -> set ({ v [0] }); // Put back the info
	r [1] -> set ({ v [1] });

	// No flags affected

	return (true);
}

// ---
bool FZ80::INC_General::executeWith (const MCHEmul::Address& a)
{
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// The operation...
	MCHEmul::UInt v (memory () -> value (a));
	bool i7F = (v == MCHEmul::UInt (MCHEmul::UByte (0x7f)));
	v += 1; // Increment...
	memory () -> set (a, v.values ()[0]);

	// How the flags are affected...
	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, false);
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, i7F); // Only if it was 7F before the operation...
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, true);
	st.setBitStatus (FZ80::CZ80::_ZEROFLAG, v == MCHEmul::UByte::_0);
	st.setBitStatus (FZ80::CZ80::_SIGNFLAG, v.negative ());

	return (true);
}

// ---
_INST_IMPL (FZ80::INC_A)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerA ()));
}

// ---
_INST_IMPL (FZ80::INC_B)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerB ()));
}

// ---
_INST_IMPL (FZ80::INC_C)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerC ()));
}

// ---
_INST_IMPL (FZ80::INC_D)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerD ()));
}

// ---
_INST_IMPL (FZ80::INC_E)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerE ()));
}

// ---
_INST_IMPL (FZ80::INC_H)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerH ()));
}

// ---
_INST_IMPL (FZ80::INC_L)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerL ()));
}

// ---
_INST_IMPL (FZ80::INC_IndirectHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (addressHL ()));
}

// ---
_INST_IMPL (FZ80::INC_IndirectIndexIX)
{
	assert (parameters ().size () == 3);

	return (executeWith (addressIX (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::INC_IndirectIndexIY)
{
	assert (parameters ().size () == 3);

	return (executeWith (addressIY (parameters ()[2].value ())));
}

// ---
_INST_IMPL (FZ80::INC_BC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerBC ()));
}

// ---
_INST_IMPL (FZ80::INC_DE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerDE ()));
}

// ---
_INST_IMPL (FZ80::INC_HL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerHL ()));
}

// ---
_INST_IMPL (FZ80::INC_SP)
{
	assert (parameters ().size () == 1);

	memory () -> stack () -> setPosition (memory () -> stack () -> position () + 1);
	
	return (true);
}

// ---
_INST_IMPL (FZ80::INC_IX)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIX ()));
}

// ---
_INST_IMPL (FZ80::INC_IY)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIY ()));
}

// ---
_INST_IMPL (FZ80::INC_IXH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXH ()));
}

// ---
_INST_IMPL (FZ80::INC_IXL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIXL ()));
}

// ---
_INST_IMPL (FZ80::INC_IYH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYH ()));
}

// ---
_INST_IMPL (FZ80::INC_IYL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIYL ()));
}
