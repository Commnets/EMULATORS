#include <FZ80/Instructions.hpp>

// ---
bool FZ80::ADD_General::executeWith (MCHEmul::Register& r, const MCHEmul::UByte& v, bool c)
{
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// The operation...
	bool a = c ? (st.bitStatus (FZ80::CZ80::_CARRYFLAG) ? true : false) : false;
	MCHEmul::UInt rst  = MCHEmul::UInt (r.values ()[0]).add (MCHEmul::UInt (v, a));
	MCHEmul::UInt rstH = MCHEmul::UInt (r.values ()[0] & 0x0f).add (MCHEmul::UInt (v & 0x0f, a)); // Just to calculate the half carry! (from 3 to 4)
	r.set (rst.values ()); // and stored back...

	// How the flags are affected...
	st.setBitStatus (FZ80::CZ80::_CARRYFLAG, rst.carry ());
	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, false); // Always!
	st.setBitStatus (FZ80::CZ80::_PARITYOVERFLOWFLAG, rst.overflow ());
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, rstH [0].bit (4)); // When true, there will have been a half carry from bit 3 to 4!
	st.setBitStatus (FZ80::CZ80::_ZEROFLAG, rst == MCHEmul::UInt::_0);
	st.setBitStatus (FZ80::CZ80::_SIGNFLAG, rst.negative ());

	return (true);
}

// ---
bool FZ80::ADD_General::executeWith (MCHEmul::RefRegisters& r, const MCHEmul::UBytes& v, bool c)
{
	MCHEmul::StatusRegister& st = cpu () -> statusRegister ();

	// The operation...
	bool a = c ? (st.bitStatus (FZ80::CZ80::_CARRYFLAG) ? true : false) : false;
	MCHEmul::UInt rst  = MCHEmul::UInt ({ r [0] -> values ()[0], r [1] -> values ()[0] }).
		add (MCHEmul::UInt ({ v [0], v [1] }, a));
	MCHEmul::UInt rstH = MCHEmul::UInt ({ r [0] -> values ()[0] & 0x0f, r [1] -> values ()[0] }).
		add (MCHEmul::UInt ({ v [0] & 0x0f, v [1] }, a)); // Just to calculate the half carry in bit 11 to 12!
	r [0] -> set ({ rst.values ()[0] }); r [1] -> set ({ rst.values ()[1] }); 

	// How the flags are affected...
	st.setBitStatus (FZ80::CZ80::_CARRYFLAG, rst.carry ());
	st.setBitStatus (FZ80::CZ80::_NEGATIVEFLAG, false); // Always!
	st.setBitStatus (FZ80::CZ80::_HALFCARRYFLAG, rstH [0].bit (4 /** Bit 12 within the total. */)); 

	return (true);
}

// ---
_INST_IMPL (FZ80::ADD_AWithA)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerA (), registerA ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithB)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerA (), registerB ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithC)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerA (), registerC ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithD)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerA (), registerD ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithE)
{
	assert (parameters ().size () == 1 ||parameters ().size () == 2);
	// Because in the non documented instructions the instruction code is 2 bytes long

	return (executeWith (registerA (), registerE ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithH)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerH ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerL ().values ()[0], false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithIndirectHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), valueAddressHL (), false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithIndirectIndexIX)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerA (), valueAddressIX (parameters ()[2].value ()), false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithIndirectIndexIY)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerA (), valueAddressIY (parameters ()[2].value ()), false));
}

// ---
_INST_IMPL (FZ80::ADD_A)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), parameters ()[1].value (), false));
}

// ---
_INST_IMPL (FZ80::ADD_HLWithBC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerHL (), valueRegisterBC (), false));
}

// ---
_INST_IMPL (FZ80::ADD_HLWithDE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerHL (), valueRegisterDE (), false));
}

// ---
_INST_IMPL (FZ80::ADD_HLWithHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerHL (), valueRegisterHL (), false));
}

// ---
_INST_IMPL (FZ80::ADD_HLWithSP)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerHL (), valueRegisterSP (), false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithIXH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), valueRegisterIXH (), false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithIXL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), valueRegisterIXL (), false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithIYH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), valueRegisterIYH (), false));
}

// ---
_INST_IMPL (FZ80::ADD_AWithIYL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), valueRegisterIYL (), false));
}

// ---
_INST_IMPL (FZ80::ADD_IXWithBC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIX (), valueRegisterBC (), false));
}

// ---
_INST_IMPL (FZ80::ADD_IXWithDE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIX (), valueRegisterDE (), false));
}

// ---
_INST_IMPL (FZ80::ADD_IXWithIX)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIX (), valueRegisterIX (), false));
}

// ---
_INST_IMPL (FZ80::ADD_IXWithSP)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIX (), valueRegisterSP (), false));
}

// ---
_INST_IMPL (FZ80::ADD_IYWithBC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIY (), valueRegisterBC (), false));
}

// ---
_INST_IMPL (FZ80::ADD_IYWithDE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIY (), valueRegisterDE (), false));
}

// ---
_INST_IMPL (FZ80::ADD_IYWithIY)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIY (), valueRegisterIX (), false));
}

// ---
_INST_IMPL (FZ80::ADD_IYWithSP)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerIY (), valueRegisterSP (), false));
}

// ---
_INST_IMPL (FZ80::ADC_AWithA)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerA ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithB)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerB ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithC)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerC ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithD)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerD ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithE)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerE ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithH)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerH ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), registerL ().values ()[0], true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithIndirectHL)
{
	assert (parameters ().size () == 1);

	return (executeWith (registerA (), valueAddressHL (), true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithIndirectIndexIX)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerA (), valueAddressIX (parameters ()[2].value ()), true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithIndirectIndexIY)
{
	assert (parameters ().size () == 3);

	return (executeWith (registerA (), valueAddressIY (parameters ()[2].value ()), true));
}

// ---
_INST_IMPL (FZ80::ADC_A)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), parameters ()[1].value (), true));
}

// ---
_INST_IMPL (FZ80::ADC_HLWithBC)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerHL (), valueRegisterBC (), true));
}

// ---
_INST_IMPL (FZ80::ADC_HLWithDE)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerHL (), valueRegisterDE (), true));
}

// ---
_INST_IMPL (FZ80::ADC_HLWithHL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerHL (), valueRegisterHL (), true));
}

// ---
_INST_IMPL (FZ80::ADC_HLWithSP)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerHL (), valueRegisterSP (), true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithIXH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), valueRegisterIXH (), true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithIXL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), valueRegisterIXL (), true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithIYH)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), valueRegisterIYH (), true));
}

// ---
_INST_IMPL (FZ80::ADC_AWithIYL)
{
	assert (parameters ().size () == 2);

	return (executeWith (registerA (), valueRegisterIYL (), true));
}
