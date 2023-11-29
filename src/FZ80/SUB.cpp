#include <FZ80/Instructions.hpp>

// ---
bool FZ80::SUB_General::executeWith (const MCHEmul::UByte& v, bool c)
{
	MCHEmul::Register& r = registerA ();
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// The operation...
	bool a = c ? (st.bitStatus (FZ80::CZ80::_CARRYFLAG) ? true : false) : false;
	MCHEmul::UInt rst  = MCHEmul::UInt (r.values ()[0]).substract (MCHEmul::UInt (v), a);
	MCHEmul::UInt rstH = MCHEmul::UInt (r.values ()[0] & 0x0f).substract (MCHEmul::UInt (v & 0x0f), a); // Just to calculate the half borrow!
	r.set (rst.values ()); // and stored back...

	// How the flags are affected...
	st.setBitStatus (FZ80::CZ80::_CARRYFLAG, rst.carry ());
	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, true); // Always in substractions!
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, rst.overflow ());
	st.setBitStatus (FZ80::CZ80::_BIT3FLAG, rst [0].bit (3)); // Undocumented...
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, rstH [0].bit (4)); // When true, there will have been a half borrow!
	st.setBitStatus (FZ80::CZ80::_BIT5FLAG, rst [0].bit (5)); // Undocumented...
	st.setBitStatus (FZ80::CZ80::_ZEROFLAG, rst == MCHEmul::UInt::_0);
	st.setBitStatus (FZ80::CZ80::_SIGNFLAG, rst.negative ());

	return (true);
}

// ---
bool FZ80::SUB_General::executeWith (const MCHEmul::UBytes& v)
{
	MCHEmul::RefRegisters& r = registerHL ();
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// The operation...
	MCHEmul::UInt rst  = MCHEmul::UInt ({ r [0] -> values ()[0], r [1] -> values ()[0] }).
		substract (MCHEmul::UInt ({ v [0], v [1] }, st.bitStatus (FZ80::CZ80::_CARRYFLAG)));
	MCHEmul::UInt rstH = MCHEmul::UInt ({ r [0] -> values ()[0] & 0x0f, r [1] -> values ()[1] }).
		substract (MCHEmul::UInt ({ v [0] & 0x0f, v [1] }, st.bitStatus (FZ80::CZ80::_CARRYFLAG))); // half carry from bit 11 to 12?
	r [0] -> set ({ rst.values ()[0] }); r [1] -> set ({ rst.values ()[1] }); 

	// How the flags are affected...
	st.setBitStatus (FZ80::CZ80::_CARRYFLAG, rst.carry ());
	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, true); // Always! 
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, rst.overflow ());
	st.setBitStatus (FZ80::CZ80::_BIT3FLAG, rst [0].bit (3)); // a copy of the status of the bit 3... but undocumented!
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, rstH [0].bit (4 /** Bit 12 within the total. */)); 
	st.setBitStatus (FZ80::CZ80::_BIT5FLAG, rst [0].bit (5)); // a copy of the status of the bit 3... but undocumented!
	st.setBitStatus (FZ80::CZ80::_ZEROFLAG, rst == MCHEmul::UInt::_0);
	st.setBitStatus (FZ80::CZ80::_SIGNFLAG, rst.negative ());

	return (true);
}

// ---
_INST_IMPL (FZ80::SUB_AWithA)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerA ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithB)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerB ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithC)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerC ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithD)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerD ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::SUB_AWithE)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

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
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerA ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithB)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerB ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithC)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerC ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithD)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerD ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::SBC_AWithE)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

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
